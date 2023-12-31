#pragma once
#include "num.h"
#include "tile.h"

struct p_track {
	dvec o;
	double t = 0;
};

struct Cur;
struct Var;
struct Link;
struct Body {
	vec2 o, old_o;
	Num r = 0;
	Num inv_m = 0;
	Num k = 0;
	Num damp = 0;
	Num friction = 0;
	Num viscosity = 0;
	dcol c_normal;
	wstring cmd, tmp_cmd;

	// 临时成员，不知道应该怎么处理这个
	int id = 0;
	
	vec2 v_goal;
	vec2 o_rel_ms;
	bool dead = false;
	bool fixed = false;
	bool hovered = false;
	bool dragged = false;
	bool selected = false;
	bool lk_selected = false;

	bool vanish = false;
	vector<Link*> lks;

	deque<p_track> track;

	Body();
	Body(Cur& cur, FILE* f);
	void save(FILE *f);
	void check_dead();
	void refresh(Cur& cur);
	void read_cfg(Cur& cur, Var const& v);

	void Step(Cur& cur);
	void Render(Cur& cur) const;
	void register_grid(Cur& cur);

	void Update(Cur& cur);
	void Discard(Cur& cur);
	void PreUpdate(Cur& cur);
};