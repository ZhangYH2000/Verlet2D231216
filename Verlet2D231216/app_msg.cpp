#include "app.h"

int created(App*& app, WPARAM wp, LPARAM lp) {
	auto pcs = (CREATESTRUCT*)lp;
	app = (App*)pcs->lpCreateParams;
	return 0;
}
int destroyed(App* app, WPARAM wp, LPARAM lp) {
	PostQuitMessage(0);
	return 0;
}
int kill_focus(App* app, WPARAM wp, LPARAM lp) {
	app->in.kbd.fill(false);
	return 0;
}
int set_cursor(App* app, WPARAM wp, LPARAM lp) {
	HCURSOR cs =
		LOWORD(lp) != HTCLIENT || app->use_sys_cs ?
		app->sys_hcs : NULL;
	SetCursor(cs);
	return 0;
}
int bar_clicked(App* app, WPARAM wp, LPARAM lp) {
	app->in.bar_clicked = true;
	app->in.kbd.fill(false);
	return 0;
}
int char_translated(App* app, WPARAM wp, LPARAM lp) {
	app->in.chs += (wchar_t)wp;
	return 0;
}
int key_up(App* app, WPARAM wp, LPARAM lp) {
	app->in.kbd[wp] = false;
	return 0;
}
int key_down(App* app, WPARAM wp, LPARAM lp) {
	app->in.kbmsg[wp]++;
	app->in.kbd[wp] = true;
	return 0;
}
int mouse_move(App* app, WPARAM wp, LPARAM lp) {
	return 0;
}
int mouse_wheel(App* app, WPARAM wp, LPARAM lp) {
	int dt = GET_WHEEL_DELTA_WPARAM(wp);
	app->in.msw = (double)dt / WHEEL_DELTA;
	return 0;
}

// �кܶ����Ҫ���ǣ������걻�ε��ˣ�����ô����
// ����һ֮֡��������˺ܶ����顣
// ������С�����߱��ڵ�֮������ж�����Ƿ��ڴ����ڡ�
// �������������֮���ʱ�򲻻��� MOUSELEAVE ��Ϣ��
int mouse_leave(App* app, WPARAM wp, LPARAM lp) {
	app->in.mouse_owned = false;
	app->in.mouse_tracked = false;
	return 0;
}
int left_mouse_up(App* app, WPARAM wp, LPARAM lp) {
	app->in.msd[0] = false;
	app->in.mouse_owned = true;
	return 0;
}
int left_mouse_down(App* app, WPARAM wp, LPARAM lp) {
	app->in.msd[0] = true;
	app->in.mouse_owned = true;
	app->in.track_mouse();
	return 0;
}
int mid_mouse_up(App* app, WPARAM wp, LPARAM lp) {
	app->in.msd[1] = false;
	app->in.mouse_owned = true;
	return 0;
}
int mid_mouse_down(App* app, WPARAM wp, LPARAM lp) {
	app->in.msd[1] = true;
	app->in.mouse_owned = true;
	app->in.track_mouse();
	return 0;
}
int right_mouse_up(App* app, WPARAM wp, LPARAM lp) {
	app->in.msd[2] = false;
	app->in.mouse_owned = true;
	return 0;
}
int right_mouse_down(App* app, WPARAM wp, LPARAM lp) {
	app->in.msd[2] = true;
	app->in.mouse_owned = true;
	app->in.track_mouse();
	return 0;
}

// �Ҹ㲻���ʲô�� capture changed��
// ʵ���з����ڵ����������ʱ����������Ϣ��
LRESULT CALLBACK wnd_proc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
	static App* app = NULL;
	switch (msg) {
	case WM_CREATE: return created(app, wp, lp);
	case WM_DESTROY: return destroyed(app, wp, lp);
	case WM_KILLFOCUS: return kill_focus(app, wp, lp);
	case WM_SETCURSOR: return set_cursor(app, wp, lp);
	case WM_CAPTURECHANGED: return bar_clicked(app, wp, lp);
	case WM_CHAR: return char_translated(app, wp, lp);
	case WM_KEYUP: return key_up(app, wp, lp);
	case WM_KEYDOWN: return key_down(app, wp, lp);
	case WM_MOUSEMOVE: return mouse_move(app, wp, lp);
	case WM_MOUSEWHEEL: return mouse_wheel(app, wp, lp);
	case WM_MOUSELEAVE: return mouse_leave(app, wp, lp);
	case WM_LBUTTONUP: return left_mouse_up(app, wp, lp);
	case WM_LBUTTONDOWN: return left_mouse_down(app, wp, lp);
	case WM_MBUTTONUP: return mid_mouse_up(app, wp, lp);
	case WM_MBUTTONDOWN: return mid_mouse_down(app, wp, lp);
	case WM_RBUTTONUP: return right_mouse_up(app, wp, lp);
	case WM_RBUTTONDOWN: return right_mouse_down(app, wp, lp);
	default: return DefWindowProc(wnd, msg, wp, lp);
	}
}
