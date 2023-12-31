#pragma once
#include "utils.h"

struct Cur;
struct ParRef {
	wstring nm;
	double val = 0;
	double a = 0, b = 1;
	ParRef() = default;
	ParRef(Cur& cur, FILE* f);
	void save(FILE *f);
};


struct Var;
struct Num {
	double val = 0;
	ParRef* p = NULL;

	Num() = default;
	Num(Cur& cur, FILE* f);
	Num(Cur& cur, Var const& v);
	Num(double v) : val(v) {}
	operator double() const { return p ? p->val : val; }
	void save(FILE *f) const;
};

wstring tw(Num n);
wstring tw2(Num n);
void AddPar(Cur& cur, Var const& v);
void UpdateParRefs(Cur& cur);
