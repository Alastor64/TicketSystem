#include "BPT.hpp"
#include "predef.hpp"
#include "pretest.hpp"
int main() {
    BPT<pretest::phi, 3> b("ff");
    pii k = pii(1, 4);
    // b.insert(pretest::phi(k, 0));
    pretest::print(b, k);
    for (int i = 0; i < 50; i++) {
        b.insert(pretest::phi(k, i));
    }
    // for (int i = 0; i < 50; i++) {
    //     b.del(pretest::phi(k, i));
    // }
    // for (int i = 0; i < 50; i++) {
    //     b.del(pretest::phi(k, i));
    // }
    for (int i = 0; i < 50; i++) {
        b.del(pretest::phi(k, i));
        b.data.read(b.rootIndex, b.Gtmp);
        cout << b.Gtmp.next << " " << b.Gtmp.size << ":";
        pretest::print(b, k);
    }
    // for (int i = 0; i < 50; i++) {
    //     b.insert(pretest::phi(k, i));
    // }
    return 0;
}