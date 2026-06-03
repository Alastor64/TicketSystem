#pragma once
#include "predef.hpp"
template <int maxlen> class arraystring {
  public:
    arraystring() {
        s[0] = '\0';
        len = 0;
    }
    arraystring(char *_s) {
        len = 0;
        while (_s[len]) {
            s[len] = _s[len];
            len++;
        }
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
};