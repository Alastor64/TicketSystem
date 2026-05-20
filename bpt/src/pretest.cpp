#include "pretest.hpp"
#include <climits>
void print(BPT<phi> &b, pii x) {
    int l = b.lower_bound(phi(x, INT_MIN));
    if (l == b.END) {
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