#include "Filer.hpp"
#include <iostream>
using namespace std;
int main() {
    cout << "hello\n";
    Filer<int> a("zz");
    int k;
    a.read(8, k);
    cout << k << "\n";
    a.push(2);
    int i = a.push(3);
    a.push(4);
    a.pop(i);
    a.push(6);
    a.push(7);
    return 0;
}