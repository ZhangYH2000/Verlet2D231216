#pragma once
#include "utils.h"

struct Cur;
struct Sqr {
	int id = 0;
	bool dead = false;
	operator bool() { return dead; }

	Sqr() = default;
};
