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
void command_cnt();
extern int add_user_cnt;
extern int login_cnt;
extern int logout_cnt;
extern int query_profile_cnt;
extern int modify_profile_cnt;
extern int add_train_cnt;
extern int delete_train_cnt;
extern int release_train_cnt;
extern int query_train_cnt;
extern int query_ticket_cnt;
extern int query_transfer_cnt;
extern int buy_ticket_cnt;
extern int query_order_cnt;
extern int refund_ticket_cnt;
extern int exit_cnt;
extern int clean_cnt;
