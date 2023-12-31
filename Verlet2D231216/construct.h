#pragma once
#include "tile.h"

struct Cur;
struct Var;

int constexpr BRK_NONE = 0;
int constexpr BRK_DIAG = 1;
int constexpr BRK_AUX = 2;
int constexpr BRK_SQR = 3;
int constexpr BRK_VNSH = 4;

// n 指的是线段的数量。
void MakeRope(Cur& cur, Var const& cfg_ball,
	Var const& cfg_link, vec2 o, vec2 v, int n);
// cfg_link 的 solid 属性在这里没用了。
void MakeSBox(Cur& cur, Var const& cfg_body,
	Var const& cfg_link, vec2 tl, double w, double h, double density);
void MakeFShp(Cur& cur, Var const& cfg_body,
	Var const& cfg_link, wstring const &nm, vec2 ct, int lv);
void MakeFBox(Cur& cur, Var const& cfg_body, 
	Var const& cfg_link, vec2 tl, int nx, int ny, 
	int lv, bool fix_top, bool fix_left);
void MakePoly(Cur& cur, Var const& cfg_sh, Var const& cfg);
void MakeLink(Cur& cur, Var const& cfg_link);
void MakePendulum(Cur& cur, vec2 tl, int n, Var const& cfg_body, Var const& cfg_link);
