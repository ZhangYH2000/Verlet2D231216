#include "coll.h"
#include "cur.h"
#include "bgr.h"
#include "body.h"
#include "poly.h"
#include "link.h"

#include "my_def.h"

// 我在这里发现过无数个 bug，搞得头晕脑涨，很可能还有隐藏的 bug。
void move(Cur& cur, Body& b, vec2 n, double dif) {
	b.o += b.k * dif * n;
	if (cur.skip_friction) { return; }
	if (b.dragged && b.fixed) { return; }

	vec2 m = vec2(-n.y, n.x);
	double vm = dot(m, b.o - b.old_o);
	if (vm < 0) { m = -m; vm = -vm; }
	

	b.old_o += b.viscosity * vm * sdt * m * b.inv_m;
	vm = min(vm, b.k * dif * b.friction);
	b.old_o += vm * m;
}
void CollBd(Cur& cur, Body& b) {
	double x0 = bgr.tl.x;
	double x1 = x0 + bgr.w;
	double y0 = 0;
	double y1 = y0 + bgr.h;
	if (cur.circle_bd) {
		double r = abs(x1 - x0) / 2 - b.r;
		auto n = vec2(x0 + x1, y0 + y1) / 2 - b.o;
		auto len = n.len();
		auto dif = len - r;
		if (dif <= 0) { return; }
		n /= len;
		move(cur, b, n, dif); return;
	}

	x0 += b.r; y0 += b.r;
	x1 -= b.r; y1 -= b.r;



	if (b.o.x < x0) { move(cur, b, vec2(1, 0), x0 - b.o.x); }
	else if (b.o.x > x1) { move(cur, b, vec2(-1, 0), b.o.x - x1); }
	if (b.o.y < y0) { move(cur, b, vec2(0, 1), y0 - b.o.y); }
	else if (b.o.y > y1) { move(cur, b, vec2(0, -1), b.o.y - y1); }
}
void CollPoly(Cur& cur, Poly& p) {
	// 应该还可以搞得更精确。
	int x0 = (p.x0 - bgr.tl.x) / sgd - 1;
	int x1 = (p.x1 - bgr.tl.x) / sgd + 2;
	int y0 = (p.y0 - bgr.tl.y) / sgd - 1;
	int y1 = (p.y1 - bgr.tl.y) / sgd + 2;
	x0 = clmp(x0, 0, nxgd - 1); x1 = min(nxgd, x1);
	y0 = clmp(y0, 0, nygd - 1); y1 = min(nygd, y1);

	rep(i, x0, x1) rep(j, y0, y1)
	for (auto& b : gd[j * nxgd + i]) { Coll(cur, p, *b); }
	for (auto& b : cur.giants) { Coll(cur, p, *b); }
}

double sub_collide(vec2 n, vec2 o, Poly& p) {
	double out = DBL_MAX;
	for (auto v : p.vs) {
		auto tmp = dot(v - o, n);
		if (tmp < out) { out = tmp; }
	} return -out;
}
void Coll(Cur& cur, Poly& p, Body& b) {
	if (b.dragged) { return; }
	vec2 n;
	double d = DBL_MAX;

	for (auto& v : p.vs) {
		vec2 tmp_n = (v - b.o).unit();
		// 不知道用 eps 是不是更好。
		if (tmp_n.zero()) { continue; }
		auto o = b.o + tmp_n * b.r;
		auto tmp_d = sub_collide(tmp_n, o, p);
		if (tmp_d < 0) { return; }
		if (tmp_d < d) { d = tmp_d; n = -tmp_n; }
	}

	rep(i, 0, p.vs.size()) {
		int j = (i + 1) % p.vs.size();
		vec2 tmp_n = p.vs[j] - p.vs[i];
		tmp_n = vec2(tmp_n.y, -tmp_n.x).unit();
		auto tmp_d = b.r - dot(b.o - p.vs[i], tmp_n);

		if (tmp_d < 0) { return; }
		if (tmp_d < d) { d = tmp_d; n = tmp_n; }
	}

	move(cur, b, n, d);
}

void Coll(Cur& cur, Body& b0, Body& b1) {
	vec2 n = b1.o - b0.o;
	double len = n.len();
	if (len < 1e-3) { return; }
	n /= len;

	double dif = b0.r + b1.r - len;
	if (dif <= 0) { return; }
	double inv_m0 = b0.fixed || b0.dragged ? 0.0 : (double)b0.inv_m;
	double inv_m1 = b1.fixed || b1.dragged ? 0.0 : (double)b1.inv_m;
	double sum = inv_m0 + inv_m1;
	// 这里是不是该用 epsilon 之类。
	if (sum == 0) { return; }
	double k = (b0.k + b1.k) / 2;
	b1.o += k * dif * inv_m1 / sum * n;
	b0.o -= k * dif * inv_m0 / sum * n;
	if (cur.skip_friction) { return; }

	double friction = (b0.friction + b1.friction) * 0.5;
	double viscosity = (b0.viscosity + b1.viscosity) * 0.5;
	vec2 m = vec2(-n.y, n.x);
	vec2 v01 = b1.o - b1.old_o - b0.o + b0.old_o;
	double vm = dot(m, v01);
	if (vm < 0) { m = -m; vm = -vm; }
	// 我不知道这个是不是正确的粘性。
	// 粘性和摩擦有啥本质区别。
	// 应该会有角动量不守恒的问题。
	b1.old_o += viscosity * sdt * vm * m * inv_m1;
	b0.old_o -= viscosity * sdt * vm * m * inv_m0;
	vm = min(vm, k * dif * friction);
	b1.old_o += vm * inv_m1 / sum * m;
	b0.old_o -= vm * inv_m0 / sum * m;
}			

void CollGrid(Cur& cur, int i, int j) {
	// 我不知道为什么堆积的小球会突然炸开。
	// 下面是教学视频里面的做法，不知道是否有帮助。
	//rep(a, max(0, i - 1), min(nxgd, i + 2))
	//rep(b, max(0, j - 1), min(nygd, j + 2))
	//for (auto pb0 : gd[j * nxgd + i])
	//for (auto pb1 : gd[b * nxgd + a])
	//if (pb0 != pb1) { Coll(cur, *pb0, *pb1); } 
	//return;
	
	auto& gp = gd[j * nxgd + i];
	rep(k, 0, gp.size()) {
		auto& b0 = *gp[k];
#define TMP(a, b) for (auto pb1 : gd[(b) * nxgd + (a)]) { Coll(cur, b0, *pb1); }

		rep(m, k + 1, gp.size()) { Coll(cur, b0, *gp[m]); }
		if (j + 1 < nygd) { TMP(i, j + 1); }
		if (i + 1 >= nxgd) { continue; }
		if (j - 1 >= 0) { TMP(i + 1, j - 1); }
		if (j + 1 < nygd) { TMP(i + 1, j + 1); }
		TMP(i + 1, j);
	}
}
void CollGiants(Cur& cur) {
	rep(i, 0, cur.giants.size()) rep(j, i + 1, cur.giants.size()) {
		Coll(cur, *cur.giants[i], *cur.giants[j]);
	}
}
void CollGiantGd(Cur& cur, Body& b) {
	// 应该还可以搞得更精确。
	int x0 = (b.o.x - b.r - bgr.tl.x) / sgd - 1;
	int x1 = (b.o.x + b.r - bgr.tl.x) / sgd + 2;
	int y0 = (b.o.y - b.r - bgr.tl.y) / sgd - 1;
	int y1 = (b.o.y + b.r - bgr.tl.y) / sgd + 2;
	x0 = clmp(x0, 0, nxgd - 1); x1 = min(nxgd, x1);
	y0 = clmp(y0, 0, nygd - 1); y1 = min(nygd, y1);

	rep(i, x0, x1) rep(j, y0, y1)
	for (auto& b0 : gd[j * nxgd + i]) { Coll(cur, b, *b0); }
}

void Coll(Cur& cur, Body& b, Link& lk) {
	if (&b == &*lk.b0) { return; }
	if (&b == &*lk.b1) { return; }

	auto& b0 = *lk.b0;
	auto& b1 = *lk.b1;
	auto m = b1.o - b0.o;
	auto len = m.len();
	if (len < 1e-2) { return; }
	m /= len;
	auto n = vec2(-m.y, m.x);

	auto ao = b.o - b0.o;
	auto perp = dot(n, ao);
	auto len0 = dot(m, ao);
	auto len1 = len - len0;
	// 不知道怎么设置，好像顶点的半径比边小的时候会发生爆炸。
	double eps = 1e-1;
	if (!insd(len0, eps, len - eps)) { return; }
	if (!insd(len1, eps, len - eps)) { return; }

	if (perp < 0) { perp = -perp; n = -n; }
	double dif = b.r + lk.r - perp;
	if (dif <= 0) { return; }

	double inv_len0 = 1.0 / len0;
	double inv_len1 = 1.0 / len1;
	double sum_len = inv_len0 + inv_len1;
	double inv_m00 = b0.fixed || b0.dragged ? 0.0 : (double)b0.inv_m;
	double inv_m01 = b1.fixed || b1.dragged ? 0.0 : (double)b1.inv_m;
	double inv_m1 = b.fixed || b.dragged ? 0.0 : (double)b.inv_m;

	// 这些东西肯定要优化。
	// 之前我把这个 double 写成 auto 了，结果类型推导成 int，造成效果很奇怪。
	// 不要再这样了。
	double inv_m0 = 0;
	if (inv_m00 > 0 && inv_m01 > 0) {
		inv_m0 = 1 / (1 / inv_m00 + 1 / inv_m01);
	}
	inv_m00 *= inv_len0 / sum_len * len1 / len;
	inv_m01 *= inv_len1 / sum_len * len0 / len;
	double sum = inv_m00 + inv_m01 + inv_m1;
	if (sum == 0) { return; }
	 
	b0.o -= b.k * n * dif * inv_m00 / sum;
	b1.o -= b.k * n * dif * inv_m01 / sum;
	b.o += b.k * n * dif * inv_m1 / sum;
	if (cur.skip_friction) { return; }

	vec2 v01 = b.o - b.old_o - b0.o + b0.old_o;
	double vm = dot(m, v01);
	if (vm < 0) { m = -m; vm = -vm; }

	b.old_o += b.viscosity * sdt * vm * m * inv_m1;
	auto tmp = b.viscosity * sdt * vm * m * inv_m0;
	double sum_m = inv_m0 + inv_m1;
	vm = min(vm, b.k * dif * b.friction * sum_m / sum);
	b.old_o += vm * inv_m1 / sum_m * m;
	tmp += vm * inv_m0 / sum_m * m;
	b0.old_o -= tmp;  b1.old_o -= tmp;
}
void LinkColl(Cur& cur, Link& lk) {
	if (lk.dead) { return; }
	if (!lk.solid) { return; }
	auto& b0 = *lk.b0;
	auto& b1 = *lk.b1;
	int x0 = (min(b0.o.x, b1.o.x) - bgr.tl.x) / sgd - 1;
	int x1 = (max(b0.o.x, b1.o.x) - bgr.tl.x) / sgd + 2;
	int y0 = (min(b0.o.y, b1.o.y) - bgr.tl.y) / sgd - 1;
	int y1 = (max(b0.o.y, b1.o.y) - bgr.tl.y) / sgd + 2;
	x0 = clmp(x0, 0, nxgd - 1); x1 = min(nxgd, x1);
	y0 = clmp(y0, 0, nygd - 1); y1 = min(nygd, y1);

	rep(i, x0, x1) rep(j, y0, y1)
	for (auto& b : gd[j * nxgd + i]) { Coll(cur, *b, lk); }

	for (auto& b : cur.giants) { Coll(cur, *b, lk); }
}
