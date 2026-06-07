#include <iostream>
#include <string>
using namespace std;
int main() {
    string s;
    getline(cin, s);
    s = s + " ";
    int i = s.find_first_not_of(' ', 0);
    while (i < s.length()) {
        int j = s.find_first_of(' ', i);
        cout << s.substr(i, j - i) << "|||";
        i = s.find_first_not_of(' ', j);
    }
    cout << "\n";
    return 0;
}