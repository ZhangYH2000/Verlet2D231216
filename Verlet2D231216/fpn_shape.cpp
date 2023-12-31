#include "fpn_shape.h"
#include "ui.h"
#include "cur.h"
#include "draw_geo.h"
#include "draw_comp.h"
#include "draw_px_seg.h"

#define cur (*(Cur*)&app)
#include "my_def.h"

OtShape::OtShape() {
	w = 960; h = 640;
	dep = 1000;
	gap = { 10, 10 };

	c_frame = dcol(100);
	c_normal = { 20, 20, 20 };
	c_hovered = { 10, 10, 255 };
	c_filled = { 255, 255, 255 };
	c_select = { 255, 10, 10 };

	sx = (double)w / nx_shape;
	sy = (double)h / ny_shape;
	shape.resize(nx_shape * ny_shape);
}

void OtShape::render(App& app) {
	int x0 = tl.x;
	int y0 = tl.y;
	int x1 = x0 + w;
	int y1 = y0 + h;

	draw_rect_raw(scr, tl, w, h, vp, c_normal);

	if (edit) {
		int gx0 = p0_rel.x / sx;
		int gy0 = p0_rel.y / sy;
		int gx1 = (msp.x - x0) / sx + 1;
		int gy1 = (msp.y - y0) / sy + 1;
		gx0 = clmp(gx0, 0, nx_shape - 1);
		gy0 = clmp(gy0, 0, ny_shape - 1);
		gx1 = clmp(gx1, gx0 + 1, nx_shape);
		gy1 = clmp(gy1, gy0 + 1, ny_shape);

		draw_rect_raw(scr, tl + dvec(gx0 * sx, gy0 * sy), 
			(gx1 - gx0) * sx, (gy1 - gy0) * sy, vp, c_select);
	} else if (hovered) {
		int gx = (msp.x - x0) / sx;
		int gy = (msp.y - y0) / sy;
		gx = clmp(gx, 0, nx_shape - 1);
		gy = clmp(gy, 0, ny_shape - 1);
		draw_rect_raw(scr, 
			tl + dvec(gx * sx, gy * sy), sx, sy, vp, c_hovered);
	}

	rep(i, 0, nx_shape + 1) {
		int x = x0 + sx * i;
		draw_px_seg(scr, dscr, vec2(x, y0), vec2(x, y1), dep, vp, c_frame);
	}

	rep(j, 0, ny_shape + 1) {
		int y = y0 + sy * j;
		draw_px_seg(scr, dscr, vec2(x0, y), vec2(x1, y), dep, vp, c_frame);
	}

	rep(i, 0, nx_shape) rep(j, 0, ny_shape) {
		int id = j * nx_shape + i;
		if (!shape[id]) { continue; }

		auto tl_gd = tl + dvec(i * sx, j * sy) + dvec(sx, sy) / 2;
		int s = 3;
		tl_gd -= dvec(s / 2, s / 2);
		draw_rect_raw(scr, tl_gd, s, s, vp, c_filled);
	}
}
void OtShape::Update(App& app) {
	hovered = (this == hvd);
	if (edit) {
		if (!msd[0]) {
			edit = false;
			int gx0 = p0_rel.x / sx;
			int gy0 = p0_rel.y / sy;
			int gx1 = (msp.x - tl.x) / sx + 1;
			int gy1 = (msp.y - tl.y) / sy + 1;
			gx0 = clmp(gx0, 0, nx_shape - 1);
			gy0 = clmp(gy0, 0, ny_shape - 1);
			gx1 = clmp(gx1, gx0 + 1, nx_shape);
			gy1 = clmp(gy1, gy0 + 1, ny_shape);

			int id = gy0 * nx_shape + gx0;
			int val = (int)!shape[id];
			rep(i, gx0, gx1) rep(j, gy0, gy1) {
				id = j * nx_shape + i;
				shape[id] = val;
			}
		}
	} else {
		if (msc(0) && hovered) {
			edit = true;
			p0_rel = msp - tl;
		}
	}

	render(app);
}
void OtShape::Discard(App& app) {
	rmv; edit = hovered = false;
}
void OtShape::PreUpdate(App& app) {
	bool ok = dhv <= dep &&
		insd(msp, { tl, w, h }) && insd(msp, vp);
	if (ok) { dhv = dep; hvd = this; }
}

#define nm (ui.fpn_shape->tb_nm_shape->str)
#define shp (ui.fpn_shape->ot_shape->shape)

BtLdShape::BtLdShape() : BtLan(80, L"bt_ld_shape") {}
void BtLdShape::OnClick(App& app) {
	if (cur.shapes.find(nm) != cur.shapes.end()) { shp = *cur.shapes[nm]; }
}
BtSvShape::BtSvShape() : BtLan(80, L"bt_sv_shape") {}
void BtSvShape::OnClick(App& app) {
	cur.shapes[nm] = msh<vector<int>>(shp);
}
BtDelShape::BtDelShape() : BtLan(80, L"bt_del_shape") {}
void BtDelShape::OnClick(App& app) {

}
RLbShapes::RLbShapes() : RichLabel(960) {}
void RLbShapes::Sync(App& app) {
	txt = loc(L"shapes_saved");
	for (auto& sh : cur.shapes) { txt += sh.first + L","; }
}

FPnShape::FPnShape(App& app) : FPnLan(app, 1000, 770, L"fpn_shape") {
	vector<Control*> tmp;
	mkp(ot_shape)();
	mkp(tb_nm_shape)(300); mkp(bt_ld_shape)();
	mkp(bt_sv_shape)(); mkp(bt_del_shape)();
	tmp = { &*tb_nm_shape, &*bt_ld_shape, &*bt_sv_shape, &*bt_del_shape };
	mkcl(clx_nm_shape); mkp(rlb_shapes)();
	tmp = { &*ot_shape, &*clx_nm_shape, &*rlb_shapes };
	mkcl(cly); c = &*cly; Init(app);
}
