#pragma once
#include "panel.h"
#include "label.h"
#include "ctrl_list.h"

struct LbFPS : Label {
	LbFPS();
	void Sync(App& app) override;
};
struct LbNBody : Label {
	LbNBody();
	void Sync(App& app) override;
};
struct LbMode : Label {
	LbMode();
	void Sync(App& app) override;
};
struct LbMouse : Label {
	LbMouse();
	void Sync(App& app) override;
};

struct PnBottom : Panel {
	ptr<LbFPS> lb_fps;
	ptr<LbNBody> lb_n_body;
	ptr<LbMode> lb_mode;
	ptr<LbMouse> lb_mouse;
	ptr<CtrlListX> clx;
	PnBottom(App& app);
};
