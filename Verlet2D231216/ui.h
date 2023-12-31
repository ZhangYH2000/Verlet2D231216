#pragma once
#include "utils.h"

struct Cur;
struct FloatPanel;
struct FPnGlobal;
struct FPnCmd;
struct FPnPar;
struct FPnBody;
struct FPnLink;
struct FPnLang;
struct FPnAbout;
struct FPnShape;
struct PnMenu;
struct PnBottom;
struct UI {
	vector<FloatPanel*> fpns;
	ptr<FPnGlobal> fpn_global;
	ptr<FPnCmd> fpn_cmd;
	ptr<FPnPar> fpn_par;
	ptr<FPnBody> fpn_body;
	ptr<FPnLink> fpn_link;
	ptr<FPnLang> fpn_lang;
	ptr<FPnAbout> fpn_about;
	ptr<FPnShape> fpn_shape;
	ptr<PnMenu> pn_menu;
	ptr<PnBottom> pn_bottom;

	UI(Cur& cur) {};
	void sort_fpns();

	// ������ init ��ԭ���ǿؼ����� cur->ui ������ ui, �����ʱ�� ui ��δ��ʼ����
	// ��Ҳ��Ϊʲô��������Ҫ Init����Ӧ����һ���ձ�����⡣
	// ����������Щ���Ѿ���ʱ�ˣ��������Ǳ����������Է���һ��
	void Init(Cur& cur);
	void Update(Cur& cur);
	void PreUpdate(Cur& cur);
};
