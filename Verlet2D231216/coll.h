#pragma once
#include "vec2.h"

struct Cur;
struct Body;
struct Poly;
struct Link;
void CollBd(Cur& cur, Body& b);
void CollPoly(Cur& cur, Poly& p);
void Coll(Cur& cur, Poly& p, Body& b);
void Coll(Cur& cur, Body& b0, Body& b1);

void CollGrid(Cur& cur, int i, int j);
void CollGiants(Cur& cur);
void CollGiantGd(Cur& cur, Body& b);

void Coll(Cur& cur, Body& b, Link& lk);
void LinkColl(Cur& cur, Link& lk);
