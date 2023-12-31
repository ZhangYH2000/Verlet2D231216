#pragma once
#include "button.h"
#include "ctrl_list.h"
#include "float_panel.h"
#include "rich_textbox.h"
#include "rich_label.h"
#include "separator.h"

struct RTbCon : RichTextbox {
	RTbCon();
	void Sync(App& app) override;
	void Upload(App& app) const override;
};
struct BtConOK : BtLan {
	BtConOK();
	void OnClick(App& app) override;
};
struct BtConDel : BtLan {
	BtConDel();
	void OnClick(App& app) override;
};

struct ClyCon : CtrlListY {
	ptr<RTbCon> rtb;
	ptr<BtConOK> bt_ok;
	ptr<BtConDel> bt_del;
	ptr<CtrlListX> clx_bt;
	ptr<SeparatorY> spy;
	ClyCon();
};
struct DgCon : Delegate {
	ptr<ClyCon> cly;
	DgCon();
	void Update(App& app) override;
};

struct RLbCon : RichLabel {
	void Sync(App& app) override;
};

struct FPnLink : FPnLan {
	ptr<DgCon> dg_con;
	ptr<RLbCon> rlb_con;
	ptr<CtrlListY> cly;
	FPnLink(App& app);
};
