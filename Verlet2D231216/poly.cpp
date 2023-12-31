#include "poly.h"
#include "cur.h"
#include "bgr.h"
#include "draw_tri.h"
#include "draw_px_seg.h"

#include "my_def.h"

double constexpr dep = 0;
Poly::Poly(vector<vec2> const& _vs) {
	vs = _vs;
}
Poly::Poly(Cur& cur, FILE* f) {
	int sz = 0; 
	frd(o); frd(ang);
	frd(sz);
	rep(i, 0, sz) {
		vec2 v;
		frd(v);
		vs_rel.push_back(v);
	} Init(true);
}
void Poly::save(FILE* f) {
	int sz = 0;
	fwt(o); fwt(ang);
	sz = vs_rel.size();
	fwt(sz);
	for (auto v : vs_rel) { fwt(v); }
}
void Poly::Init(bool preset_o) {
	vec2 o_preset = o; o = {};
	double area = 0;
	rep(i, 0, vs.size()) {
		int j = (i + 1) % vs.size();
		auto p = vs[i], q = vs[j];
		double tri_area = cross(p, q) / 2;

		o += tri_area * (p + q) / 3;
		area += tri_area;
	} o /= area;

	vs_rel = vs;
	for (auto& v : vs_rel) { v -= o; }
	if (preset_o) { o = o_preset; }
	generate();
}
void Poly::Step(Cur& cur) {
	if (dragged) {
		o += v_goal * sdt;
		ang += v_goal_ang * sdt;
		generate();
	}
}
void Poly::Render(Cur& cur) {
	dcol c =
		dragged ? dcol(255, 0, 0) :
		hovered ? dcol(0, 0, 255) : c_normal;
	dcol c_bd =
		cur.render_mode >= RNDR_BDRY ? c : dcol(255);
	if (cur.render_mode == RNDR_FULL) {
		rep(i, 1, vs.size() - 1) {
			draw_tri(scr, dscr, bgr.vp(), vs[0], vs[i], vs[i + 1], dep, c);
		}
	}

	rep(i, 0, vs.size()) {
		int j = (i + 1) % vs.size();
		draw_px_seg(scr, dscr, vs[i], vs[j], dep, bgr.vp(), c_bd);
	}
}
void Poly::generate() {
	auto tsf = mat2::rot(ang);
	rep(i, 0, vs.size()) { vs[i] = o + tsf * vs_rel[i]; }
	update_box();
}

bool Poly::inside(vec2 p) const {
	rep(i, 0, vs.size()) {
		int j = (i + 1) % vs.size();
		if (inside_tri(p, o, vs[i], vs[j])) { return true; }
	} return false;
}
void Poly::update_box() {
	x0 = y0 = DBL_MAX; x1 = y1 = -DBL_MAX;
	for (auto v : vs) {
		x0 = min(x0, v.x); x1 = max(x1, v.x);
		y0 = min(y0, v.y); y1 = max(y1, v.y);
	}
}

void Poly::Update(Cur& cur) {
	hovered = (hvd == this);
	v_goal_ang = 0;
	if (dragged) {
		dragged = msd[0];
		if (!kb) {
			if (kbd[L'Q']) { v_goal_ang = -0.4; }
			else if (kbd[L'E']) { v_goal_ang = 0.4; }
			if (kbc(VK_DELETE)) { dead = true; }
		}

		vec2 o_goal = (vec2)msp - o_rel_ms;
		if (cur.paused) {
			// 这需要先判断键盘输入。
			ang += v_goal_ang * rdt;
			v_goal = {}; o = o_goal; 
			generate();
		} else {
			v_goal = (o_goal - o) / rdt;
		}
	} else {
		dragged = hovered && msc(0);
		if (dragged) {
			v_goal = {};
			o_rel_ms = (vec2)msp - o;
		}
	}
}
void Poly::Discard(Cur& cur) {
	rmv; hovered = dragged = false;
}
void Poly::PreUpdate(Cur& cur) {
	bool ok = dhv <= dep &&
		inside((vec2)msp) && insd(msp, bgr.vp());
	if (ok) { dhv = dep; hvd = this; }
}

bool inside_tri(vec2 v, vec2 v0, vec2 v1, vec2 v2) {
	vec3 u(v.x, v.y, 1);
	vec3 u0(v0.x, v0.y, 1);
	vec3 u1(v1.x, v1.y, 1);
	vec3 u2(v2.x, v2.y, 1);
	vec3 t = mat3(u0, u1, u2).inv() * u;
	return
		insd(t.x, 0.0, 1.0) &&
		insd(t.y, 0.0, 1.0) &&
		insd(t.z, 0.0, 1.0);
}
