#pragma once
#include "BPT.hpp"
#include "predef.hpp"
namespace pretest {

typedef std::pair<pii, int> phi;
template <int degree = getDegree<phi>()>
void print(BPT<phi, degree> &b, pii x) {
    int l = b.lower_bound(phi(x, INT_MINIMUN));
    if (l == b.END || b.Gtmp.a[l].first != x) {
        cout << "null" << endl;
        return;
    }
    for (int i = l; i <= b.Gtmp.size; i++) {
        if (i >= b.Gtmp.size) {
            if (b.Gtmp.next) {
                i = 0;
                b.data.read(b.Gtmp.next, b.Gtmp);
            } else {
                break;
            }
        }
        if (b.Gtmp.a[i].first == x) {
            cout << b.Gtmp.a[i].second << " ";
        } else {
            break;
        }
    }
    cout << endl;
}
constexpr ll M1 = 1e9 + 7;
constexpr ll M2 = 1e9 + 9;
constexpr ll MB = 521;
pii hash(const char *s);
void main();
} // namespace pretest