#pragma once
#include "predef.hpp"
constexpr int cn = 3; // UTF-8
template <int maxlen> class arraystring {
  public:
    arraystring() {
        s[0] = '\0';
        len = 0;
    }
    arraystring(char const *_s) {
        len = 0;
        while (_s[len]) {
            s[len] = _s[len];
            len++;
        }
        s[len] = '\0';
    }
    arraystring(const string &_s) {
        len = _s.length();
        for (int i = 0; i < len; i++) {
            s[i] = _s[i];
        }
        s[len] = '\0';
    }
    char s[maxlen];
    int len;
    friend istream &operator>>(istream &input, arraystring &x) {
        input >> x.s;
        x.len = 0;
        while (x.s[x.len]) {
            x.len++;
        }
        return input;
    }
    friend ostream &operator<<(ostream &output, const arraystring &x) {
        output << x.s;
        return output;
    }
    bool operator==(const arraystring &x) const {
        if (x.len != len)
            return 0;
        for (int i = 0; i < x.len; i++) {
            if (x.s[i] != s[i])
                return 0;
        }
        return 1;
    }
    bool operator<(const arraystring &x) const {
        for (int i = 0; i < min(len, x.len); i++) {
            if (x.s[i] > s[i])
                return 1;
            if (s[i] > x.s[i])
                return 0;
        }
        return len < x.len;
    }
    bool operator<=(const arraystring &x) const {
        return *this == x || *this < x;
    }
    bool operator>(const arraystring &x) const { return !(*this <= x); }
    bool operator>=(const arraystring &x) const { return !(*this < x); }
    bool operator!=(const arraystring &x) const { return !(*this == x); }
};