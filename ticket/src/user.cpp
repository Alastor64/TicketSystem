#include "user.hpp"
#include "BPT.hpp"
#include "Filer.hpp"
#include "command.hpp"
#include "map.hpp"
#include "predef.hpp"
#include <utility>
// #define need_userprivilege
User::User() {}
User::User(const Username &u, const Password &p, const Name &n,
           const MailAddr &m, const int &g)
    : username(u), password(p), name(n), mailAddr(m), privilege(g) {}
Filer<User, 1> *userData;
BPT<pair<Username, int>> *userIndex;
#ifdef need_userprivilege
BPT<pair<Username, int>> *userprivilege;
#endif
map<Username, int> loggeduser;
bool firstuser;
void user_init() { firstuser = userData->blank(); }
void add_user(Command &c) { //[N]
    int g;
    if (firstuser) {
        g = 10;
        firstuser = 0;
    } else {
        g = stoi(c['g']);
        decltype(loggeduser)::iterator I = loggeduser.find(c['c']);
        if (I == loggeduser.end() || I->second <= g) {
            cout << -1 << endl;
            return;
        }
    }
#ifdef need_userprivilege
    userprivilege.insert(pair<Username, int>(c['u'], g));
#endif
    userIndex->insert(pair<Username, int>(
        c['u'], userData->push(User(c['u'], c['p'], c['n'], c['m'], g))));
    cout << 0 << endl;
}
void login(Command &c) { //[F]
    if (loggeduser.find(c['u']) != loggeduser.end()) {
        cout << -1 << endl;
        return;
    }
    // cout << "zz\n";
    decltype(get_BPT_T(*userIndex)) tmp(c['u'], INT_MINIMUN);
    if (!BPTValue(*userIndex, tmp)) {
        cout << -1 << endl;
        return;
    }
    // cout << "zz\n";
    int index = tmp.second;
    static User u;
    userData->read(index, u);
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
    if (!BPTValue(*userIndex, tmp)) {
        cout << -1 << endl;
        return;
    }
    static User u;
    userData->read(tmp.second, u);
    if (u.privilege > g || (u.privilege == g && c['u'] != c['c'])) {
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
    if (!BPTValue(*userIndex, tmp)) {
        cout << -1 << endl;
        return;
    }
    static User u;
    userData->read(tmp.second, u);
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
        userData->update(tmp.second, u);
        cout << u.username << " " << u.name << " " << u.mailAddr << " "
             << u.privilege << endl;
    } else {
        cout << -1 << endl;
    }
}