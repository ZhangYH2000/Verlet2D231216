#pragma once
#include "app.h"
#include "var.h"
#include "num.h"
#include "sqr.h"

struct Sqr;
struct Body;
struct Poly;
struct Link;
struct ParRef;
struct Creator;
struct mass_pnt;

int constexpr nx_shape = 96;
int constexpr ny_shape = 64;

int constexpr RNDR_FULL = 0;
int constexpr RNDR_BDRY = 1;
int constexpr RNDR_NONE = 2;

struct UI;
struct Bgr;
struct param;
struct Cur : App {
	ptr<UI> ui;
	ptr<Bgr> bgr;

	Scope gl;
	wstring dbstr, cmd, tmp_cmd;
	vector<ptr<param>> pars;

	double vol = 1;
	bool mute = false;

	bool paused = false;
	double rdt = 0;
	double sdt = 0;
	int n_step = 0;
	Num gx, gy;
	vector<ptr<Body>> bs;
	vector<ptr<Link>> lks;
	vector<ptr<Poly>> polys;
	vector<ptr<Sqr>> sqrs;
	bool skip_friction = false;

	int nx_grid = 0;
	int ny_grid = 0;
	double s_grid = 0;
	vector<Body*> giants;
	vector<vector<Body*>> grid;
	ptr<Creator> crt;
	map<wstring, ptr<ParRef>> par_refs;

	map<wstring, ptr<vector<int>>> shapes;
	
	double t = 0;
	Body* body_sel = NULL;
	Link* link_sel = NULL;
	
	int n_track = 0;
	double mult_hue_track = 0;
	bool show_track = false;
	bool circle_bd = false;
	bool ranged_drag = false;
	double r_drag = 0;
	bool shuffle_links = false;
	bool no_grid_coll = false;
	bool no_break = false;
	
	int render_mode = 0;
	bool dark_bd = false;
	bool show_velocity = false;
	bool show_kinetic = false;
	bool show_force = false;
	Num mult_energy, mult_velocity, mult_force;

	bool wind = false;
	Num p_wind, x_wind, y_wind;
	vector<ptr<mass_pnt>> mass_pnts;

	Cur();
	void Save(wstring const& nm);
	void Load(wstring const& nm);
	void Reset();
	void Update() override;
	void Exit() override;
	void set_bsel(Body* b);
	void set_lksel(Link* lk);
	void remove_dead();

	void save_par(FILE* f) const;
	void load_par(FILE* f);
	void set_cfg(Var const& v);
	void init_def_fun();
	void basic_update();
};
