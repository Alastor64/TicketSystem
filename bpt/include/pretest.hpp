#pragma once
#include "BPT.hpp"
#include "predef.hpp"
namespace pretest {

typedef std::pair<pii, int> phi;
template <int degree> void print(BPT<phi, degree> &b, pii x) {
    b.Gpos = b.lower_bound(phi(x, INT_MINIMUN));
    if (b.GposInvalid() || b.Gvalue().first != x) {
        cout << "null" << endl;
        return;
    }
    while (!b.GposInvalid() && b.Gvalue().first == x) {
        cout << b.Gvalue().second << " ";
        b.plusGpos();
    }
    cout << endl;
}
constexpr ll M1 = 1e9 + 7;
constexpr ll M2 = 1e9 + 9;
constexpr ll MB = 521;
pii hash(const char *s);
void main();
} // namespace pretest