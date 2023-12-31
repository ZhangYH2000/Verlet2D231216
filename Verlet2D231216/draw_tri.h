#pragma once
#include "tile.h"

void sub_draw_tri(tile& dest, dbuf& ds,
	dvec pa, dvec pb, dvec pc, double d, dcol col, bool same_y);
void draw_tri(tile& dest, dbuf& ds, drect vp,
	dvec pa, dvec pb, dvec pc, double d, dcol col);
void draw_tri(tile& dest, dbuf& ds, drect vp,
	vec2 pa, vec2 pb, vec2 pc, double d, dcol col);

void sub_draw_tri(tile& dest, dbuf& ds,
	dvec pa, dvec pb, dvec pc, double d,
	vec2 ua, vec2 ub, vec2 uc, tile& t, bool same_y);
void draw_tri(tile& dest, dbuf& ds, drect vp,
	dvec pa, dvec pb, dvec pc, double d,
	vec2 ua, vec2 ub, vec2 uc, tile& t);
