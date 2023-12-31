#include "num.h"
#include "cur.h"
#include "var.h"
#include "par.h"

ParRef::ParRef(Cur& cur, FILE* f) {
	int sz = 0;
	frdv(nm);
	frd(a); frd(b);
}
void ParRef::save(FILE* f) {
	int sz = 0;
	fwtv(nm);
	fwt(a); fwt(b);
}

Num::Num(Cur& cur, FILE* f) {
	bool b = false;
	int sz = 0;
	wstring nm;
	frd(b);
	if (b) { 
		frdv(nm);
		auto it = cur.par_refs.find(nm);
		if (it != cur.par_refs.end()) { p = &*it->second; }
	} else { frd(val); }
}
Num::Num(Cur &cur, Var const& v) {
	if (v.typ != L"str") {
		val = v.num; return;
	}

	auto par = v.str;
	auto it = cur.par_refs.find(par);
	if (it == cur.par_refs.end()) { return; }
	p = &*it->second;
}
void Num::save(FILE* f) const {
	bool b = false;
	int sz = 0;
	b = p;
	fwt(b);
	if (b) { fwtv(p->nm); }
	else { fwt(val); }
}

wstring tw(Num n) {
	double v = n;
	wstring out = tw(v);
	if (!n.p) { return out; }
	out += L" (" + n.p->nm + L")";
	return out;
}
wstring tw2(Num n) {
	double v = n;
	wstring out = tw2(v);
	if (!n.p) { return out; }
	out += L" (" + n.p->nm + L")";
	return out;
}

void AddPar(Cur& cur, Var const& v) {
	auto& vec = v.vec;
	if (vec.size() < 3) { return; }
	auto pr = msh<ParRef>();
	pr->a = vec[1]->num;
	pr->b = vec[2]->num;
	pr->nm = vec[0]->str;

	auto it = cur.par_refs.find(pr->nm);
	if (it == cur.par_refs.end()) {
		cur.par_refs[pr->nm] = pr; return;
	} *it->second = *pr;
}
void UpdateParRefs(Cur& cur) {
	for (auto pp : cur.par_refs) {
		auto nm = pp.first; auto& pr = *pp.second;
		for (auto& p : cur.pars) {
			if (p->nm == nm) {
				pr.val = pr.a + (pr.b - pr.a) * p->val;
				break;
			}
		}
	}
}
