#include "creator.h"
#include "cur.h"
#include "bgr.h"
#include "var.h"
#include "body.h"
#include "poly.h"
#include "link.h"
#include "construct.h"
#include "draw_comp.h"
#include "draw_curve.h"
#include "draw_px_seg.h"

#include "my_def.h"

#define found(s) (dic.find(s) != dic.end())
#define getv(nm) if (found(L#nm)) { nm = dic.at(L#nm)->num; }
#define getp(nm) if (found(L#nm)) { nm = Num(cur, *dic.at(L#nm)); }

double constexpr dep = 1;

Creator::Creator() {
	mkp(cfg_body)();
	// 这里 len 应该怎么处理也是一个问题。
	// 具体实现中 len 的表现应该会很奇怪。
	mkp(cfg_link)();
	density = 1;
	col_poly = col3(1);
	nm_shape = L"shape";
	break_level = BRK_VNSH;
	max_t_spray = 0.01;
	v_spray = vec2(4500, 1000);
}
void Creator::update_poly(Cur& cur) {
	if (active) {
		double x0 = p0_ms.x, y0 = p0_ms.y;
		double x1 = msp.x, y1 = msp.y;
		double min_wh = 10;
		if (x1 - x0 >= min_wh && y1 - y0 >= min_wh) {
			vector<vec2> vs = {
				vec2(x0, y0), vec2(x1, y0), vec2(x1, y1), vec2(x0, y1)
			};
			mkp(poly)(vs); 
			poly->c_normal = (dcol)col_poly;
			poly->Init(false);
		}

		if (!msd[0]) {
			active = false;
			if (poly) {
				cur.polys.push_back(poly); poly = NULL;
			}
		}
	} else {
		if (msc(0) && hovered) {
			active = true; p0_ms = msp;
		}
	}
}
void Creator::update_conn(Cur& cur) {
	if (active) {
		// 要考虑中途放手的情况。
		// 要求 creator 的更新在 body 后面对吧，是不是耦合太高了。
		// 但也没啥办法是吧。
		if (!msd[0]) {
			if (b1 && b0 != b1) {
				auto lk = msh<Link>();
				lk->b0 = &*b0;
				lk->b1 = &*b1;
				lk->read_cfg(cur, *cfg_link);

				if (fixed_len && lk->len >= 10) {
					auto n = (b1->o - b0->o).unit();
					b1->o = b1->old_o = b0->o + n * lk->len;
				} else {
					lk->len = (b0->o - b1->o).len();
				}
				
				cur.lks.push_back(lk);  lk->init();
			}
			b0 = b1 = NULL; active = false;
		}
	} else {
		if (b0) { active = true; }
	}
}
void Creator::update_rope(Cur& cur) {
	if (active) {
		if (!msd[0]) {
			active = false;
			auto lk = msh<Link>();
			lk->read_cfg(cur, *cfg_link);

			if (lk->len >= 10) {
				auto p0 = (vec2)p0_ms;
				auto p1 = (vec2)msp;
				int n = (p1 - p0).len() / lk->len;
				MakeRope(cur, *cfg_body, *cfg_link, p0, (p1 - p0).unit(), n);
			}
		}
	} else {
		if (msc(0) && hovered) {
			active = true; p0_ms = msp;
		}
	}
}
void Creator::update_sbox(Cur& cur) {
	if (active) {
		if (!msd[0]) {
			active = false;
			double x0 = p0_ms.x, y0 = p0_ms.y;
			double x1 = msp.x, y1 = msp.y;
			double min_wh = 10;
			if (x1 - x0 >= min_wh && y1 - y0 >= min_wh) {
				MakeSBox(cur, *cfg_body, *cfg_link, 
					vec2(x0, y0), x1 - x0, y1 - y0, density);
			}
		}
	} else {
		if (msc(0) && hovered) {
			active = true; p0_ms = msp;
		}
	}
}
void Creator::update_fshp(Cur& cur) {
	if (msc(0) && hovered) {
		auto lk = msh<Link>();
		lk->read_cfg(cur, *cfg_link);

		if (lk->len >= 10) {
			MakeFShp(cur, *cfg_body, *cfg_link, 
				nm_shape, (vec2)msp, break_level);
		}
	}
}
void Creator::update_fbox(Cur& cur) {
	if (active) {
		if (!msd[0]) {
			active = false;
			double x0 = p0_ms.x, y0 = p0_ms.y;
			double x1 = msp.x, y1 = msp.y;
			double min_wh = 10;
			auto lk = msh<Link>();
			lk->read_cfg(cur, *cfg_link);

			if (x1 - x0 >= min_wh && y1 - y0 >= min_wh && lk->len >= 10) {
				// 请注意下面需要加 1，与 rope 的情形不同。
				int nx = (x1 - x0) / lk->len + 1;
				int ny = (y1 - y0) / lk->len + 1;
				MakeFBox(cur, *cfg_body, *cfg_link,
					(vec2)p0_ms, nx, ny, break_level, fix_top, fix_left);
			}
		}
	} else {
		if (msc(0) && hovered) {
			active = true; p0_ms = msp;
		}
	}
}
void Creator::update_knfe(Cur& cur) {
	if (active) {
		active = msd[0];
	} else {
		active = msc(0) && hovered;
	}
}
void Creator::update_dele(Cur& cur) {
	if (active) {
		active = msd[0];
	} else {
		active = msc(0) && hovered;
	}
}
void Creator::Update(Cur& cur) {
	hovered = (hvd == this);
	poly = NULL; 

	if (spray) {
		spray = msd[2];
		if (t_spray < 0) {
			auto b = msh<Body>();
			b->o = (vec2)msp;
			b->old_o = b->o - v_spray * sdt;
			b->read_cfg(cur, *cfg_body);
			cur.bs.push_back(b);
			t_spray = max_t_spray;
		} 
		if (!cur.paused) { t_spray -= rdt; }
	} else {
		// 其实应该有自己的 wheeled 的。
		spray = msc(2) && bgr.wheeled && !cur.paused;
		if (spray) { t_spray = 0; }

		if (msc(2) && bgr.wheeled && cur.paused) {
			auto b = msh<Body>();
			b->old_o = b->o = (vec2)msp;
			b->read_cfg(cur, *cfg_body);
			cur.bs.push_back(b);
		}
	}


	switch (mode) {
	case MODE_POLY: update_poly(cur); break;
	case MODE_CONN: update_conn(cur); break;
	case MODE_ROPE: update_rope(cur); break;
	case MODE_SBOX: update_sbox(cur); break;
	case MODE_FSHP: update_fshp(cur); break;
	case MODE_FBOX: update_fbox(cur); break;
	case MODE_KNFE: update_knfe(cur); break;
	case MODE_DELE: update_dele(cur); break;
	}
}
void Creator::Discard(Cur& cur) {
	b0 = b1 = NULL;
	rmv; poly = NULL; 
	hovered = active = false;
}
void Creator::PreUpdate(Cur& cur) {
	if (mode == MODE_SLCT) { return; }
	if (mode == MODE_DRAG) { return; }
	if (mode == MODE_CONN) { return; }
	if (mode == MODE_DELE && active) { return; }

	bool ok = dhv < dep&& insd(msp, bgr.vp());
	if (ok) { dhv = dep; hvd = this; }
}
void Creator::Render(Cur& cur) const {
	if (poly) { poly->Render(cur); }
	if (mode == MODE_CONN && b0) {
		draw_px_seg(scr, dscr, b0->o, (vec2)msp, dep, bgr.vp(), dcol(255));
	}
	if (mode == MODE_ROPE && active) {
		draw_px_seg(scr, dscr, (vec2)p0_ms, (vec2)msp, dep, bgr.vp(), dcol(255));
	}
	if ((mode == MODE_FBOX || mode == MODE_SBOX) && active) {
		int w = msp.x - p0_ms.x;
		int h = msp.y - p0_ms.y;
		if (w >= 0 && h >= 0) {
			draw_px_rect_frame(scr, dscr, dep, p0_ms, w, h, bgr.vp(), dcol(255));
		}
	}
	if ((mode == MODE_DRAG || mode == MODE_DELE) && cur.ranged_drag) {
		draw_eclipse_frame(scr, dscr, dep, (vec2)msp,
			cur.r_drag, cur.r_drag, bgr.vp(), dcol(255), 40);
	}
}
void Creator::refresh(Cur& cur, Var const& v) {
	auto& dic = v.dic;
	getv(fix_top); getv(fix_left);
	getv(break_level);
	getv(max_t_spray); getv(density);
	if (found(L"shape")) { nm_shape = dic.at(L"shape")->str; }
	if (found(L"cfg_body")) { mkp(cfg_body)(*dic.at(L"cfg_body")); }
	if (found(L"cfg_link")) { mkp(cfg_link)(*dic.at(L"cfg_link")); }
	if (found(L"col_poly")) { col_poly = tv3(*dic.at(L"col_poly")); }
	if (found(L"v_spray")) { v_spray = tv2(*dic.at(L"v_spray")); }
}
