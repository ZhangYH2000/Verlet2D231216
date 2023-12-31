#include "fpn_global.h"
#include "ui.h"
#include "cur.h"


#define cur (*(Cur*)&app)
#include "my_def.h"

BtSave::BtSave() : BtLan(80, L"bt_save") {}
void BtSave::OnClick(App& app) { cur.Save(ui.fpn_global->tb_save->str); }
BtLoad::BtLoad() : BtLan(80, L"bt_load") {}
void BtLoad::OnClick(App& app) { cur.Load(ui.fpn_global->tb_load->str); }

void RLbGlobal::Sync(App& app) {
	txt.clear();
	txt += loc(L"t") + tw2(cur.t) + L"\n";
	txt += loc(L"vol") + tw2(cur.vol) + L"\n";
	txt += loc(L"rdt") + tw2(rdt) + L"\n";
	txt += loc(L"n_step") + tw(cur.n_step) + L"\n";
	txt += loc(L"gx") + tw2(cur.gx) + L"\n";
	txt += loc(L"gy") + tw2(cur.gy) + L"\n";
	txt += loc(L"skip_friction") + tw(cur.skip_friction) + L"\n";
	txt += loc(L"s_grid") + tw2(cur.s_grid) + L"\n";
	txt += loc(L"no_bd_coll") + tw(cur.circle_bd) + L"\n";
	txt += loc(L"ranged_drag") + tw(cur.ranged_drag) + L"\n";
	txt += loc(L"r_drag") + tw2(cur.r_drag) + L"\n";
	txt += loc(L"shuffle_links") + tw(cur.shuffle_links) + L"\n";
	txt += loc(L"no_grid_coll") + tw(cur.no_grid_coll) + L"\n";
	txt += loc(L"no_break") + tw(cur.no_break) + L"\n";
	txt += loc(L"mult_energy") + tw(cur.mult_energy) + L"\n";
	txt += loc(L"mult_velocity") + tw(cur.mult_velocity) + L"\n";
	txt += loc(L"mult_force") + tw(cur.mult_force) + L"\n";
	txt += loc(L"wind") + tw(cur.wind) + L"\n";
	txt += loc(L"p_wind") + tw2(cur.p_wind) + L"\n";
	txt += loc(L"x_wind") + tw2(cur.x_wind) + L"\n";
	txt += loc(L"y_wind") + tw2(cur.y_wind) + L"\n";
}

FPnGlobal::FPnGlobal(App& app) : FPnLan(app, 400, 600, L"fpn_global") {
	vector<Control*> tmp;
	mkp(tb_save)(270); mkp(bt_save)();
	tmp = { &*tb_save, &*bt_save }; mkcl(clx_save);
	mkp(tb_load)(270); mkp(bt_load)();
	tmp = { &*tb_load, &*bt_load }; mkcl(clx_load);
	mkp(spy)(400); mkp(rlb_global)();
	tmp = { &*clx_save, &*clx_load, &*spy, &*rlb_global };
	mkcl(cly); c = &*cly; Init(app);
}
