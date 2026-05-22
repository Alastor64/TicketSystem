#include "BPT.hpp"
#include "Filer.hpp"
#include "pretest.hpp"
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <time.h>
using namespace std;
int main() {
    srand(time(NULL));
    BPT<phi, 3> b("zf");
    pii k = pii(0, 0);
    for (int i = 0; i < 50; i++) {
        int x = i;
        cout << x << " ";
        b.insert(phi(k, x));
    }
    // b.insert(phi(k, 4));
    b.del(phi(k, 4));
    cout << endl;
    cout << endl;
    cout << endl;
    print(b, k);
    return 0;
}