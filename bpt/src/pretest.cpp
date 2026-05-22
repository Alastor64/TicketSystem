#include "pretest.hpp"
pii pretest::hash(const char *s) {
    ll h1 = 0, h2 = 0;
    for (int i = 0; s[i]; i++) {
        h1 *= MB;
        h1 += s[i];
        h2 *= MB;
        h2 += s[i];
        h1 %= M1;
        h2 %= M2;
    }
    return pii(h1, h2);
}
void pretest::main() {
    int n;
    cin >> n;
    BPT<phi> b("zz");
    char s1[200], s2[200];
    int x;
    while (n--) {
        cin >> s1 >> s2;
        if (s1[0] == 'i') {
            cin >> x;
            b.insert(phi(hash(s2), x));
        }
        if (s1[0] == 'd') {
            cin >> x;
            b.del(phi(hash(s2), x));
        }
        if (s1[0] == 'f') {
            print(b, hash(s2));
        }
    }
}