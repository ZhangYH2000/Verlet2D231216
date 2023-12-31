#include "cur.h"
#include "ui.h"
#include "bgr.h"
#include "par.h"
#include "fpn_body.h"
#include "fpn_link.h"
#include "sqr.h"
#include "test.h"
#include "body.h"
#include "coll.h"
#include "poly.h"
#include "link.h"
#include "clip.h"
#include "creator.h"
#include "mass_pnt.h"
#include "construct.h"

// 小球存储使用连续内存地址可能会好一点?
// 因为我是 4 核，所以这么写。
int n_threads = max<int>(4, std::thread::hardware_concurrency());
vector<ptr<thread>> pool;
mutex mt_done, mt_ready;
mutex mt_task;
condition_variable cv_done, cv_ready;
volatile int n_working = 0;
int mode = 0;
vector<int> ready;
bool bye = false;

vector<ptr<clip>> bgms;
int id_bgm = 0;

// 我忘记如何说明这里不会有死锁了。
// 我也不知道是不是真的不会有死锁。
void StartThread(int m) {
	mode = m;
	{
		lock_guard<mutex> lc0(mt_ready);
		lock_guard<mutex> lc1(mt_done);
		ready = vector<int>(n_threads, 1);
		n_working = n_threads;
	}
	cv_ready.notify_all();
	unique_lock<mutex> lc(mt_done);
	cv_done.wait(lc, [] { return n_working == 0; });
}
void EndThread() {
	{
		lock_guard<mutex> lc0(mt_ready);
		lock_guard<mutex> lc1(mt_done);
		ready = vector<int>(n_threads, 1);
	}
	bye = true;
	cv_ready.notify_all();
}
void CollideThread(Cur& cur, int id) {
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	while (true) {
		{
			unique_lock<mutex> lc(mt_ready);
			cv_ready.wait(lc, [id] { return ready[id]; });
			ready[id] = 0;
		}

		if (bye) { return; }

		int i0 = mode + 2 * id;
		for (int i = i0; i < cur.nx_grid; i += n_threads * 2) {
			rep(j, 0, cur.ny_grid) { CollGrid(cur, i, j); }
		}

		bool ok = false;
		{
			lock_guard<mutex> lc(mt_done);
			--n_working;
			ok = n_working == 0;
		}
		if (ok) { cv_done.notify_all(); }
		std::this_thread::yield();
	}
}

Cur::Cur() {
	w = 1800; h = 900; set_locale();
	wv.len_block *= 8;
	print_console(L"正在加载字体...");
	App::Init();
	print_console(L"字体加载完成.", true);
	print_console(L"正在加载控件...");
	// 这些构造函数明明可以不传参的。而且传参会传自己的引用带来问题。
	mkp(ui)(*this); ui->Init(*this);
	mkp(bgr)(*this); Reset();
	print_console(L"控件加载完成.", true);

	print_console(L"正在加载音乐...");
	rep(i, 0, 2) {
		auto path = L"music/bgm" + tw(i) + L".clip";
		auto cl = msh<clip>(path);
		bgms.push_back(cl);
	}
	print_console(L"音乐加载完成.", true);

	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	
	// 下面传了引用！那么初始化 Cur的时候就不要有复制了。
	// 再版的时候要规范化。
	ready = vector<int>(n_threads, 0);
	rep(i, 0, n_threads) {
		pool.push_back(msh<thread>(CollideThread, std::ref(*this), i));
	}
}

#define cur (*this)
#include "my_def.h"

#define found(s) (dic.find(s) != dic.end())
#define getv(nm) if (found(L#nm)) { nm = dic.at(L#nm)->num; }
#define getp(nm) if (found(L#nm)) { nm = Num(cur, *dic.at(L#nm)); }

void Cur::Save(wstring const& nm) {
	FILE* f = wfopen(nm, L"wb");
	if (!f) { return; }
	int sz = 0; fwtv(cmd); save_par(f);

	remove_dead();

	sz = par_refs.size(); fwt(sz);
	for (auto& p : par_refs) { p.second->save(f); }

	// 感觉哪些参数应该保存，哪些参数应该重置也挺难决定的。
	// 如何让版本更新不影响保存和读取也是个问题。
	// 我还是不保存参数了吧。只要是不变的参数，可以用文本设置的，就不保存。
	// 其实有点怪，仍然保存了 ParRef 和 MassPnt

	fwt(t);
	sz = mass_pnts.size(); fwt(sz);
	for (auto& mp : mass_pnts) { mp->save(f); }
	sz = sqrs.size(); fwt(sz);
	rep(i, 0, sz) { sqrs[i]->id = i; }
	sz = bs.size(); fwt(sz);
	rep(i, 0, sz) { bs[i]->id = i; }
	for (auto& b : bs) { b->save(f); }
	sz = lks.size(); fwt(sz);
	rep(i, 0, sz) { lks[i]->id = i; }
	for (auto& lk : lks) { lk->save(f); }
	sz = polys.size(); fwt(sz);
	for (auto& p : polys) { p->save(f); }

	sz = shapes.size(); fwt(sz);
	for (auto& sh : shapes) {
		fwtv(sh.first);
		auto& bs = *sh.second;
		fwtv(bs);
	}

	fclose(f);
}
void Cur::Load(wstring const& nm) {
	FILE* f = wfopen(nm, L"rb");
	if (!f) { return; } Reset();
	int sz = 0; frdv(cmd); 
	tmp_cmd = cmd; load_par(f);

	frd(sz);
	rep(i, 0, sz) {
		auto pr = msh<ParRef>(cur, f);
		par_refs[pr->nm] = pr;
	}
	frd(t);
	frd(sz);
	rep(i, 0, sz) {
		mass_pnts.push_back(msh<mass_pnt>(cur, f));
	}
	frd(sz);
	rep(i, 0, sz) { sqrs.push_back(msh<Sqr>()); }
	frd(sz);
	rep(i, 0, sz) { bs.push_back(msh<Body>(cur, f)); }
	frd(sz);
	rep(i, 0, sz) { lks.push_back(msh<Link>(cur, f)); }
	for (auto& lk : lks) { lk->on_load(cur); }
	frd(sz);
	rep(i, 0, sz) { polys.push_back(msh<Poly>(cur, f)); }

	frd(sz);
	rep(i, 0, sz) {
		wstring nm;
		frdv(nm);
		vector<int> bs;
		frdv(bs);
		shapes[nm] = msh<vector<int>>(bs);
	}

	Execute(gl, Compile(cmd)); fclose(f);
}
void Cur::Reset() {
	gl.clear(); init_def_fun(); pars.clear();

	n_step = 8;
	rdt = 1.0 / 60;
	gx = 0; gy = 1200;

	bs.clear();
	lks.clear();
	polys.clear();
	sqrs.clear();
	
	skip_friction = false;
	s_grid = 8;

	mkp(crt)();
	par_refs.clear();
	shapes.clear();
	t = 0;
	body_sel = NULL;
	link_sel = NULL;

	n_track = 1000;
	mult_hue_track = 0.5;
	circle_bd = false;
	r_drag = 40;
	shuffle_links = false;
	no_grid_coll = false;
	
	mult_energy = 2e-5;
	mult_velocity = 1e-1;
	mult_force = 1e-5;

	wind = false;
	p_wind = 0.05;
	x_wind = 0.88;

	mass_pnts.clear();
}

std::random_device seed_gen;
std::mt19937 engine(seed_gen());
void Cur::Update() {
	no_break = false;

	bgr.PreUpdate(*this);
	for (auto& b : bs) { b->PreUpdate(cur); }
	for (auto& p : polys) { p->PreUpdate(cur); }
	crt->PreUpdate(cur);
	ui.PreUpdate(*this);
	basic_update();

	sdt = rdt / n_step;
	nx_grid = bgr.w / s_grid + 1;
	ny_grid = bgr.h / s_grid + 1;

	if (!bgms.empty()) {
		auto& cl = *bgms[id_bgm];
		if (cl.csp >= cl.n()) {
			cl.csp = 0;
			id_bgm = (id_bgm + 1) % bgms.size();
		}

		if (!mute) {
			auto& cl = *bgms[id_bgm];
			cl.play(wv.wvin);
		}
	}
	if (!kb) {
		if (kbc(L' ')) { paused = !paused; }
		if (kbc(L'D')) { dark_bd = !dark_bd; }
		if (kbc(L'B')) { render_mode = (render_mode + 1) % 3; }
		if (kbc(L'V')) { show_velocity = !show_velocity; }
		if (kbc(L'K')) { show_kinetic = !show_kinetic; }
		if (kbc(L'L')) { show_force = !show_force; }
		if (kbc(L'A')) { ranged_drag = !ranged_drag; }
		if (kbc(L'W')) { wind = !wind; }
		if (kbc(L'Z')) { show_track = !show_track; }
		if (kbc(L'O')) { crt->mode = MODE_SLCT; crt->Discard(cur); }
		if (kbc(L'R')) { crt->mode = MODE_DRAG; crt->Discard(cur); }
		if (kbc(L'P')) { crt->mode = MODE_POLY; crt->Discard(cur); }
		if (kbc(L'C')) { crt->mode = MODE_CONN; crt->Discard(cur); }
		if (kbc(L'T')) { crt->mode = MODE_ROPE; crt->Discard(cur); }
		if (kbc(L'S')) { crt->mode = MODE_SBOX; crt->Discard(cur); }
		if (kbc(L'M')) { crt->mode = MODE_FSHP; crt->Discard(cur); }
		if (kbc(L'X')) { crt->mode = MODE_FBOX; crt->Discard(cur); }
		if (kbc(L'N')) { crt->mode = MODE_KNFE; crt->Discard(cur); }
		if (kbc(L'G')) { crt->mode = MODE_DELE; crt->Discard(cur); }

		if (kbd[L'H']) { no_break = true; }
	}

	bgr.Update(*this);
	UpdateParRefs(cur);		

	if (!paused) rep(i, 0, n_step) {
		t += sdt;
		for (auto& b : bs) { b->Step(cur); }
		for (auto& b : bs) { CollBd(cur, *b); }
		for (auto& p : polys) { p->Step(cur); }

		grid.clear(); giants.clear();
		grid.resize(nx_grid * ny_grid);
		for (auto& b : bs) { b->register_grid(cur); }

		if (!no_grid_coll) {
			//for (int i = 0; i < nxgd; i += 2) rep(j, 0, nygd) { CollGrid(cur, i, j); }
			//for (int i = 1; i < nxgd; i += 2) rep(j, 0, nygd) { CollGrid(cur, i, j); }
			StartThread(0); StartThread(1);
		}
		for (auto& b : giants) { CollGiantGd(cur, *b); }
		CollGiants(cur);

		if (shuffle_links && frnd(1) < 0.1) {
			std::shuffle(lks.begin(), lks.end(), engine);
			//for (auto it = lks.rbegin(); it != lks.rend(); ++it) { LinkColl(cur, **it); }
			//for (auto it = lks.rbegin(); it != lks.rend(); ++it) { (*it)->Resolve(cur); }
		}
		for (auto& lk : lks) { LinkColl(cur, *lk); }
		for (auto& lk : lks) { lk->Resolve(cur); }
		for (auto& p : polys) { CollPoly(cur, *p); }
	}

	remove_dead();
	for (auto& b : bs) { b->Update(cur); }
	for (auto& lk : lks) { lk->Update(cur); }
	for (auto& p : polys) { p->Update(cur); }
	crt->Update(cur);
	for (auto& mp : mass_pnts) { mp->Render(cur); }
	for (auto& lk : lks) { lk->Render(cur); }
	for (auto& b : bs) { b->Render(cur); }
	for (auto& p : polys) { p->Render(cur); }
	crt->Render(cur);
	ui.Update(*this);
}
void Cur::Exit() {
	EndThread();
	for (auto& t : pool) { t->join(); }
}

void Cur::set_bsel(Body* b) {
	body_sel = b;
	ui.fpn_body->Discard(cur);
}
void Cur::set_lksel(Link* lk) {
	link_sel = lk;
	ui.fpn_link->Discard(cur);
}
void Cur::remove_dead() {
	for (auto& b : bs) { b->check_dead(); }
	for (auto& lk : lks) { lk->check_dead(); }
	if (body_sel && body_sel->dead) { set_bsel(NULL); }
	if (link_sel && link_sel->dead) { set_lksel(NULL); }

	for (auto& b : bs) if (b->dead) { b->Discard(cur); }
	for (auto& p : polys) if (p->dead) { p->Discard(cur); }

	bs.erase(remove_if(bs.begin(), bs.end(),
		[](ptr<Body> b) { return b->dead; }), bs.end());
	lks.erase(remove_if(lks.begin(), lks.end(),
		[](ptr<Link> lk) { return lk->dead; }), lks.end());
	sqrs.erase(remove_if(sqrs.begin(), sqrs.end(),
		[](ptr<Sqr> s) { return s->dead; }), sqrs.end());
	polys.erase(remove_if(polys.begin(), polys.end(),
		[](ptr<Poly> p) { return p->dead; }), polys.end());
}

void Cur::save_par(FILE* f) const {
	int sz = pars.size(); fwt(sz);
	for (auto p : pars) { p->save(f); }
}
void Cur::load_par(FILE* f) {
	int sz = 0; frd(sz);
	rep(i, 0, sz) { pars.push_back(msh<param>(f)); }
}
void Cur::set_cfg(Var const &v) {
	auto& dic = v.dic;
	getv(vol);
	getv(wind);
	getv(r_drag);
	getv(circle_bd);
	getv(no_grid_coll); getv(shuffle_links);
	getv(skip_friction); 
	getp(gx); getp(gy);
	getp(mult_energy); getp(mult_velocity); getp(mult_force);
	getp(p_wind); getp(x_wind); getp(y_wind); 
	getp(mult_hue_track);
	
	// 这些地方要有异常检查的。
	getv(s_grid); getv(n_step); getv(n_track);

	if (found(L"mass_pnts")) {
		mass_pnts.clear();
		auto& vec = dic.at(L"mass_pnts")->vec;
		for (auto& v : vec) {
			mass_pnts.push_back(msh<mass_pnt>(*v));
		}
	}
}
void Cur::init_def_fun() {
	{
		auto f = [this](vector<ptr<Var>> const& in) {
			if (in.size() >= 1) { AddPar(cur, *in[0]); }
			return msh<Var>();
		};
		gl[L"add_par"] = msh<Var>(f);
	}
	{
		auto f = [this](vector<ptr<Var>> const& in) {
			if (in.size() >= 1) { crt->refresh(cur, *in[0]); }
			return msh<Var>();
		};
		gl[L"set_creator"] = msh<Var>(f);
	}
	{
		auto f = [this](vector<ptr<Var>> const& in) {
			if (in.size() >= 1) { set_cfg(*in[0]); }
			return msh<Var>();
		};
		gl[L"set_cfg"] = msh<Var>(f);
	}
	{
		auto f = [this](vector<ptr<Var>> const& in) {
			if (in.size() >= 2) { MakePoly(cur, *in[0], *in[1]); }
			return msh<Var>();
		};
		gl[L"make_poly"] = msh<Var>(f);
	}
	{
		auto f = [this](vector<ptr<Var>> const& in) {
			if (in.size() >= 1) { MakeLink(cur, *in[0]); }
			return msh<Var>();
		};
		gl[L"make_link"] = msh<Var>(f);
	}
	{
		auto f = [this](vector<ptr<Var>> const& in) {
			if (in.size() >= 4) { 
				MakePendulum(cur, tv2(*in[0]), in[1]->num, *in[2], *in[3]);
			} return msh<Var>();
		};
		gl[L"make_pendulum"] = msh<Var>(f);
	}
}
void Cur::basic_update() {
	title = loc(L"title");
	if (gl.find(L"dbstr") != gl.end()) { dbstr = gl[L"dbstr"]->str; }
	if (gl.find(L"update") != gl.end()) {
		auto f = gl[L"update"];  Execute(gl, f->procs);
	}

	pars.erase(remove_if(pars.begin(), pars.end(),
		[](ptr<param> p) { return p->del; }), pars.end());
	for (auto& p : pars) { gl[p->nm] = msh<Var>(p->val); }
}
