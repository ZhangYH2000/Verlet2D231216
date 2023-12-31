#include "pn_bottom.h"
#include "cur.h"
#include "creator.h"

#define cur (*(Cur*)&app)
#include "my_def.h"

LbFPS::LbFPS() { w = 450; fixed_w = true; }
void LbFPS::Sync(App& app) {
	txt = loc(L"frm_tm") + tw2(cur.fps.frm_time * 1000) +
		L", FPS: " + tw2(cur.fps.fps);
}

LbNBody::LbNBody() { w = 300; fixed_w = true; }
void LbNBody::Sync(App& app) {
	txt = loc(L"lb_n_body") + tw(cur.bs.size());
}

LbMode::LbMode() { w = 300; fixed_w = true; }
void LbMode::Sync(App& app) {
	txt = loc(L"lb_mode");
	switch (cur.crt->mode) {
	case MODE_DRAG: txt += loc(L"mode_drag"); break;
	case MODE_SLCT: txt += loc(L"mode_slct"); break;
	case MODE_POLY: txt += loc(L"mode_poly"); break;
	case MODE_CONN:	txt += loc(L"mode_conn"); break;
	case MODE_ROPE:	txt += loc(L"mode_rope"); break;
	case MODE_SBOX:	txt += loc(L"mode_sbox"); break;
	case MODE_FSHP:	txt += loc(L"mode_fshp"); break;
	case MODE_FBOX:	txt += loc(L"mode_fbox"); break;
	case MODE_KNFE:	txt += loc(L"mode_knfe"); break;
	case MODE_DELE:	txt += loc(L"mode_dele"); break;
	}
}
LbMouse::LbMouse() { w = 300; fixed_w = true; }
void LbMouse::Sync(App& app) {
	txt = loc(L"lb_mouse") + tw(msp);
}

PnBottom::PnBottom(App& app) : Panel(Panel::bottom(app)) {
	vector<Control*> tmp;
	mkp(lb_fps)(); mkp(lb_n_body)(); mkp(lb_mode)(); mkp(lb_mouse)();
	tmp = { &*lb_fps, &*lb_n_body, &*lb_mode, &*lb_mouse };
	mkcl(clx); c = &*clx; Init(app);
}
