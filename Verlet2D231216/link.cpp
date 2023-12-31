#include "link.h"
#include "cur.h"
#include "var.h"
#include "bgr.h"
#include "sqr.h"
#include "body.h"
#include "creator.h"
#include "draw_px_seg.h"

#include "my_def.h"

#define found(s) (dic.find(s) != dic.end())
#define getv(nm) if (found(L#nm)) { nm = dic.at(L#nm)->num; }
#define getp(nm) if (found(L#nm)) { nm = Num(cur, *dic.at(L#nm)); }

#define frdn(nm) nm = Num(cur, f);

double constexpr dep = 0;

Link::Link() {
	len = 10;
	stiff = 4e6;
	hooke = 1e6;
	r = 4;
	c_normal = dcol(200);
}
Link::Link(Cur& cur, FILE* f) {
	int sz = 0;
	int b = 0;
	frd(b); 
	if (b >= 0) { b0 = &*cur.bs[b]; }
	frd(b);
	if (b >= 0) { b1 = &*cur.bs[b]; }
	frd(rope); frd(solid);
	frd(spring); frd(breakable);
	frdn(len); frdn(damp);
	frdn(hooke); frdn(stiff);
	frdn(r);
	frd(c_normal);
	frd(aux);
	frd(id_f0); 
	frd(id_f1);
	frd(id_f2);
	frd(id_f3);
	frd(square);
	frd(b);
	if (b >= 0) { s0 = &*cur.sqrs[b]; }
	frd(b);
	if (b >= 0) { s1 = &*cur.sqrs[b]; }
	frdv(cmd);
	tmp_cmd = cmd;
	refresh(cur);
	init();
}
void Link::save(FILE* f) {
	int sz = 0;
	int b = 0;
	b = b0 ? b0->id : -1; fwt(b);
	b = b1 ? b1->id : -1; fwt(b);
	fwt(rope); fwt(solid);
	fwt(spring); fwt(breakable);
	len.save(f); damp.save(f);
	hooke.save(f); stiff.save(f);
	r.save(f);
	fwt(c_normal);
	fwt(aux);
	b = f0 ? f0->id : -1; fwt(b);
	b = f1 ? f1->id : -1; fwt(b);
	b = f2 ? f2->id : -1; fwt(b);
	b = f3 ? f3->id : -1; fwt(b);
	fwt(square);
	b = s0 ? s0->id : -1; fwt(b);
	b = s1 ? s1->id : -1; fwt(b);
	fwtv(cmd);
}
void Link::on_load(Cur& cur) {
#define TMP(i) if (i >= 0) { f##i = &*cur.lks[i]; }
	TMP(0); TMP(1); TMP(2); TMP(3);
#undef TMP
}
void Link::check_dead() {
	// 这里对待 dead 和无效引用的态度和之前是不一样的。
	if (b0 && b0->dead) { b0 = NULL; }
	if (b1 && b1->dead) { b1 = NULL; }
	if (f0 && f0->dead) { f0 = NULL; }
	if (f1 && f1->dead) { f1 = NULL; }
	if (f2 && f2->dead) { f2 = NULL; }
	if (f3 && f3->dead) { f3 = NULL; }
	if (s0 && s0->dead) { s0 = NULL; }
	if (s1 && s1->dead) { s1 = NULL; }
}
void Link::init() {
	b0->lks.push_back(this);
	b1->lks.push_back(this);
}
void Link::die() {
	dead = true;
	if (square) {
		if (s0) { s0->dead = true; }
		if (s1) { s1->dead = true; }
	}
}
void Link::refresh(Cur& cur) {
	gl[L"link"] = msh<Var>();
	Execute(gl, Compile(cmd));
	if (gl.find(L"link") == gl.end()) { return; }
	read_cfg(cur, *gl[L"link"]);
}
void Link::read_cfg(Cur& cur, Var const& v) {
	auto& dic = v.dic;
	getv(rope); getv(solid);
	getv(spring); getv(breakable);
	getp(r);
	getp(len); getp(damp);
	getp(hooke); getp(stiff);
	if (found(L"col")) { c_normal = (dcol)tv3(*dic.at(L"col")); }
}

void Link::Render(Cur& cur) {
	if (dead) { return; }

	double h = abs(force) * cur.mult_force;
	auto max_h = 2 * 2 * PI / 3;
	h = clmp(h, 0.0, max_h);
	h = max_h - h;
	dcol c_force = (dcol)from_hsl(h, 1, 0.5);
	dcol c =
		selected ? dcol(0, 255, 255) :
		cur.show_force ? c_force :
		cur.dark_bd ? dcol(40) : c_normal;

	if (!solid) {
		draw_px_seg(scr, dscr, b0->o, b1->o, dep, bgr.vp(), c);
		return;
	}

	vec2 n = (b1->o - b0->o).unit();
	vec2 m = vec2(-n.y, n.x);
	draw_px_seg(scr, dscr, b0->o + r * m, b1->o + r * m, dep, bgr.vp(), c);
	draw_px_seg(scr, dscr, b0->o - r * m, b1->o - r * m, dep, bgr.vp(), c);
}
void Link::Update(Cur& cur) {
	// check_dead 需要在 Update 之前做成。
	if (!b0 || b0->dead || !b1 || b1->dead) { die(); }
	if (dead) { return; }
	if (cur.crt->mode == MODE_KNFE && cur.crt->active) {
		auto v0 = (vec2)msp;
		auto v1 = (vec2)msp_old;
		if (intersect(v0, v1, b0->o, b1->o)) { dead = true; }
		// 不知道这一行有没有用。
		// 我不知道有没有算法可以准确完整的切割。
		if (dist_sqr(b0->o, b1->o, (vec2)msp) < 3) { dead = true; }
	}
	selected = cur.link_sel == this;
}
void Link::Resolve(Cur& cur) {
	if (dead) { return; }

	vec2 n = b1->o - b0->o;
	double rlen = n.len();
	if (rlen < 1e-2) { return; }
	double dif = rlen - len;
	if (rope && dif <= 0) { return; }
	n /= rlen;

	double inv_m0 = b0->fixed || b0->dragged ? 0.0 : (double)b0->inv_m;
	double inv_m1 = b1->fixed || b1->dragged ? 0.0 : (double)b1->inv_m;
	double sum = inv_m0 + inv_m1;
	if (sum == 0) { return; }

	if (spring) {
		force = dif * hooke;
		b1->old_o += n * force * inv_m1 * sdt * sdt;
		b0->old_o -= n * force * inv_m0 * sdt * sdt;

		auto v01 = b1->o - b1->old_o - b0->o + b0->old_o;
		auto vn = dot(n, v01);
		b1->old_o += n * vn * damp * sdt;
		b0->old_o -= n * vn * damp * sdt;
	} else {
		b1->o -= n * inv_m1 / sum * dif;
		b0->o += n * inv_m0 / sum * dif;
		force = dif / (sdt * sdt);
	}

	// 由于 check_dead 的存在，我感觉这里面很多判断有重复的。
	// 不知道怎么处理，不过也懒得管了。
	if (!aux && !cur.no_break && breakable && abs(force) > stiff) { die(); }
	if (aux) {
		bool b0 = f0 && !f0->dead && f1 && !f1->dead;
		bool b1 = f2 && !f2->dead && f3 && !f3->dead;
		if (!b0 && !b1) { die(); }
	}
	if (square) {
		bool b0 = s0 && !*s0;
		bool b1 = s1 && !*s1;
		if (!b0 && !b1) { die(); }
	}
}

bool intersect(vec2 va0, vec2 va1, vec2 vb0, vec2 vb1) {
	vec2 va = va1 - va0;
	vec2 vb = vb1 - vb0;
	// 感觉总割不干净，不知道怎么办。
	bool b0 = cross(va, vb1 - va0) * cross(va, vb0 - va0) <= 0;
	bool b1 = cross(vb, va1 - vb0) * cross(vb, va0 - vb0) <= 0;
	return b0 && b1;
}
