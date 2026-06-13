#include "command.hpp"
#include "Filer.hpp"
#include "order.hpp"
#include "predef.hpp"
#include "train.hpp"
#include "user.hpp"
#include <cstdio>
#include <cstdlib>
#include <filesystem>
string &Command::operator[](char c) { return argument[c - 'a']; }
void init() {
    easy_new(userData, "userData");
    easy_new(userIndex, "userIndex");
    easy_new(trainData, "trainData");
    easy_new(trainIndex, "trainIndex");
    easy_new(releasedTrainIndex, "releasedTrainIndex");
    easy_new(seatData, "seatData");
    easy_new(seatIndex, "seatIndex");
    easy_new(releasedSeatNum, "releasedSeatNum");
    easy_new(leaveTrain, "leaveTrain");
    easy_new(arriveTrain, "arriveTrain");
    easy_new(orderIndex, "orderIndex");
    easy_new(pendingOrder, "pendingOrder");
    easy_new(orderData, "orderData");
    user_init();
}
void clean() {
    exit();
    for (auto i : directory_iterator(".")) {
        if (i.path().extension() == ".data") {
            try {
                std::filesystem::remove(i);
            } catch (...) {
                throw "unknown error in clean";
            }
        }
    }
    init();
}
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
void exit() {
    delete userData;
    delete userIndex;
    delete trainData;
    delete trainIndex;
    delete releasedTrainIndex;
    delete seatData;
    delete seatIndex;
    delete releasedSeatNum;
    delete leaveTrain;
    delete arriveTrain;
    delete orderData;
    delete pendingOrder;
    delete orderIndex;
}
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
            add_user_cnt++;
            add_user(tmp);
        } else if (type == "login") {
            login_cnt++;
            login(tmp);
        } else if (type == "logout") {
            logout_cnt++;
            logout(tmp);
        } else if (type == "query_profile") {
            query_profile_cnt++;
            query_profile(tmp);
        } else if (type == "modify_profile") {
            modify_profile_cnt++;
            modify_profile(tmp);
        } else if (type == "exit") {
            exit_cnt++;
            cout << "bye" << endl;
            break;
        } else if (type == "clean") {
            clean_cnt++;
            clean();
        } else if (type == "query_train") {
            query_train_cnt++;
            query_train(tmp);
        } else if (type == "delete_train") {
            delete_train_cnt++;
            delete_train(tmp);
        } else if (type == "add_train") {
            add_train_cnt++;
            add_train(tmp);
        } else if (type == "release_train") {
            release_train_cnt++;
            release_train(tmp);
        } else if (type == "buy_ticket") {
            buy_ticket_cnt++;
            buy_ticket(tmp);
        } else if (type == "query_ticket") {
            query_ticket_cnt++;
            query_ticket(tmp);
        } else if (type == "query_order") {
            query_order_cnt++;
        } else if (type == "refund_ticket") {
            refund_ticket_cnt++;
        } else if (type == "query_transfer") {
            query_transfer_cnt++;
        } else {
            throw "invalid <cmd>:" + type;
        }
    }
    exit();
}
void command_cnt() {
    cout << "\n\n===================\n\n";
    cout << "add_user_cnt " << add_user_cnt << "\n";
    cout << "login_cnt " << login_cnt << "\n";
    cout << "logout_cnt " << logout_cnt << "\n";
    cout << "query_profile_cnt " << query_profile_cnt << "\n";
    cout << "modify_profile_cnt " << modify_profile_cnt << "\n";
    cout << "add_train_cnt " << add_train_cnt << "\n";
    cout << "delete_train_cnt " << delete_train_cnt << "\n";
    cout << "release_train_cnt " << release_train_cnt << "\n";
    cout << "query_train_cnt " << query_train_cnt << "\n";
    cout << "query_ticket_cnt " << query_ticket_cnt << "\n";
    cout << "query_transfer_cnt " << query_transfer_cnt << "\n";
    cout << "buy_ticket_cnt " << buy_ticket_cnt << "\n";
    cout << "query_order_cnt " << query_order_cnt << "\n";
    cout << "refund_ticket_cnt " << refund_ticket_cnt << "\n";
    cout << "exit_cnt " << exit_cnt << "\n";
    cout << "clean_cnt " << clean_cnt << "\n";
}
int add_user_cnt;
int login_cnt;
int logout_cnt;
int query_profile_cnt;
int modify_profile_cnt;
int add_train_cnt;
int delete_train_cnt;
int release_train_cnt;
int query_train_cnt;
int query_ticket_cnt;
int query_transfer_cnt;
int buy_ticket_cnt;
int query_order_cnt;
int refund_ticket_cnt;
int exit_cnt;
int clean_cnt;
