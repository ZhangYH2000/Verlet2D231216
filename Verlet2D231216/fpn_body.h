#pragma once
#include "label.h"
#include "button.h"
#include "ctrl_list.h"
#include "float_panel.h"
#include "rich_textbox.h"
#include "checkbox.h"
#include "separator.h"
#include "rich_label.h"

struct Body;
struct RTbBody : RichTextbox {
	RTbBody();
	void Sync(App& app) override;
	void Upload(App& app) const override;
};
struct BtBodyOK : BtLan {
	BtBodyOK();
	void OnClick(App& app) override;
};
struct BtBodyDel : BtLan {
	BtBodyDel();
	void OnClick(App& app) override;
};

// con ���� link ����˼�����ø���
struct LbBodyCon : Label {
	int id = 0;
	LbBodyCon(int id);
	void Sync(App& app) override;
};
struct CbBodyCon : CbLan {
	int id = 0;
	CbBodyCon(int id);
	void Sync(App& app) override;
	void Upload(App& app) const override;
};
struct ClXBodyCon : CtrlListX {
	ptr<LbBodyCon> lb;
	ptr<CbBodyCon> cb;
	ClXBodyCon(int id);
};
struct ClYBodyCons : CtrlListY {
	vector<ptr<ClXBodyCon>> mbs;
	void Update(App& app) override;
};

// ����һ�����������Ҫ�� bsel ���ĵ�ʱ������ UI�������֪��������á�
struct ClyBody : CtrlListY {
	ptr<ClYBodyCons> cly_cons;
	ptr<RTbBody> rtb;
	ptr<BtBodyOK> bt_ok;
	ptr<BtBodyDel> bt_del;
	ptr<CtrlListX> clx_bt;
	ptr<SeparatorY> spy;
	ClyBody();
};
struct DgBody : Delegate {
	ptr<ClyBody> cly;
	DgBody();
	void Update(App& app) override;
};

struct RLbBody : RichLabel {
	void Sync(App& app) override;
};

struct FPnBody : FPnLan {
	ptr<RLbBody> rlb_body; 
	ptr<DgBody> dg_body;
	ptr<CtrlListY> cly;
	FPnBody(App& app);
};
