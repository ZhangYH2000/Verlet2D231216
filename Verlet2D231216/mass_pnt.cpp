#include "mass_pnt.h"
#include "cur.h"
#include "var.h"
#include "bgr.h"
#include "draw_geo.h"
#include "draw_curve.h"
#include "draw_px_seg.h"

#include "my_def.h"

double constexpr dep = 0;

mass_pnt::mass_pnt(Var const& v) {
	auto& vec = v.vec;
	if (vec.size() < 2) { return; }
	m = vec[0]->num;
	o = tv2(*vec[1]);
	if (vec.size() < 3) { return; }
	no_square = vec[2]->num;
}
mass_pnt::mass_pnt(Cur& cur, FILE* f) {
	frd(o); frd(m); frd(no_square);
}
void mass_pnt::save(FILE* f) {
	fwt(o); fwt(m); fwt(no_square);
}
void mass_pnt::Render(Cur& cur) const {
	draw_eclipse_frame(scr, dscr, dep, o, 8, 8, bgr.vp(), dcol(255), 10);
	double hs = 12;
	draw_px_seg(scr, dscr, 
		o - vec2(0, hs), o + vec2(0, hs), dep, bgr.vp(), dcol(255));
	draw_px_seg(scr, dscr,
		o - vec2(hs, 0), o + vec2(hs, 0), dep, bgr.vp(), dcol(255));
}
vec2 mass_pnt::acc(vec2 p) {
	auto n = o - p;
	auto lsqr = n.lensqr();
	if (lsqr < 20 * 20) { return {}; }
	if (no_square) { return m * n / lsqr; }

	auto len = sqrt(lsqr);
	return m * n / (lsqr * len);
}
