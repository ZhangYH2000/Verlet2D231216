#pragma once
#include "tile.h"

struct Cur;
struct Body;
struct Poly {
	vec2 o;
	double ang = 0;
	vector<vec2> vs_rel, vs;
	double x0 = 0, x1 = 0;
	double y0 = 0, y1 = 0;

	dcol c_normal;
	double v_goal_ang = 0;
	vec2 o_rel_ms, v_goal;
	bool dead = false;
	bool hovered = false;
	bool dragged = false;

	Poly(vector<vec2> const& _vs);
	Poly(Cur& cur, FILE* f);
	void save(FILE *f);
	void Init(bool preset_o = true);
	void Step(Cur& cur);
	void Render(Cur& cur);
	void generate();
	bool inside(vec2 p) const;
	void update_box();

	void Update(Cur& cur);
	void Discard(Cur& cur);
	void PreUpdate(Cur& cur);
};

bool inside_tri(vec2 v, vec2 v0, vec2 v1, vec2 v2);
