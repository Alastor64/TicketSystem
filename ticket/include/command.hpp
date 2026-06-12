#pragma once
#include "predef.hpp"
constexpr int maxargn = 100;
class Command {
  public:
    string argument[26];
    int timestamp;
    string &operator[](char c);
    void clear();
};
void getCommand(Command &, string &);
void exit();
void clean();
void init();
void cmd();
template <typename T> void easy_new(T *&ptr, const char *s) { ptr = new T(s); }
