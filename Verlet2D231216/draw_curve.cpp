#include "draw_curve.h"
#include "draw_px_seg.h"

void draw_eclipse_frame(tile& dest, dbuf& ds, double dep,
	vec2 ct, double ax, double ay, drect vp, dcol col, int n) {

	auto p0 = ct + vec2(ax, 0);
	rep(i, 0, n - 1) {
		double phi = 2 * PI * (i + 1) / n;
		auto p1 = ct + vec2(ax, ay) * vec2(cos(phi), sin(phi));
		draw_px_seg(dest, ds, p0, p1, dep, vp, col);
		p0 = p1;
	}
	auto p1 = ct + vec2(ax, 0);
	draw_px_seg(dest, ds, p0, p1, dep, vp, col);
}

#define TMP \
if (insd(p, vp)) {\
	int dp = p.y * dest.w + p.x;\
	dest.cols[dp] = col;\
	ds[dp] = dep;\
}

void draw_px_circle(tile& dest, dbuf& ds, double dep,
	dvec ct, int r, drect vp, dcol col) {

	int x = r, y = 0;
	int e = 3 - 2 * r;

	while (x >= y) {
		dvec p;
		p = ct + dvec(+x, +y); TMP;
		p = ct + dvec(+x, -y); TMP;
		p = ct + dvec(-x, +y); TMP;
		p = ct + dvec(-x, -y); TMP;
		p = ct + dvec(+y, +x); TMP;
		p = ct + dvec(+y, -x); TMP;
		p = ct + dvec(-y, +x); TMP;
		p = ct + dvec(-y, -x); TMP;

#undef TMP

		if (e > 0) { e += 2 * (5 - 2 * x + 2 * y);  --x; }
		else { e += 2 * (3 + 2 * y); }
		++y;
	}
}
