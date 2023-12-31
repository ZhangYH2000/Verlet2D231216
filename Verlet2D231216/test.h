#pragma once
#include "vec2.h"

struct Cur;
struct TestScene;
struct TestBall {
	vec2 o, old_o;
	vec2 v, v_rec;

	void register_grid(Cur& cur, TestScene& ts);
	void CollBd(Cur& cur, TestScene& ts);
	void Step(Cur& cur, TestScene& ts);
	void Render(Cur& cur, TestScene& ts);
};

struct TestScene {
	double t_ball = 0;
	vector<ptr<TestBall>> bs;
	int nx_grid = 0;
	int ny_grid = 0;
	double s_grid = 40;
	vector<vector<TestBall*>> grid;

	void Update(Cur& cur);
};
