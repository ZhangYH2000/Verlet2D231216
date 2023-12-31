#pragma once
#include "tile.h"

struct Cur;
struct Var;
struct mass_pnt {
	vec2 o;
	double m = 0;
	bool no_square = false;

	mass_pnt(Var const &v);
	mass_pnt(Cur& cur, FILE* f);
	void save(FILE *f);
	void Render(Cur& cur) const;
	vec2 acc(vec2 p);
};