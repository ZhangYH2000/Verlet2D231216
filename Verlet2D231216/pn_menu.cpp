#include "pn_menu.h"
#include "ui.h"
#include "cur.h"
#include "fpn_global.h"
#include "fpn_cmd.h"
#include "fpn_par.h"
#include "fpn_body.h"
#include "fpn_link.h"
#include "fpn_lang.h"
#include "fpn_about.h"
#include "fpn_shape.h"

#define cur (*(Cur*)&app)
#include "my_def.h"
#define mkbt(nm) mkp(bt_##nm)(&*ui.fpn_##nm);

BtPause::BtPause() : Button(150) {}
void BtPause::Update(App& app) {
	txt = cur.paused ? loc(L"start") : loc(L"pause");
	Button::Update(app);
}
void BtPause::OnClick(App& app) { cur.paused = !cur.paused; }

BtReset::BtReset() : BtLan(150, L"bt_reset") {}
void BtReset::OnClick(App& app) { cur.Reset(); }

BtMute::BtMute() : Button(150) {}
void BtMute::Update(App& app) {
	txt = cur.mute ? loc(L"unmute") : loc(L"mute");
	Button::Update(app);
}
void BtMute::OnClick(App& app) { cur.mute = !cur.mute; }

PnMenu::PnMenu(App& app) : Panel(Panel::menu(app)) {
	vector<Control*> tmp;
	mkp(bt_pause)(); mkp(bt_reset)(); mkp(bt_mute)();
	mkbt(global);
	mkbt(shape); mkbt(body); mkbt(link);
	mkbt(cmd); mkbt(par);
	mkbt(lang); mkbt(about);
	tmp = { &*bt_pause, &*bt_reset, &*bt_mute , 
		&*bt_global, &*bt_shape, &*bt_body, &*bt_link,
		&*bt_cmd, &*bt_par, &*bt_lang, &*bt_about };
	mkcl(cly); c = &*cly; Init(app);
}
