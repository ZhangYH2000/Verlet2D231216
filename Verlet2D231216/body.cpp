#include "body.h"
#include "cur.h"
#include "bgr.h"
#include "body.h"
#include "draw_geo.h"
#include "draw_curve.h"
#include "draw_px_seg.h"
#include "creator.h"
#include "mass_pnt.h"
#include "link.h"

#include "my_def.h"

double constexpr dep = 0;

#define n_track (cur.n_track)
#define found(s) (dic.find(s) != dic.end())
#define getv(nm) if (found(L#nm)) { nm = dic.at(L#nm)->num; }
#define getp(nm) if (found(L#nm)) { nm = Num(cur, *dic.at(L#nm)); }

#define frdn(nm) nm = Num(cur, f);

Body::Body() {
	r = 4;
	inv_m = 1 / (r * r * PI);
	k = 0.75;
	c_normal = dcol(255);
}
Body::Body(Cur& cur, FILE* f) {
	int sz = 0;
	frd(o); frd(old_o);
	frdn(r); frdn(inv_m);
	frdn(k); frdn(damp);
	frdn(friction);
	frdn(viscosity);
	frd(c_normal); 
	frdv(cmd); tmp_cmd = cmd;
	frd(fixed); frd(vanish);
	refresh(cur);
}
void Body::save(FILE* f) {
	int sz = 0;
	fwt(o); fwt(old_o); 
	r.save(f); inv_m.save(f);
	k.save(f); damp.save(f);
	friction.save(f);
	viscosity.save(f);
	fwt(c_normal); fwtv(cmd);
	fwt(fixed); fwt(vanish);
}
void Body::check_dead() {
	// 这行真的会节省效率吗。
	if (lks.empty()) { return; }
	lks.erase(remove_if(lks.begin(), lks.end(),
		[](Link* lk) { return lk->dead; }), lks.end());
}
void Body::refresh(Cur& cur) {
	gl[L"body"] = msh<Var>();
	Execute(gl, Compile(cmd));
	if (gl.find(L"body") == gl.end()) { return; }
	read_cfg(cur, *gl[L"body"]);

	// 如果要指定轨迹的话，需要放在这里，而不是下面的 read_cfg 里面。
	// 要单独放是因为它们无法只能通过 cmd 获得，无法保存。
}
void Body::read_cfg(Cur& cur, Var const &v) {
	auto& dic = v.dic;
	getp(r); getp(inv_m); getp(k); getp(damp);
	getp(friction); getp(viscosity);
	if (found(L"col")) { c_normal = (dcol)tv3(*dic.at(L"col")); }
}

void Body::Step(Cur& cur) {
	if (dead) { return; }
	if (dragged) {
		// 不知道这里拖拽使用其它方式会有什么不同。
		vec2 new_o = o + v_goal * sdt;
		old_o = o; o = new_o; return;
	}
	if (fixed) { old_o = o; return; }

	vec2 g = vec2(cur.gx, cur.gy);
	for (auto& mp : cur.mass_pnts) {
		g += mp->acc(o);
	}

	auto new_o = 
		2 * o - old_o 
		+ sdt * sdt * g
		- sdt * (o - old_o) * damp;
	old_o = o; o = new_o;

	if (cur.wind) {
		if (frnd(1) < cur.p_wind) {
			old_o -= vec2(cur.x_wind, cur.y_wind);
		}
	}
}
void Body::Render(Cur& cur) const {
	if (dead) { return; }
	double e = (o - old_o).lensqr() / (sdt * sdt);
	double h = e * cur.mult_energy;
	double max_h = 2 * PI * 2 / 3;
	h = max_h - clmp(h, 0.0, max_h);
	dcol c_energy = (dcol)from_hsl(h, 1, 0.5);
	
	dcol c =
		dragged ? dcol(255, 0, 0) :
		selected ? dcol(255, 0, 255) :
		lk_selected ? dcol(0, 255, 255) :
		hovered ? dcol(0, 0, 255) :
		fixed ? dcol(255, 255, 0) : 
		cur.show_kinetic ? c_energy : 
		cur.dark_bd && cur.render_mode == RNDR_BDRY ? dcol(40) : c_normal;
	dcol c_bd =
		cur.render_mode == RNDR_BDRY ? c : 
		cur.dark_bd ? dcol(40) : dcol(255);
	int n = r <= 10 ? 8 : 100;
	if (cur.render_mode == RNDR_FULL) {
		draw_eclipse(scr, dscr, dep, o, r, r, bgr.vp(), c);
	}
	if (cur.render_mode <= RNDR_BDRY) {
		draw_px_circle(scr, dscr, dep, (dvec)o, r, bgr.vp(), c_bd);
	} else {
		dvec p = (dvec)o;
		if (insd(p, bgr.vp())) {
			int dp = p.y * scr.w + p.x;
			scr.cols[dp] = c; dscr[dp] = dep;
		}
	}
	
	//draw_eclipse_frame(scr, dscr, dep, o, r, r, bgr.vp(), c_bd, n);

	if (!track.empty()) {
		rep(i, 0, track.size() - 1) {
			int idx = track.size() - 2 - i;
			double lght = 0.8 * (n_track - idx) / n_track;
			double h = track[i].t * cur.mult_hue_track;
			col3 c = from_hsl(h, 1, lght);
			draw_px_seg(scr, dscr, 
				track[i].o, track[i + 1].o, dep + 1, bgr.vp(), (dcol)c);
		}
	}

	if (!cur.show_velocity) { return; }
	vec2 ov = o + cur.mult_velocity * (o - old_o) / sdt;
	draw_px_seg(scr, dscr, o, ov, dep, bgr.vp(), dcol(0, 255, 0));
}
void Body::register_grid(Cur& cur) {
	if (dead) { return; }
	if (sgd < 2 * r) { cur.giants.push_back(this); return; }

	int x = (o.x - bgr.tl.x) / sgd;
	int y = (o.y - bgr.tl.y) / sgd;
	// 不启用这个主要是因为 Poly 和 Giant 的碰撞还没有适配这个。
	//if (cur.inv_x) { x = cur.nx_grid - 1 - x; }
	//if (cur.inv_y) { y = cur.nx_grid - 1 - y; }
	x = clmp(x, 0, nxgd - 1);
	y = clmp(y, 0, nygd - 1);

	int id = y * nxgd + x;
	gd[id].push_back(this);
}

void Body::Update(Cur& cur) {
	if (dead) { return; }
	if (vanish) {
		// 不知道这些有没有必要。
		bool ok = false;
		for (auto& lk : lks) if (!lk->dead) { ok = true; }
		if (!ok) { dead = true; }
	}

	hovered = (hvd == this);
	selected = cur.body_sel == this;
	lk_selected = cur.link_sel &&
		(cur.link_sel->b0 == this || cur.link_sel->b1 == this);
	if (dragged) {
		dragged = msd[0];
		vec2 o_goal = (vec2)msp - o_rel_ms;
		if (cur.paused) {
			v_goal = {}; old_o = o = o_goal;
		} else {
			v_goal = (o_goal - o) / rdt;
		}
		
		if (!kb) {
			if (kbc(L'F')) { fixed = !fixed; }
			if (kbc(VK_DELETE)) { dead = true; }
		}
	} else {
		if (cur.ranged_drag) {
			dragged =
				bgr.wheeled && 
				(o - (vec2)msp).lensqr() < cur.r_drag * cur.r_drag 
				&& msc(0) && cur.crt->mode == MODE_DRAG;
		} else {
			dragged =
				hovered && msc(0)
				&& cur.crt->mode == MODE_DRAG;
		}

		if (dragged) {
			v_goal = {};
			o_rel_ms = (vec2)msp - o;
		}
	}

	if (cur.crt->mode == MODE_DELE && cur.crt->active) { 
		bool b0 = hovered && !cur.ranged_drag;
		bool b1 =
			cur.ranged_drag && bgr.wheeled &&
			(o - (vec2)msp).lensqr() < cur.r_drag * cur.r_drag;
		if (b0 || b1) { dead = true; }
	}
	if (cur.crt->mode == MODE_SLCT && hovered && msc(0)) {
		cur.set_bsel(this);
	}
	if (selected && cur.show_track) { 
		track.push_back({ (dvec)o, cur.t });
		if (track.size() > n_track) { track.pop_front(); }
	} else { track.clear(); }
	
	if (hovered && cur.crt->mode == MODE_CONN) {
		if (msc(0) && !cur.crt->active) {
			cur.crt->b0 = this;
		} else if (!msd[0] && msd_old[0] && cur.crt->active) {
			cur.crt->b1 = this;
		}
	}
}
void Body::Discard(Cur& cur) {
	rmv; hovered = dragged = false;
}
void Body::PreUpdate(Cur& cur) {
	if (dead) { return; }
	bool ok = dhv < dep&&
		insd(msp, bgr.vp()) &&
		((vec2)msp - o).lensqr() < r* r;
	if (ok) { dhv = dep; hvd = this; }
}
