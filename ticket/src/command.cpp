#include "command.hpp"
string Command::operator[](char c) { return argument[c - 'a']; }