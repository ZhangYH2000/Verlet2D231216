#include "clip.h"

clip::clip(wstring const& nm) {
	FILE* f = wfopen(nm, L"rb");
	int sz = 0;
	frd(sp); frd(replay); frdv(vs);
	fclose(f);
}
clip::clip(int sp) : sp(sp) {}
void clip::play(vector<double>& in, double vol) {
	for (auto& v : in) {
		if (csp >= n()) { return; }
		v += vs[csp] * vol;
		if (++csp == n() && replay) { csp = 0; }
	}
}

void clip::save(wstring const& nm) const {
	FILE* f = wfopen(nm, L"wb");
	if (!f) { return; } int sz = 0;
	fwt(sp); fwt(replay);
	fwtv(vs);
	fclose(f);
}
