#include "draw_tri.h"

void sub_draw_tri(tile& dest, dbuf& ds,
	dvec pa, dvec pb, dvec pc, double d, dcol col, bool same_y) {
	// 假设 pa, pb, pc  已经交换过了。
	// 用它画斜的长方形有的时候会出现裂痕，不知道能不能修复。
	if (pa.y == pb.y) { return; }
	int sy = pb.y > pa.y ? 1 : -1;
	int sx = pc.x > pb.x ? 1 : -1;
	for (int y = pa.y; y != pb.y + sy; y += sy) {
		int xb = pa.x * (pb.y - y) + pb.x * (y - pa.y); xb /= (pb.y - pa.y);
		int xc = pa.x * (pb.y - y) + pc.x * (y - pa.y); xc /= (pb.y - pa.y);
		for (int x = xb; x != xc + sx; x += sx) {
			int dp = same_y ?
				y * dest.w + x : x * dest.w + y;
			if (ds[dp] <= d) { ds[dp] = d; dest.cols[dp] = col; }
		}
	}
}
void draw_tri(tile& dest, dbuf& ds, drect vp,
	dvec pa, dvec pb, dvec pc, double d, dcol col) {
	// 如果可以让这里写得好看有条理，容易修改且不容易出 bug 就好了。

#define TMP(a, c, x, y, sgn, bd)\
if (p##c.y sgn bd) { return; }\
if (p##a.y sgn bd) {\
	dvec p0 = p##c; p0.y = bd;\
	p0.x += (p##a.x - p##c.x) * (bd - p##c.y) / (p##a.y - p##c.y);\
	if (pb.y sgn bd) {\
		dvec p1 = p##c; p1.y = bd;\
		p1.x += (pb.x - p##c.x) * (bd - p##c.y) / (p##b.y - p##c.y);\
		draw_tri(dest, ds, vp, p0, p1, p##c, d, col); return;\
	}\
	else {\
		dvec p1 = pb; p1.y = bd;\
		p1.x += (p##a.x - pb.x) * (bd - pb.y) / (p##a.y - pb.y);\
		draw_tri(dest, ds, vp, p0, pb, p##c, d, col);\
		draw_tri(dest, ds, vp, p0, pb, p1, d, col); return;\
	}\
}

	// 这里有点低效，重复比较了，但是总的影响不大。
	long long x0 = vp.left();
	long long x1 = vp.right() - 1;
	long long y0 = vp.top();
	long long y1 = vp.bottom() - 1;
	// 不要直接把表达式放在 TMP 里面。

	if (pa.x < pb.x) { swap(pa, pb); }
	if (pa.x < pc.x) { swap(pa, pc); }
	if (pb.x < pc.x) { swap(pb, pc); }
	if (pa.x == pc.x) { return; }
	int dx = pa.x - pc.x;
	TMP(a, c, y, x, > , x1);
	TMP(c, a, y, x, < , x0);

	if (pa.y < pb.y) { swap(pa, pb); }
	if (pa.y < pc.y) { swap(pa, pc); }
	if (pb.y < pc.y) { swap(pb, pc); }
	if (pa.y == pc.y) { return; }
	int dy = pa.y - pc.y;
	TMP(a, c, x, y, > , y1);
	TMP(c, a, x, y, < , y0);

#undef TMP
	// 这里是出于数值稳定性的考虑交换一下方向，否则如果三角形很扁的话会奇怪。
	bool y_big = dy > dx;
	if (!y_big) {
		if (pa.x < pb.x) { swap(pa, pb); }
		if (pa.x < pc.x) { swap(pa, pc); }
		if (pb.x < pc.x) { swap(pb, pc); }
		swap(pa.x, pa.y); swap(pb.x, pb.y); swap(pc.x, pc.y);
	}
	dvec pd = pb;
	pd.x = pa.x * (pb.y - pc.y) + pc.x * (pa.y - pb.y); pd.x /= pa.y - pc.y;
	sub_draw_tri(dest, ds, pa, pb, pd, d, col, y_big);
	sub_draw_tri(dest, ds, pc, pb, pd, d, col, y_big);
}
void draw_tri(tile& dest, dbuf& ds, drect vp,
	vec2 pa, vec2 pb, vec2 pc, double d, dcol col) {
	draw_tri(dest, ds, vp, (dvec)pa, (dvec)pb, (dvec)pc, d, col);
}

void sub_draw_tri(tile& dest, dbuf& ds,
	dvec pa, dvec pb, dvec pc, double d,
	vec2 ua, vec2 ub, vec2 uc, tile& t, bool same_y) {
	// 假设 pa, pb, pc  已经交换过了。
	// 用它画斜的长方形有的时候会出现裂痕，不知道能不能修复。
	if (pa.y == pb.y) { return; }
	int sy = pb.y > pa.y ? 1 : -1;
	int sx = pc.x > pb.x ? 1 : -1;
	for (int y = pa.y; y != pb.y + sy; y += sy) {
		int xb = pa.x * (pb.y - y) + pb.x * (y - pa.y); xb /= (pb.y - pa.y);
		int xc = pa.x * (pb.y - y) + pc.x * (y - pa.y); xc /= (pb.y - pa.y);
		vec2 u0 = ua * (pb.y - y) + ub * (y - pa.y); u0 /= (pb.y - pa.y);
		vec2 u1 = ua * (pb.y - y) + uc * (y - pa.y); u1 /= (pb.y - pa.y);
		// 这样的话应该不需要检查坐标是否超出范围。
		// 但可能还是要的，我不太清楚。
		for (int x = xb; x != xc + sx; x += sx) {
			// 下面比较怪，有一些原因这么做，不知道有没有好的修改方式。
			vec2 u = u0;
			if (xb != xc) {
				u = u0 * (xc - x) + u1 * (x - xb); u /= (xc - xb);
			}
			int sx = u.x * t.w; sx = clmp(sx, 0, t.w - 1);
			int sy = u.y * t.h; sy = clmp(sy, 0, t.h - 1);
			int sp = sy * t.w + sx;
			int dp = same_y ?
				y * dest.w + x : x * dest.w + y;
			if (ds[dp] <= d) { ds[dp] = d; dest.cols[dp] = t.cols[sp]; }
		}
	}
}
void draw_tri(tile& dest, dbuf& ds, drect vp,
	dvec pa, dvec pb, dvec pc, double d,
	vec2 ua, vec2 ub, vec2 uc, tile& t) {

#define TMP(a, c, x, y, sgn, bd)\
if (p##c.y sgn bd) { return; }\
if (p##a.y sgn bd) {\
	dvec p0 = p##c; p0.y = bd;\
	p0.x += (p##a.x - p##c.x) * (bd - p##c.y) / (p##a.y - p##c.y);\
	vec2 v0 = (u##a * (bd - p##c.y) + u##c * (p##a.y - bd)) / (p##a.y - p##c.y);\
	if (pb.y sgn bd) {\
		dvec p1 = p##c; p1.y = bd;\
		p1.x += (pb.x - p##c.x) * (bd - p##c.y) / (pb.y - p##c.y);\
		vec2 v1 = (ub * (bd - p##c.y) + u##c * (pb.y - bd)) / (pb.y - p##c.y);\
		draw_tri(dest, ds, vp, p0, p1, p##c, d, v0, v1, u##c, t); return;\
	}\
	else {\
		dvec p1 = pb; p1.y = bd;\
		p1.x += (p##a.x - pb.x) * (bd - pb.y) / (p##a.y - pb.y);\
		vec2 v1 = (u##a * (bd - pb.y) + ub * (p##a.y - bd)) / (p##a.y - pb.y);\
		draw_tri(dest, ds, vp, p0, pb, p##c, d, v0, ub, u##c, t);\
		draw_tri(dest, ds, vp, p0, pb, p1, d, v0, ub, v1, t); return;\
	}\
}

	// 这里有点低效，重复比较了，但是总的影响不大。
	long long x0 = vp.left();
	long long x1 = vp.right() - 1;
	long long y0 = vp.top();
	long long y1 = vp.bottom() - 1;
	// 不要直接把表达式放在 TMP 里面。

	if (pa.x < pb.x) { swap(pa, pb); swap(ua, ub); }
	if (pa.x < pc.x) { swap(pa, pc); swap(ua, uc); }
	if (pb.x < pc.x) { swap(pb, pc); swap(ub, uc); }
	if (pa.x == pc.x) { return; }
	int dx = pa.x - pc.x;
	TMP(a, c, y, x, > , x1);
	TMP(c, a, y, x, < , x0);

	if (pa.y < pb.y) { swap(pa, pb); swap(ua, ub); }
	if (pa.y < pc.y) { swap(pa, pc); swap(ua, uc); }
	if (pb.y < pc.y) { swap(pb, pc); swap(ub, uc); }
	if (pa.y == pc.y) { return; }
	int dy = pa.y - pc.y;
	TMP(a, c, x, y, > , y1);
	TMP(c, a, x, y, < , y0);

#undef TMP
	// 这里是出于数值稳定性的考虑交换一下方向，否则如果三角形很扁的话会奇怪。
	bool y_big = dy > dx;
	if (!y_big) {
		if (pa.x < pb.x) { swap(pa, pb); swap(ua, ub); }
		if (pa.x < pc.x) { swap(pa, pc); swap(ua, uc); }
		if (pb.x < pc.x) { swap(pb, pc); swap(ub, uc); }
		swap(pa.x, pa.y); swap(pb.x, pb.y); swap(pc.x, pc.y);
	}
	dvec pd = pb;
	pd.x = pa.x * (pb.y - pc.y) + pc.x * (pa.y - pb.y); pd.x /= pa.y - pc.y;
	vec2 ud = ua * (pb.y - pc.y) + uc * (pa.y - pb.y); ud /= pa.y - pc.y;
	sub_draw_tri(dest, ds, pa, pb, pd, d, ua, ub, ud, t, y_big);
	sub_draw_tri(dest, ds, pc, pb, pd, d, uc, ub, ud, t, y_big);
}
