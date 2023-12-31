#pragma once
#include "num.h"
#include "tile.h"

int constexpr MODE_DRAG = 0;
int constexpr MODE_POLY = 1;
int constexpr MODE_CONN = 2;
int constexpr MODE_ROPE = 3;
int constexpr MODE_SBOX = 4;
int constexpr MODE_FSHP = 5;
int constexpr MODE_FBOX = 6;
int constexpr MODE_KNFE = 7;
int constexpr MODE_SLCT = 8;
int constexpr MODE_DELE = 9;

struct Cur;
struct Var;
struct Poly;
struct Body;
struct Creator {
	int mode = MODE_DRAG;
	double t_spray = 0;
	double max_t_spray = 0;
	vec2 v_spray;

	bool spray = false;
	bool active = false;
	bool hovered = false;
	
	bool fix_top = false;
	bool fix_left = false;
	int break_level = 0;
	wstring nm_shape;
	double density = 0;
	col3 col_poly;
	bool fixed_len = false;
	ptr<Var> cfg_body, cfg_link;

	dvec p0_ms;
	ptr<Poly> poly;
	Body* b0 = NULL;
	Body* b1 = NULL;

	Creator();
	void update_poly(Cur& cur);
	void update_conn(Cur& cur);
	void update_rope(Cur& cur);
	void update_sbox(Cur& cur);
	void update_fshp(Cur& cur);
	void update_fbox(Cur& cur);
	void update_knfe(Cur& cur);
	void update_dele(Cur& cur);
	void Update(Cur& cur);
	void Discard(Cur& cur);
	void PreUpdate(Cur& cur);
	void Render(Cur& cur) const;
	void refresh(Cur& cur, Var const& v);
};