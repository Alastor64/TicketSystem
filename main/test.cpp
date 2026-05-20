#include "BPT.hpp"
#include "Filer.hpp"
#include <iostream>
using namespace std;
int main() {
    cout << "hello\n";
    Filer<int[10]> a("zz");
    BPT<int> b("zf");
    a.push({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    return 0;
}