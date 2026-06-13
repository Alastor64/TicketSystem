#include "train.hpp"
#include "BPT.hpp"
#include "Filer.hpp"
#include "arraystring.hpp"
#include "command.hpp"
#include "mysort.hpp"
#include "order.hpp"
#include "predef.hpp"
#include "user.hpp"
Filer<Train> *trainData;
BPT<pair<TrainID, int>> *trainIndex;
BPT<int> *releasedTrainIndex;
Filer<int[100]> *seatData;
BPT<pair<pair<int, int>, int>> *seatIndex;
BPT<pair<int, int>> *releasedSeatNum;
BPT<pair<StationName, LeaveTrain>> *leaveTrain;
BPT<pair<StationName, ArriveTrain>> *arriveTrain;
int date_to_day(const string &s) { // s=mm-dd
    int m = s[1] - '6';
    if (m < 0 || m > 2)
        return -1;
    int d = (s[3] - '0') * 10 + s[4] - '0';
    if (d < 1 || d > 31)
        return -1;
    if (d == 31 && m == 0)
        return -1;
    static int md[3] = {0, 30, 30 + 31};
    return d + md[m];
}
string day_to_date(int day) {
    string tmp("0*-**");
    if (day < 1) {
        throw "invalid day num";
    } else if (day <= 30) {
        tmp[1] = '6';
    } else if (day <= 61) {
        tmp[1] = '7';
        day -= 30;
    } else if (day <= 92) {
        tmp[1] = '8';
        day -= 61;
    } else {
        tmp[1] = '9';
        day -= 92;
    }
    tmp[3] = day / 10 + '0';
    tmp[4] = day % 10 + '0';
    return std::move(tmp);
}
int time_to_minute(string &s) {
    return stoi(s.substr(3, 2)) + 60 * stoi(s.substr(0, 2));
}
string minute_to_time(int m) {
    string tmp("**:**");
    tmp[0] = m / 600 + '0';
    m %= 600;
    tmp[1] = m / 60 + '0';
    m %= 60;
    tmp[3] = m / 10 + '0';
    tmp[4] = m % 10 + '0';
    return std::move(tmp);
}
Train::Train() {}
void Train::load(Command &c, int d1, int d2) {
    trainID = c['i'];
    stationNum = stoi(c['n']);
    seatNum = stoi(c['m']);
    // cout << "zz" << endl;
    split(stations, c['s']);
    // cout << "abab:" << stations[0] << " ";
    split(prices, c['p']);
    int startTime = time_to_minute(c['x']);
    static int travelTime[100], stopoverTime[100];
    split(travelTime, c['t']);
    split(stopoverTime, c['o']);
    leaveTime[0] = startTime;
    for (int i = 1; i < stationNum - 1; i++) {
        arriveTime[i - 1] = leaveTime[i - 1] + travelTime[i - 1];
        leaveTime[i] = arriveTime[i - 1] + stopoverTime[i - 1];
    }
    arriveTime[stationNum - 2] =
        leaveTime[stationNum - 2] + travelTime[stationNum - 2];
    beginDay = d1;
    endDay = d2;
    type = c['y'].front();
}
void add_train(Command &c) {
    static decltype(get_BPT_T(*trainIndex)) tmp;
    tmp.first = c['i'];
    tmp.second = INT_MINIMUN;
    if (BPTValue(*trainIndex, tmp)) {
        cout << -1 << endl;
        return;
    }
    int d1 = date_to_day(c['d'].substr(0, 5));
    int d2 = date_to_day(c['d'].substr(6, 5));
    if (d1 < 0 || d2 < 0) {
        cout << -1 << endl;
        return;
    }
    static Train t;
    t.load(c, d1, d2);
    trainIndex->insert(
        decltype(get_BPT_T(*trainIndex))(t.trainID, trainData->push(t)));
    cout << 0 << endl;
}
void delete_train(Command &c) {
    static decltype(get_BPT_T(*trainIndex)) tmp;
    tmp.first = c['i'];
    tmp.second = INT_MINIMUN;
    if (!BPTValue(*trainIndex, tmp)) {
        cout << -1 << endl;
        return;
    }
    if (BPTContain(*releasedTrainIndex, tmp.second)) {
        cout << -1 << endl;
        return;
    }
    static Train t;
    trainData->read(tmp.second, t);
    trainIndex->del(tmp);
    trainData->pop(tmp.second);
    cout << 0 << endl;
}
void release_train(Command &c) {
    static decltype(get_BPT_T(*trainIndex)) tmp;
    tmp.first = c['i'];
    tmp.second = INT_MINIMUN;
    if (!BPTValue(*trainIndex, tmp)) {
        cout << -1 << endl;
        return;
    }
    if (BPTContain(*releasedTrainIndex, tmp.second)) {
        cout << -1 << endl;
        return;
    }
    static Train t;
    trainData->read(tmp.second, t);
    releasedTrainIndex->insert(tmp.second);
    releasedSeatNum->insert(pair<int, int>(tmp.second, t.seatNum));
    static LeaveTrain lp;
    static ArriveTrain ap;
    lp.trainIndex = ap.trainIndex = tmp.second;
    int sp = 0;
    for (int i = 0; i < t.stationNum - 1; i++) {
        sp += t.prices[i];
    }
    for (int i = 0; i < t.stationNum - 1; i++) {
        lp.leaveTime = t.leaveTime[i];
        lp.beginDay = t.beginDay + t.leaveTime[i] / (60 * 24);
        lp.endDay = t.endDay + t.leaveTime[i] / (60 * 24);
        lp.subprice = sp;
        lp.pos = i;
        sp -= t.prices[i];
        ap.pos = i + 1;
        ap.arriveTime = t.arriveTime[i];
        ap.subprice = sp;
        // cout << t.stations[i] << " ";
        leaveTrain->insert(pair<StationName, LeaveTrain>(t.stations[i], lp));
        arriveTrain->insert(
            pair<StationName, ArriveTrain>(t.stations[i + 1], ap));
    }
    cout << 0 << endl;
}
void query_train(Command &c) {
    decltype(get_BPT_T(*trainIndex)) tmp(c['i'], INT_MINIMUN);
    if (!BPTValue(*trainIndex, tmp)) {
        cout << -1 << endl;
    }
    static Train t;
    trainData->read(tmp.second, t);
    int d = date_to_day(c['d']);
    if (d < t.beginDay || d > t.endDay) {
        cout << -1 << endl;
        return;
    }
    cout << t.trainID << " " << t.type;
    static int sn[100];
    bool fg = 0;
    decltype(get_BPT_T(*seatIndex)) tmp2(pair<int, int>(d, tmp.second),
                                         INT_MINIMUN);
    if (BPTValue(*seatIndex, tmp2)) {
        fg = 1;
        seatData->read(tmp2.second, sn);
    }
    int sp = 0;
    for (int i = 0; i < t.stationNum; i++) {
        cout << "\n" << t.stations[i] << " ";
        if (i == 0) {
            cout << "xx-xx xx:xx -> ";
        } else {
            cout << day_to_date(d + t.arriveTime[i - 1] / (60 * 24)) << " "
                 << minute_to_time(t.arriveTime[i - 1] % (60 * 24)) << " -> ";
        }
        if (i == t.stationNum - 1) {
            cout << "xx-xx xx:xx ";
        } else {
            cout << day_to_date(d + t.leaveTime[i] / (60 * 24)) << " "
                 << minute_to_time(t.leaveTime[i] / (60 * 24)) << " ";
        }
        if (i != 0) {
            sp += t.prices[i - 1];
        }
        cout << sp << " ";
        if (i == t.stationNum - 1) {
            cout << "x";
        } else {
            if (fg) {
                cout << sn[i];
            } else {
                cout << t.seatNum;
            }
        }
    }
    cout << endl;
}
void query_ticket(Command &c) {
    if (c['s'] == c['t']) {
        throw "meaningless ticket";
    }
    static decltype(get_BPT_T(*leaveTrain)) tmp1;
    static decltype(get_BPT_T(*arriveTrain)) tmp2;
    tmp1.first = c['s'];
    tmp1.second.trainIndex = INT_MINIMUN;
    tmp2.first = c['t'];
    tmp2.first = c['t'];
    leaveTrain->Gpos = leaveTrain->lower_bound(tmp1);
    arriveTrain->Gpos = arriveTrain->lower_bound(tmp2);
    // while (!leaveTrain->GposInvalid() &&
    //        leaveTrain->Gvalue().first == tmp1.first) {
    //     cout << leaveTrain->Gvalue().second.trainIndex << "["
    //          << leaveTrain->Gvalue().second.beginDay << ","
    //          << leaveTrain->Gvalue().second.endDay << "]" << "|";
    //     leaveTrain->plusGpos();
    // }
    // cout << "\n";
    // while (!arriveTrain->GposInvalid() &&
    //        arriveTrain->Gvalue().first == tmp2.first) {
    //     cout << arriveTrain->Gvalue().second.trainIndex << "|";
    //     arriveTrain->plusGpos();
    // }
    // cout << "\n";
    int cnt = 0;
    int d = date_to_day(c['d']);
    class PairedTrain {
      public:
        TrainID trainID;
        int trainIndex;
        int leaveTime;
        int sPos;
        int tPos;
        int arriveTime;
        int price;
        static bool cmpt(const PairedTrain &a, const PairedTrain &b) {
            return (a.arriveTime - a.leaveTime < b.arriveTime - b.leaveTime) ||
                   (a.arriveTime - a.leaveTime == b.arriveTime - b.leaveTime &&
                    a.trainID < b.trainID);
        }
        static bool cmpp(const PairedTrain &a, const PairedTrain &b) {
            return (a.price < b.price) ||
                   (a.price == b.price && a.trainID < b.trainID);
        }
    };
    static PairedTrain ans[10005], bans[10005];
    while (!(leaveTrain->GposInvalid() || arriveTrain->GposInvalid()) &&
           leaveTrain->Gvalue().first == tmp1.first &&
           arriveTrain->Gvalue().first == tmp2.first) {
        if (leaveTrain->Gvalue().second.trainIndex >
            arriveTrain->Gvalue().second.trainIndex) {
            arriveTrain->plusGpos();
            continue;
        }
        if (leaveTrain->Gvalue().second.trainIndex <
            arriveTrain->Gvalue().second.trainIndex) {
            leaveTrain->plusGpos();
            continue;
        }
        if (d >= leaveTrain->Gvalue().second.beginDay &&
            d <= leaveTrain->Gvalue().second.endDay &&
            leaveTrain->Gvalue().second.leaveTime <=
                arriveTrain->Gvalue().second.arriveTime) {
            ans[cnt].trainIndex = leaveTrain->Gvalue().second.trainIndex;
            trainData->halfread(ans[cnt].trainIndex, ans[cnt].trainID);
            ans[cnt].leaveTime = leaveTrain->Gvalue().second.leaveTime;
            ans[cnt].arriveTime = arriveTrain->Gvalue().second.arriveTime;
            ans[cnt].price = leaveTrain->Gvalue().second.subprice -
                             arriveTrain->Gvalue().second.subprice;
            ans[cnt].sPos = leaveTrain->Gvalue().second.pos;
            ans[cnt].tPos = arriveTrain->Gvalue().second.pos;
            cnt++;
        }
        leaveTrain->plusGpos();
        arriveTrain->plusGpos();
    }
    maxPairedTrain = max(maxPairedTrain, cnt);
    cout << cnt << endl;
    static decltype(get_BPT_T(*seatIndex)) tmp3;
    static decltype(get_BPT_T(*releasedSeatNum)) tmp4;
    static int sn[100];
    if (cnt) {
        if (c['p'] == "time") {
            sort(0, cnt - 1, ans, bans, PairedTrain::cmpt);
        } else {
            sort(0, cnt - 1, ans, bans, PairedTrain::cmpp);
        }
        for (int i = 0; i < cnt; i++) {
            cout << ans[i].trainID << " ";
            cout << c['s'] << " " << c['d'] << " ";
            cout << minute_to_time(ans[i].leaveTime % (24 * 60)) << " -> ";
            cout << c['t'] << " ";
            cout << day_to_date(d - ans[i].leaveTime / (24 * 60) +
                                ans[i].arriveTime / (24 * 60))
                 << " ";
            cout << minute_to_time(ans[i].arriveTime % (24 * 60)) << " ";
            cout << ans[i].price << " ";
            // cout << endl;
            tmp3.first.first = d - ans[i].leaveTime / (24 * 60);
            tmp3.first.second = ans[i].trainIndex;
            tmp3.second = INT_MINIMUN;
            if (!BPTValue(*seatIndex, tmp3)) {
                tmp4.first = ans[i].trainIndex;
                tmp4.second = INT_MINIMUN;
                BPTValue(*releasedSeatNum, tmp4);
                cout << tmp4.second << endl;
            } else {
                seatData->read(tmp3.second, sn);
                int mn = sn[ans[i].sPos];
                for (int j = ans[i].sPos + 1; j < ans[i].tPos; j++) {
                    mn = min(mn, sn[j]);
                }
                cout << mn << endl;
            }
        }
    }
}
void buy_ticket(Command &c) {
    if (loggeduser.find(c['u']) == loggeduser.end()) {
        cout << -1 << endl;
        return;
    }
    static decltype(get_BPT_T(*trainIndex)) tmp1;
    tmp1.first = c['i'];
    tmp1.second = INT_MINIMUN;
    if (!BPTValue(*trainIndex, tmp1)) {
        cout << -1 << endl;
        return;
    }
    if (!BPTContain(*releasedTrainIndex, tmp1.second)) {
        cout << -1 << endl;
        return;
    }
    static Train t;
    trainData->read(tmp1.second, t);
    int n = stoi(c['n']);
    if (t.seatNum < n) {
        cout << -1 << endl;
        return;
    }
    int pos1(-1), pos2(-1);
    for (int i = 0; i < t.stationNum; i++) {
        if (pos1 == -1 && t.stations[i] == c['f']) {
            pos1 = i;
        }
        if (pos2 == -1 && t.stations[i] == c['t']) {
            pos2 = i;
        }
    }
    // cout << "(" << pos1 << "," << pos2 << ")\n";
    if (pos1 < 0 || pos2 < 0 || pos1 >= pos2) {
        cout << -1 << endl;
        return;
    }
    int d = date_to_day(c['d']) - t.leaveTime[pos1];
    static decltype(get_BPT_T(*seatIndex)) tmp2;
    tmp2.first.first = d;
    tmp2.first.second = tmp1.second;
    tmp2.second = INT_MINIMUN;
    static int sn[100];
    ll sp = 0;
    for (int i = pos1; i < pos2; i++) {
        sp += t.prices[i];
    }
    // cout << "ZZ\n";
    static order_detailed od;
    class mkod { // 单纯为了复用代码
      public:
        static void mk(order_detailed &od, Train &t, int pos1, int pos2, int n,
                       int sp) {
            od.begin = t.stations[pos1];
            od.end = t.stations[pos2];
            od.leaveTime = t.leaveTime[pos1];
            od.arriveTime = t.arriveTime[pos2 - 1];
            od.num = n;
            od.price = sp;
            od.trainID = t.trainID;
        }
    };
    if (!BPTValue(*seatIndex, tmp2)) {
        for (int i = 0; i < t.stationNum - 1; i++) {
            sn[i] = t.seatNum;
            if (i >= pos1 && i < pos2) {
                sn[i] -= n;
            }
        }
        sn[t.stationNum - 1] = -1;
        tmp2.second = seatData->push(sn);
        seatIndex->insert(tmp2);
        od.status = STATUS::SUCCESS;
        mkod::mk(od, t, pos1, pos2, n, sp);
        orderIndex->insert(pair<int, int>(tmp1.second, orderData->push(od)));
        cout << sp * n << endl;
    } else {
        cout << "get it" << "\n";
        seatData->read(tmp2.second, sn);
        for (int i = 0; i < t.stationNum; i++) {
            cout << sn[i] << ",";
        }
        cout << "\n";
        int mn = sn[pos1];
        for (int i = pos1 + 1; i < pos2; i++) {
            mn = min(sn[i], mn);
        }
        if (mn < n) {
            if (c['q'] == "true") {
                od.status = STATUS::PENDING;
                mkod::mk(od, t, pos1, pos2, n, sp);
                orderIndex->insert(
                    pair<int, int>(tmp1.second, orderData->push(od)));
                cout << "queue" << endl;
            } else {
                cout << -1 << endl;
                return;
            }
        }
        for (int i = pos1; i < pos2; i++) {
            sn[i] -= n;
        }
        seatData->update(tmp2.second, sn);
        cout << sp * n << endl;
    }
}
bool LeaveTrain::operator<(const LeaveTrain &x) const {
    return trainIndex < x.trainIndex;
}
bool LeaveTrain::operator>(const LeaveTrain &x) const {
    return trainIndex > x.trainIndex;
}
bool LeaveTrain::operator<=(const LeaveTrain &x) const {
    return trainIndex <= x.trainIndex;
}
bool LeaveTrain::operator>=(const LeaveTrain &x) const {
    return trainIndex >= x.trainIndex;
}
bool LeaveTrain::operator==(const LeaveTrain &x) const {
    return trainIndex == x.trainIndex;
}
bool LeaveTrain::operator!=(const LeaveTrain &x) const {
    return trainIndex != x.trainIndex;
}
bool ArriveTrain::operator<(const ArriveTrain &x) const {
    return trainIndex < x.trainIndex;
}
bool ArriveTrain::operator>(const ArriveTrain &x) const {
    return trainIndex > x.trainIndex;
}
bool ArriveTrain::operator<=(const ArriveTrain &x) const {
    return trainIndex <= x.trainIndex;
}
bool ArriveTrain::operator>=(const ArriveTrain &x) const {
    return trainIndex >= x.trainIndex;
}
bool ArriveTrain::operator==(const ArriveTrain &x) const {
    return trainIndex == x.trainIndex;
}
bool ArriveTrain::operator!=(const ArriveTrain &x) const {
    return trainIndex != x.trainIndex;
}
int buySeatNum;
int maxPairedTrain;
