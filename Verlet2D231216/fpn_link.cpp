#include "fpn_link.h"
#include "cur.h"
#include "link.h"

#define cur (*(Cur*)&app)
#include "my_def.h"
#define pcsel (cur.link_sel)
#define csel (*pcsel)

RTbCon::RTbCon() : RichTextbox(360, 400) {}
void RTbCon::Sync(App& app) { str = csel.tmp_cmd; }
void RTbCon::Upload(App& app) const { csel.tmp_cmd = str; }

BtConOK::BtConOK() : BtLan(70, L"bt_con_ok") {}
void BtConOK::OnClick(App& app) {
	csel.cmd = csel.tmp_cmd; csel.refresh(cur);
}
BtConDel::BtConDel() : BtLan(70, L"bt_con_del") {}
void BtConDel::OnClick(App& app) { csel.die(); }

ClyCon::ClyCon() {
	vector<Control*> tmp;
	mkp(rtb)();
	mkp(bt_ok)(); mkp(bt_del)();
	tmp = { &*bt_ok, &*bt_del };
	mkcl(clx_bt);
	mkp(spy)(400);
	cs = { &*rtb, &*clx_bt, &*spy };
}
DgCon::DgCon() {
	mkp(cly)();
}
void DgCon::Update(App& app) {
	if (pcsel) { set_c(app, &*cly); }
	else { set_c(app, NULL); }
	Delegate::Update(app);
}

void RLbCon::Sync(App& app) {
	if (!pcsel) { txt = loc(L"link_not_sel"); return; }
	txt.clear();

	txt += loc(L"link_rope") + tw(csel.rope) + L"\n";
	txt += loc(L"link_solid") + tw(csel.solid) + L"\n";
	txt += loc(L"link_spring") + tw(csel.spring) + L"\n";
	txt += loc(L"link_breakable") + tw(csel.breakable) + L"\n";
	txt += loc(L"link_len") + tw2(csel.len) + L"\n";
	txt += loc(L"link_damp") + tw2(csel.damp) + L"\n";
	txt += loc(L"link_hooke") + tw2(csel.hooke) + L"\n";
	txt += loc(L"link_stiff") + tw2(csel.stiff) + L"\n";
	txt += loc(L"link_r") + tw2(csel.r) + L"\n";
	txt += loc(L"link_aux") + tw(csel.aux) + L"\n";
	txt += loc(L"link_square") + tw2(csel.square) + L"\n";
}

FPnLink::FPnLink(App& app) : FPnLan(app, 400, 600, L"fpn_link") {
	vector<Control*> tmp;
	mkp(dg_con)(); mkp(rlb_con)();
	tmp = { &*dg_con, &*rlb_con };
	mkcl(cly); c = &*cly; Init(app);
}
