#pragma once
#include "num.h"
#include "tile.h"

struct Sqr;
struct Cur;
struct Var;
struct Body;
struct Link {
	// 这里用智能指针其实没问题的，但是懒得改了。
	// 以后要记得用智能指针。
	Body* b0 = NULL;
	Body* b1 = NULL;
	bool rope = false;
	bool solid = false;
	bool spring = false;
	bool breakable = false;

	Num len = 0;
	Num damp = 0;
	Num hooke = 0;
	Num stiff = 0;
	Num r = 0;
	dcol c_normal;

	bool aux = false;
	int id_f0 = 0;
	int id_f1 = 0;
	int id_f2 = 0;
	int id_f3 = 0;
	Link* f0 = NULL;
	Link* f1 = NULL;
	Link* f2 = NULL;
	Link* f3 = NULL;
	bool square = false;
	Sqr* s0 = NULL;
	Sqr* s1 = NULL;

	int id = 0;
	
	bool selected = false;
	bool dead = false;
	double force = 0;
	wstring cmd, tmp_cmd;

	Link();
	Link(Cur& cur, FILE* f);
	void save(FILE *f);
	void on_load(Cur& cur);
	void check_dead();
	void init();
	void refresh(Cur& cur);
	void die();
	void read_cfg(Cur& cur, Var const& v);

	void Render(Cur& cur);
	void Update(Cur& cur);
	void Resolve(Cur& cur);
};

bool intersect(vec2 va0, vec2 va1, vec2 vb0, vec2 vb1);