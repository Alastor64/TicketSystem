#pragma once
#include <filesystem>
#include <iostream>
#include <string>
typedef std::pair<int, int> pii;
typedef long long ll;
constexpr int INT_MINIMUN = -2147483648;
using std::cin;
using std::cout;
// using std::endl;
constexpr char endl = '\n';
constexpr int DAY_MINUTES = 24 * 60;
using std::getline;
using std::istream;
using std::max;
using std::min;
using std::ostream;
using std::pair;
using std::stoi;
using std::string;
using std::filesystem::directory_iterator;
template <typename T> class ComparableBase {
  public:
    friend bool operator>(const T &a, const T &b) { return b < a; }
    friend bool operator>=(const T &a, const T &b) { return !(a < b); }
    friend bool operator<=(const T &a, const T &b) { return !(b < a); }
    friend bool operator==(const T &a, const T &b) {
        return !(b < a) && !(a < b);
    }
    friend bool operator!=(const T &a, const T &b) { return b < a || a < b; }
};