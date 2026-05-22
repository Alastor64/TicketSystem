#include "BPT.hpp"
#include "predef.hpp"
#include "pretest.hpp"
int main() {
    BPT<pretest::phi, 3> b("ff");
    pii k = pii(1, 4);
    for (int i = 0; i < 50; i++) {
        b.insert(pretest::phi(k, i));
    }
    for (int i = 0; i < 50; i++) {
        b.del(pretest::phi(k, i));
    }
    for (int i = 0; i < 50; i++) {
        b.del(pretest::phi(k, i));
    }
    for (int i = 0; i < 50; i++) {
        b.insert(pretest::phi(k, i));
    }
    pretest::print(b, k);
    return 0;
}