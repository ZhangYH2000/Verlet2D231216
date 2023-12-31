#pragma once
#include "button.h"
#include "textbox.h"
#include "ctrl_list.h"
#include "rich_label.h"
#include "float_panel.h"


struct OtShape : Control {
	dvec gap;
	double dep = 0;
	int w = 0, h = 0;
	double sx = 0, sy = 0;
	dcol c_frame, c_normal, c_hovered, c_filled, c_select;

	dvec p0_rel;
	vector<int> shape;
	bool edit = false;
	bool hovered = false;

	OtShape();
	int GetW() const { return w + gap.x; };
	int GetH() const { return h + gap.y; };

	void render(App& app);
	void Update(App& app) override;
	void Discard(App& app) override;
	void PreUpdate(App& app) override;
};

struct BtLdShape : BtLan {
	BtLdShape();
	void OnClick(App& app) override;
};
struct BtSvShape : BtLan {
	BtSvShape();
	void OnClick(App& app) override;
};
struct BtDelShape : BtLan {
	BtDelShape();
	void OnClick(App& app) override;
};
struct RLbShapes : RichLabel {
	RLbShapes();
	void Sync(App& app) override;
};

struct FPnShape : FPnLan {
	ptr<OtShape> ot_shape;
	ptr<Textbox> tb_nm_shape;
	ptr<BtLdShape> bt_ld_shape;
	ptr<BtSvShape> bt_sv_shape;
	ptr<BtDelShape> bt_del_shape;
	ptr<CtrlListX> clx_nm_shape;
	ptr<RLbShapes> rlb_shapes;
	ptr<CtrlListY> cly;
	FPnShape(App& app);
};
