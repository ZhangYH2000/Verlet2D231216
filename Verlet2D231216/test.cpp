#include "test.h"
#include "cur.h"
#include "bgr.h"
#include "draw_curve.h"
#include "draw_px_seg.h"

#include "my_def.h"
#undef rdt
#undef sdt

double constexpr rdt = 1.0 / 60;
int constexpr n_step = 8;
double constexpr sdt = rdt / n_step;
vec2 const g = vec2(0, 500);
double constexpr r = 20;
double constexpr dep = 0;

void TestBall::register_grid(Cur& cur, TestScene& ts) {

}
void TestBall::CollBd(Cur& cur, TestScene& ts) {
	double x0 = bgr.tl.x + r;
	double x1 = cur.w - r;
	double y0 = r;
	double y1 = bgr.h - r;

	if (o.x < x0) {
		o.x = x0;
		v.x = max(0.0, v.x);
	} else if (o.x > x1) {
		o.x = x1;
		v.x = min(0.0, v.x);
	}

	if (o.y < y0) {
		o.y = y0;
		v.y = max(0.0, v.y);
	} else if (o.y > y1) {
		o.y = y1;
		v.y = min(0.0, v.y);
	}
}
void TestBall::Step(Cur& cur, TestScene& ts) {
	o += v * sdt;
	v += g * sdt;
	//auto new_o = 2 * o - old_o + sdt * sdt * g;
	//old_o = o; o = new_o;
}
void TestBall::Render(Cur& cur, TestScene& ts) {
	int n = 20;
	dcol c = dcol(255);
	draw_eclipse_frame(scr, dscr, dep, o, r, r, bgr.vp(), c, n);
	
	vec2 rv = vec2(-v.y, v.x);
	vec2 ov = o + 300e-1 * rv;
	draw_px_seg(scr, dscr, o, ov, dep, bgr.vp(), dcol(0, 255, 0));
}

void Coll(TestBall& b0, TestBall& b1) {
	vec2 n = b1.o - b0.o;
	double len = n.len();
	if (len < 1e-3) { return; }
	n /= len;

	double dif = 2 * r - len;
	if (dif <= 0) { return; }
	b1.o += dif * n * 0.5;
	b0.o -= dif * n * 0.5;
	if (dot(n, b1.v - b0.v) >= 0) { return; }
	vec2 v = (b0.v + b1.v) / 2;
	b1.v = v;
	b0.v = v;
}

void TestScene::Update(Cur& cur) {
	if (msc(2) && bgr.hovered) {
		auto b = msh<TestBall>();
		b->o = vec2(msp);
		b->old_o = b->o - vec2(450, 100) * sdt;
		b->v = vec2(450, 100);
		bs.push_back(b);
		t_ball = 0.3;
	}

	rep(i, 0, n_step) {
		for (auto& b : bs) { b->Step(cur, *this); }
		for (auto& b : bs) { b->CollBd(cur, *this); }

		rep(i, 0, bs.size()) rep(j, i + 1, bs.size()) {
			Coll(*bs[i], *bs[j]);
		}
	}


	for (auto& b : bs) { 
		if (b == bs.back()) { 
			b->v_rec = bs.front()->v;
			//b->v_rec = {};
			//b->v_rec.y = bs.front()->o.y - (bgr.h - r);
			//b->v_rec /= sdt;
		}
		b->Render(cur, *this); 
	}
}
