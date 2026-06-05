#pragma once
#include "arraystring.hpp"
#include "command.hpp"
typedef arraystring<21> Username;
typedef arraystring<31> Password;
typedef arraystring<5 * cn + 1> Name;
typedef arraystring<31> MailAddr;
class User {
  public:
    Username username;
    Password password;
    Name name;
    MailAddr mailAddr;
    int privilege;
    User();
    User(const Username &u, const Password &p, const Name &n, const MailAddr &m,
         const int &g);
};
void user_init();
int add_user(const Command &c);