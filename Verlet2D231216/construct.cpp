#include "construct.h"
#include "cur.h"
#include "bgr.h"
#include "sqr.h"
#include "body.h"
#include "link.h"
#include "poly.h"

#include "my_def.h"

#define found(s) (dic.find(s) != dic.end())
#define getv(nm) if (found(L#nm)) { nm = dic.at(L#nm)->num; }

void MakeRope(Cur& cur, Var const& cfg_ball,
	Var const& cfg_link, vec2 o, vec2 v, int n) {

	auto b0 = msh<Body>();
	b0->read_cfg(cur, cfg_ball);
	b0->old_o = b0->o = o;
	cur.bs.push_back(b0);

	auto lk = msh<Link>();
	lk->read_cfg(cur, cfg_link);
	auto len = lk->len;

	rep(i, 0, n) {
		auto b1 = msh<Body>();
		b1->read_cfg(cur, cfg_ball);
		b1->old_o = b1->o = o + (i + 1) * v * len;
		cur.bs.push_back(b1);

		auto lk = msh<Link>();
		lk->read_cfg(cur, cfg_link);
		lk->b0 = &*b0;
		lk->b1 = &*b1;
		cur.lks.push_back(lk);  lk->init();

		b0 = b1;
	}
}

void MakeSBox(Cur& cur, Var const& cfg_body,
	Var const& cfg_link, vec2 tl, double w, double h, double density) {

	auto b0 = msh<Body>();
	auto b1 = msh<Body>();
	auto b2 = msh<Body>();
	auto b3 = msh<Body>();
	b0->read_cfg(cur, cfg_body);
	b1->read_cfg(cur, cfg_body);
	b2->read_cfg(cur, cfg_body);
	b3->read_cfg(cur, cfg_body);
	b0->o = b0->old_o = tl;
	b1->o = b1->old_o = tl + vec2(w, 0);
	b2->o = b2->old_o = tl + vec2(w, h);
	b3->o = b3->old_o = tl + vec2(0, h);
	b0->inv_m = b1->inv_m = b2->inv_m = b3->inv_m = 4.0 / density / (w * h);
	cur.bs.push_back(b0);
	cur.bs.push_back(b1);
	cur.bs.push_back(b2);
	cur.bs.push_back(b3);

	auto lk0 = msh<Link>();
	auto lk1 = msh<Link>();
	auto lk2 = msh<Link>();
	auto lk3 = msh<Link>();
	lk0->read_cfg(cur, cfg_link);
	lk1->read_cfg(cur, cfg_link);
	lk2->read_cfg(cur, cfg_link);
	lk3->read_cfg(cur, cfg_link);
	lk3->b1 = lk0->b0 = &*b0;
	lk0->b1 = lk1->b0 = &*b1;
	lk1->b1 = lk2->b0 = &*b2;
	lk2->b1 = lk3->b0 = &*b3;
	lk0->len = lk2->len = w;
	lk1->len = lk3->len = h;
	lk0->solid = lk1->solid = lk2->solid = lk3->solid = true;

	auto lk4 = msh<Link>();
	lk4->read_cfg(cur, cfg_link);
	lk4->b0 = &*b0;
	lk4->b1 = &*b2;
	lk4->len = sqrt(w * w + h * h);
	lk4->solid = false;

	// 好像不加上这根斜杆的话，有摩擦的时候就会出现盒子朝着一个方向挪动。
	auto lk5  = msh<Link>();
	lk5->read_cfg(cur, cfg_link);
	lk5->b0 = &*b1;
	lk5->b1 = &*b3;
	lk5->len = sqrt(w * w + h * h);
	lk5->solid = false;

	cur.lks.push_back(lk0);
	cur.lks.push_back(lk1);
	cur.lks.push_back(lk2);
	cur.lks.push_back(lk3);
	cur.lks.push_back(lk4);
	cur.lks.push_back(lk5);
}

void MakeFShp(Cur& cur, Var const& cfg_body,
	Var const& cfg_link, wstring const &nm, vec2 ct, int lv) {
	vector<ptr<Body>> bs;
	bs.resize(nx_shape * ny_shape);

	auto lk = msh<Link>();
	lk->read_cfg(cur, cfg_link);
	double len = lk->len;
	vec2 tl = ct - vec2(nx_shape, ny_shape) * len / 2;

	auto it = cur.shapes.find(nm);
	if (it == cur.shapes.end()) { return; }
	auto& shape = *it->second;
	
	rep(i, 0, nx_shape) rep(j, 0, ny_shape) {
		int id = j * nx_shape + i;
		if (!shape[id]) { continue; }
		auto b = msh<Body>();
		b->read_cfg(cur, cfg_body);
		b->old_o = b->o = tl + vec2(i, j) * len;
		b->vanish = lv >= BRK_VNSH;
		bs[id] = b;
		cur.bs.push_back(b);
	}

	vector<ptr<Link>> lk0, lk1, lk2, lk3;
	lk0.resize(nx_shape * ny_shape);
	lk1.resize(nx_shape * ny_shape);
	lk2.resize(nx_shape* ny_shape);
	lk3.resize(nx_shape* ny_shape);

	rep(i, 0, nx_shape - 1) rep(j, 0, ny_shape) {
		int id0 = j * nx_shape + i;
		int id1 = id0 + 1;
		if (!bs[id0] || !bs[id1]) { continue; }
		auto lk = msh<Link>();
		lk->read_cfg(cur, cfg_link);
		lk->b0 = &*bs[id0];
		lk->b1 = &*bs[id1];
		lk->square = lv >= BRK_SQR;
		lk0[id0] = lk;
		cur.lks.push_back(lk);  lk->init();
	}

	rep(i, 0, nx_shape) rep(j, 0, ny_shape - 1) {
		int id0 = j * nx_shape + i;
		int id1 = id0 + nx_shape;
		if (!bs[id0] || !bs[id1]) { continue; }
		auto lk = msh<Link>();
		lk->read_cfg(cur, cfg_link);
		lk->b0 = &*bs[id0];
		lk->b1 = &*bs[id1];
		lk->square = lv >= BRK_SQR;
		lk1[id0] = lk;
		cur.lks.push_back(lk);  lk->init();
	} 
	if (lv < BRK_DIAG) { return; }

	rep(i, 0, nx_shape - 1) rep(j, 0, ny_shape - 1) {
		int id0 = j * nx_shape + i;
		int id1 = id0 + 1;
		int id2 = id1 + nx_shape;
		int id3 = id0 + nx_shape;
		if (bs[id0] && bs[id2] ) {
			auto lk = msh<Link>();
			lk->read_cfg(cur, cfg_link);
			lk->len = lk->len * sqrt(2);
			lk->b0 = &*bs[id0];
			lk->b1 = &*bs[id2];
			lk2[id0] = lk;
			cur.lks.push_back(lk);  lk->init();

			if (lv >= BRK_AUX) {
				lk->aux = true;
				if (lk0[id0] && lk1[id1]) {
					lk->f0 = &*lk0[id0];
					lk->f1 = &*lk1[id1];
				}
				if (lk0[id3] && lk1[id0]) {
					lk->f2 = &*lk0[id3];
					lk->f3 = &*lk1[id0];
				}
			}
		}
		if (bs[id1] && bs[id3]) {
			auto lk = msh<Link>();
			lk->read_cfg(cur, cfg_link);
			lk->len = lk->len * sqrt(2);
			lk->b0 = &*bs[id1];
			lk->b1 = &*bs[id3];
			lk2[id0] = lk;
			cur.lks.push_back(lk);  lk->init();

			if (lv >= BRK_AUX) {
				lk->aux = true;
				if (lk1[id0] && lk0[id0]) {
					lk->f0 = &*lk1[id0];
					lk->f1 = &*lk0[id0];
				}
				if (lk1[id1] && lk0[id3]) {
					lk->f2 = &*lk1[id1];
					lk->f3 = &*lk0[id3];
				}
			}
		}
	}
	if (lv < BRK_SQR) { return; }

	rep(i, 0, nx_shape - 1) rep(j, 0, ny_shape - 1) {
		int id0 = j * nx_shape + i;
		int id1 = id0 + 1;
		int id2 = id1 + nx_shape;
		int id3 = id0 + nx_shape;
		
		if (bs[id0] && bs[id1] && bs[id2] && bs[id3]) {
			auto s = msh<Sqr>();
			lk0[id0]->s1 = &*s;
			lk1[id0]->s1 = &*s;
			lk0[id3]->s0 = &*s;
			lk1[id1]->s0 = &*s;
			cur.sqrs.push_back(s);
		}
	}
}

void MakeFBox(Cur& cur, Var const& cfg_body, 
	Var const& cfg_link, vec2 tl, 
	int nx, int ny, int lv, bool fix_top, bool fix_left) {

	vector<ptr<Body>> bs;
	bs.resize(nx * ny);

	auto lk = msh<Link>();
	lk->read_cfg(cur, cfg_link);
	double len = lk->len;

	rep(i, 0, nx) rep(j, 0, ny) {
		int id = j * nx + i;
		auto b = msh<Body>();
		b->read_cfg(cur, cfg_body);
		b->old_o = b->o = tl + vec2(i, j) * len;
		b->vanish = lv >= BRK_VNSH;
		bs[id] = b;
		cur.bs.push_back(b);
	}

	if (fix_top) {
		rep(i, 0, nx) { bs[i]->fixed = true; }
	}
	if (fix_left) {
		rep(j, 0, ny) { bs[j * nx]->fixed = true; }
	}

	vector<ptr<Link>> lk0, lk1, lk2, lk3;
	lk0.resize(nx * ny);
	lk1.resize(nx * ny);
	lk2.resize(nx * ny);
	lk3.resize(nx * ny);

	rep(i, 0, nx - 1) rep(j, 0, ny) {
		int id0 = j * nx + i;
		int id1 = id0 + 1;
		auto lk = msh<Link>();
		lk->read_cfg(cur, cfg_link);
		lk->b0 = &*bs[id0];
		lk->b1 = &*bs[id1];
		lk->square = lv >= BRK_SQR;
		lk0[id0] = lk;
		cur.lks.push_back(lk);  lk->init();
	}

	rep(i, 0, nx) rep(j, 0, ny - 1) {
		int id0 = j * nx + i;
		int id1 = id0 + nx;
		auto lk = msh<Link>();
		lk->read_cfg(cur, cfg_link);
		lk->b0 = &*bs[id0];
		lk->b1 = &*bs[id1];
		lk->square = lv >= BRK_SQR;
		lk1[id0] = lk;
		cur.lks.push_back(lk);  lk->init();
	}
	if (lv < BRK_DIAG) { return; }

	rep(i, 0, nx - 1) rep(j, 0, ny - 1) {
		int id0 = j * nx + i;
		int id1 = id0 + 1;
		int id2 = id1 + nx;
		int id3 = id0 + nx;
		
		{
			auto lk = msh<Link>();
			lk->read_cfg(cur, cfg_link);
			lk->len = lk->len * sqrt(2);
			lk->b0 = &*bs[id0];
			lk->b1 = &*bs[id2];
			lk2[id0] = lk;
			cur.lks.push_back(lk);  lk->init();

			if (lv >= BRK_AUX) {
				lk->aux = true;
				lk->f0 = &*lk0[id0];
				lk->f1 = &*lk1[id1];
				lk->f2 = &*lk0[id3];
				lk->f3 = &*lk1[id0];
			}
		}
		{
			auto lk = msh<Link>();
			lk->read_cfg(cur, cfg_link);
			lk->len = lk->len * sqrt(2);
			lk->b0 = &*bs[id1];
			lk->b1 = &*bs[id3];
			lk2[id0] = lk;
			cur.lks.push_back(lk);  lk->init();

			if (lv >= BRK_AUX) {
				lk->aux = true;
				lk->f0 = &*lk1[id0];
				lk->f1 = &*lk0[id0];
				lk->f2 = &*lk1[id1];
				lk->f3 = &*lk0[id3];
			}
		}
	}
	if (lv < BRK_SQR) { return; }

	rep(i, 0, nx - 1) rep(j, 0, ny - 1) {
		int id0 = j * nx + i;
		int id1 = id0 + 1;
		int id2 = id1 + nx;
		int id3 = id0 + nx;

		auto s = msh<Sqr>();
		lk0[id0]->s1 = &*s;
		lk1[id0]->s1 = &*s;
		lk0[id3]->s0 = &*s;
		lk1[id1]->s0 = &*s;
		cur.sqrs.push_back(s);
	}
}

void MakePoly(Cur& cur, Var const& cfg_sh, Var const& cfg) {
	bool preset_o = false;
	auto& dic = cfg.dic;
	getv(preset_o);
	
	vector<vec2> vs;
	for (auto& v : cfg_sh.vec) { vs.push_back(tv2(*v)); }
	auto poly = msh<Poly>(vs);
	poly->Init(preset_o);

	if (found(L"col")) { poly->c_normal = (dcol)tv3(*dic.at(L"col")); }
	cur.polys.push_back(poly);
}
void MakeLink(Cur& cur, Var const& cfg_link) {
	int idx0 = drnd(cur.bs.size());
	int idx1 = drnd(cur.bs.size());
	auto dic = cfg_link.dic;
	getv(idx0); getv(idx1);
	idx0 = clmp(idx0, 0, (int)cur.bs.size());
	idx1 = clmp(idx1, 0, (int)cur.bs.size());

	auto lk = msh<Link>();
	lk->b0 = &*cur.bs[idx0];
	lk->b1 = &*cur.bs[idx1];
	lk->read_cfg(cur, cfg_link);
	lk->init();
	cur.lks.push_back(lk);
}

void MakePendulum(Cur& cur, vec2 tl, int n, Var const& cfg_body, Var const& cfg_link) {
	auto lk = msh<Link>();
	lk->read_cfg(cur, cfg_link);
	double len = lk->len;

	auto b = msh<Body>();
	b->read_cfg(cur, cfg_body);
	double r = b->r;

	vec2 o = tl;
	rep(i, 0, n) {
		auto b0 = msh<Body>();
		b0->old_o = b0->o = o;
		b0->r = 6;
		b0->fixed = true;

		auto b1 = msh<Body>();
		b1->old_o = b1->o = o + vec2(0, len);
		b1->read_cfg(cur, cfg_body);

		auto lk = msh<Link>();
		lk->b0 = &*b0;
		lk->b1 = &*b1;
		lk->read_cfg(cur, cfg_link);
		lk->init();

		cur.bs.push_back(b0);
		cur.bs.push_back(b1);
		cur.lks.push_back(lk);
		o += vec2(2 * r, 0);
	}
}
