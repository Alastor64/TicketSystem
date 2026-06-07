#include "command.hpp"
#include "user.hpp"
#include <cstdio>
#include <string>
string &Command::operator[](char c) { return argument[c - 'a']; }
void Command::clear() {
    for (int i = 0; i < 26; i++) {
        argument[i].clear();
    }
}
void getCommand(Command &tmp, string &type) {
    string s;
    getline(cin, s);
    s.push_back(' ');
    int i = s.find_first_of('[', 0);
    int j = s.find_first_of(']', 0);
    if (i > j || i >= s.length() || j >= s.length()) {
        throw "invalid_timestamp_format";
    }
    tmp.timestamp = stoi(s.substr(i + 1, j - i - 1));
    i = s.find_first_not_of(' ', j + 1);
    j = s.find_first_of(' ', i);
    if (i >= s.length() || j >= s.length()) {
        throw "no command <cmd>";
    }
    type = s.substr(i, j - i);
    while ((i = s.find_first_not_of(' ', j)) < s.length()) {
        j = s.find_first_of(' ', i);
        if (j - i != 2 || s[i] != '-' || s[i + 1] > 'z' || s[i + 1] < 'a') {
            throw "invalid_key_format";
        }
        char c = s[i + 1];
        i = s.find_first_not_of(' ', j);
        j = s.find_first_of(' ', i);
        if (i >= s.length() || j >= s.length()) {
            throw "no value";
        }
        tmp[c] = s.substr(i, j - i);
    }
    // cin >> s;
    // if (s.front() != '[' || s.back() != ']') {
    //     throw "invalid_timestamp_format";
    // }
    // tmp.timestamp = stoi(s.substr(1, s.length() - 2));
    // cin >> type;
    // while (cin.peek() == ' ') {
    //     cin >> s;
    //     if (s.length() > 2 || s.front() != '-' || s.back() > 'z' ||
    //         s.back() < 'a') {
    //         throw "invalid_key_format";
    //     }
    //     cin >> tmp[s.back()];
    // }
}
void exit() { cout << "bye" << endl; }
void cmd() {
    Command tmp;
    string type;
    while (!cin.eof()) {
        tmp.clear();
        getCommand(tmp, type);
        cout << "[" << tmp.timestamp << "] ";
        // cout << type;
        // for (char i = 'a'; i <= 'z'; i++) {
        //     if (!tmp[i].empty()) {
        //         cout << "(" << i << "," << tmp[i] << ")";
        //     }
        // }
        // cout << endl;
        if (type == "add_user") {
            add_user(tmp);
        } else if (type == "login") {
            login(tmp);
        } else if (type == "logout") {
            logout(tmp);
        } else if (type == "query_profile") {
            query_profile(tmp);
        } else if (type == "modify_profile") {
            modify_profile(tmp);
        } else if (type == "exit") {
            exit();
            break;
        } else {
            throw "invalid <cmd>:" + type;
        }
    }
}