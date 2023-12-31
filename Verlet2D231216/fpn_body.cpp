#include "fpn_body.h"
#include "cur.h"
#include "body.h"
#include "link.h"

#define cur (*(Cur*)&app)
#include "my_def.h"
#define pbsel (cur.body_sel)
#define bsel (*pbsel)
#define pcsel (cur.link_sel)
#define pcon (bsel.lks[id])


RTbBody::RTbBody() : RichTextbox(360, 400) {}
void RTbBody::Sync(App& app) { str = bsel.tmp_cmd; }
void RTbBody::Upload(App& app) const { bsel.tmp_cmd = str; }

BtBodyOK::BtBodyOK() : BtLan(70, L"bt_body_ok") {}
void BtBodyOK::OnClick(App& app) {
	bsel.cmd = bsel.tmp_cmd; bsel.refresh(cur);
}
BtBodyDel::BtBodyDel() : BtLan(70, L"bt_body_del") {}
void BtBodyDel::OnClick(App& app) { bsel.dead = true; }

LbBodyCon::LbBodyCon(int id) : id(id), Label() {
	fixed_w = true; w = 270;
}
void LbBodyCon::Sync(App& app) {
	txt = loc(L"conn") + tw(id);
}

CbBodyCon::CbBodyCon(int id) : id(id), CbLan(L"cb_body_con") {}
void CbBodyCon::Sync(App& app) { checked = (pcsel == pcon); }
void CbBodyCon::Upload(App& app) const { 
	// 主义 Upload 在点击的时候才执行一次。
	cur.set_lksel(checked ? pcon : NULL); 
}

ClXBodyCon::ClXBodyCon(int id) {
	mkp(cb)(id); mkp(lb)(id);
	cs = { &*lb, &*cb };
}
void ClYBodyCons::Update(App& app) {
	rep(i, mbs.size(), bsel.lks.size()) {
		mbs.push_back(msh<ClXBodyCon>(i));
	}
	cs.clear();
	rep(i, 0, bsel.lks.size()) {
		cs.push_back(&*mbs[i]);
	}
	rep(i, bsel.lks.size(), mbs.size()) {
		mbs[i]->Discard(app);
	}
	CtrlListY::Update(app);
}

ClyBody::ClyBody() {
	vector<Control*> tmp;
	mkp(cly_cons)();
	mkp(rtb)();
	mkp(bt_ok)(); mkp(bt_del)();
	tmp = { &*bt_ok, &*bt_del };
	mkcl(clx_bt);
	mkp(spy)(400);
	cs = { &*cly_cons, &*rtb, &*clx_bt, &*spy };
}
DgBody::DgBody() {
	mkp(cly)();
}
void DgBody::Update(App& app) {
	if (pbsel) { set_c(app, &*cly); }
	else { set_c(app, NULL); }

	Delegate::Update(app);
}

void RLbBody::Sync(App& app) {
	if (!pbsel) { txt = loc(L"body_not_sel"); return; }
	txt.clear();

	auto v = (bsel.o - bsel.old_o) / sdt;
	txt += loc(L"body_o") + tw2(bsel.o) + L"\n";
	txt += loc(L"body_v") + tw2(v) + L"\n";
	txt += loc(L"body_r") + tw2(bsel.r) + L"\n";
	txt += loc(L"body_inv_m") + tw2(bsel.inv_m) + L"\n";
	txt += loc(L"body_k") + tw2(bsel.k) + L"\n";
	txt += loc(L"body_damp") + tw2(bsel.damp) + L"\n";
	txt += loc(L"body_friction") + tw2(bsel.friction) + L"\n";
	txt += loc(L"body_viscosity") + tw2(bsel.viscosity) + L"\n";
	txt += loc(L"body_vanish") + tw(bsel.vanish) + L"\n";
}

FPnBody::FPnBody(App& app) : FPnLan(app, 400, 600, L"fpn_body") {
	vector<Control*> tmp;
	mkp(dg_body)();  mkp(rlb_body)();
	tmp = { &*dg_body, &*rlb_body };
	mkcl(cly); c = &*cly; Init(app);
}
