#pragma once
#include "vec2.h"
#include "tile.h"

void draw_eclipse_frame(tile& dest, dbuf& ds, double dep,
	vec2 ct, double ax, double ay, drect vp, dcol col, int n);
void draw_px_circle(tile& dest, dbuf& ds, double dep,
	dvec ct, int r, drect vp, dcol col);
