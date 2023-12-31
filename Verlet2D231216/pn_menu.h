#pragma once
#include "panel.h"
#include "button.h"
#include "ctrl_list.h"

struct BtPause : Button {
	BtPause();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtReset : BtLan {
	BtReset();
	void OnClick(App& app) override;
};
struct BtMute : Button {
	BtMute();
	void Update(App& app) override;
	void OnClick(App& app) override;
};

struct PnMenu : Panel {
	ptr<BtPause> bt_pause;
	ptr<BtReset> bt_reset;
	ptr<BtMute> bt_mute;
	ptr<BtFPn> bt_global;
	ptr<BtFPn> bt_shape;
	ptr<BtFPn> bt_body;
	ptr<BtFPn> bt_link;
	ptr<BtFPn> bt_cmd;
	ptr<BtFPn> bt_par;
	ptr<BtFPn> bt_lang;
	ptr<BtFPn> bt_about;
	ptr<CtrlListY> cly;
	PnMenu(App& app);
};
