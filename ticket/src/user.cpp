#include "user.hpp"
#include "BPT.hpp"
#include "Filer.hpp"
#include "command.hpp"
#include "map.hpp"
#include "predef.hpp"
#include <utility>
#define need_userprivilege
User::User() {}
User::User(const Username &u, const Password &p, const Name &n,
           const MailAddr &m, const int &g)
    : username(u), password(p), name(n), mailAddr(m), privilege(g) {}
Filer<User, 1> userdata("userdata");
BPT<pair<Username, int>> userindex("userindex");
#ifdef need_userprivilege
BPT<pair<Username, int>> userprivilege("userprivilege");
#endif
map<Username, int> loggeduser;
typedef map<Username, int>::iterator it;
bool firstuser;
void user_init() { firstuser = userdata.blank(); }
void add_user(Command &c) { //[N]
    int g;
    Username tmp(c['c']);
    if (firstuser) {
        g = 10;
        firstuser = 0;
    } else {
        g = stoi(c['g']);
        it I = loggeduser.find(tmp);
        if (I == loggeduser.end() || I->second <= g) {
            cout << -1 << endl;
            return;
        }
    }
#ifdef need_userprivilege
    userprivilege.insert(pair<Username, int>(tmp, g));
#endif
    userindex.insert(pair<Username, int>(
        tmp, userdata.push(User(tmp, c['p'], c['n'], c['m'], g))));
    cout << 0 << endl;
}
void login(Command &c) { //[F]
    pair<Username, int> tmp(c['u'], INT_MINIMUN);
    if (loggeduser.find(c['u']) != loggeduser.end()) {
        cout << -1 << endl;
        return;
    }
    if (!bptValue(userindex, tmp)) {
        cout << -1 << endl;
        return;
    }
    int index = tmp.second;
    User u;
    userdata.read(index, u);
    if (u.password != Password(c['p'])) {
        cout << -1 << endl;
        return;
    }
    loggeduser[c['u']] = u.privilege;
    cout << 0 << endl;
}
void logout(Command &c) { //[F]
    decltype(loggeduser)::iterator pos = loggeduser.find(c['u']);
    if (pos != loggeduser.end()) {
        loggeduser.erase(pos);
        cout << 0 << endl;
    } else {
        cout << -1 << endl;
    }
}
void query_profile(Command &c) { //[SF]
    decltype(loggeduser)::iterator pos = loggeduser.find(c['c']);
    if (pos == loggeduser.end()) {
        cout << -1 << endl;
        return;
    }
    int g = pos->second;
    pair<Username, int> tmp(c['u'], INT_MINIMUN);
    if (!bptValue(userindex, tmp)) {
        cout << -1 << endl;
        return;
    }
    User u;
    userdata.read(tmp.second, u);
    if (u.privilege > g) {
        cout << -1 << endl;
        return;
    }
    cout << u.username << " " << u.name << " " << u.mailAddr << " "
         << u.privilege << endl;
}
void modify_profile(Command &c) {
    decltype(loggeduser)::iterator pos = loggeduser.find(c['c']);
    if (pos == loggeduser.end()) {
        cout << -1 << endl;
        return;
    }
    int g = pos->second;
    pair<Username, int> tmp(c['u'], INT_MINIMUN);
    if (!bptValue(userindex, tmp)) {
        cout << -1 << endl;
        return;
    }
    User u;
    userdata.read(tmp.second, u);
    if ((g > u.privilege || c['c'] == c['u']) &&
        (c['g'].empty() || stoi(c['g']) < g)) {
        if (!c['p'].empty()) {
            u.password = c['p'];
        }
        if (!c['m'].empty()) {
            u.mailAddr = c['m'];
        }
        if (!c['n'].empty()) {
            u.name = c['n'];
        }
        if (!c['g'].empty()) {
            u.privilege = stoi(c['g']);
        }
        userdata.update(tmp.second, u);
        cout << u.username << " " << u.name << " " << u.mailAddr << " "
             << u.privilege << endl;
    } else {
        cout << -1 << endl;
    }
}