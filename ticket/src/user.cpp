#include "user.hpp"
#include "BPT.hpp"
#include "Filer.hpp"
#include "map.hpp"
User::User() {}
User::User(const Username &u, const Password &p, const Name &n,
           const MailAddr &m, const int &g)
    : username(u), password(p), name(n), mailAddr(m), privilege(g) {}
Filer<User, 1> userdata("userdata");
BPT<pair<Username, int>> userindex("userindex");
// BPT<pair<Username, int>> userprivilege("userprivilege");
map<Username, int> loggeduser;
typedef map<Username, int>::iterator it;
bool firstuser;
void user_init() { firstuser = userdata.blank(); }
int add_user(Command &c) {
    int g;
    Username tmp(c['c']);
    if (firstuser) {
        g = 10;
        firstuser = 0;
    } else {
        g = stoi(c['g']);
        it I = loggeduser.find(tmp);
        if (I == loggeduser.end() || I->second <= g) {
            return -1;
        }
    }
    userindex.insert(pair<Username, int>(
        tmp, userdata.push(User(tmp, c['p'], c['n'], c['m'], g))));
    return 0;
}