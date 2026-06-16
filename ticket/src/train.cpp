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
    static decltype(get_BPT_pointer_T(trainIndex)) tmp;
    tmp.first = c['i'];
    tmp.second = INT_MINIMUN;
    if (BPTValue(trainIndex, tmp)) {
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
        decltype(get_BPT_pointer_T(trainIndex))(t.trainID, trainData->push(t)));
    cout << 0 << endl;
}
void delete_train(Command &c) {
    static decltype(get_BPT_pointer_T(trainIndex)) tmp;
    tmp.first = c['i'];
    tmp.second = INT_MINIMUN;
    if (!BPTValue(trainIndex, tmp)) {
        cout << -1 << endl;
        return;
    }
    if (BPTContain(releasedTrainIndex, tmp.second)) {
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
    static decltype(get_BPT_pointer_T(trainIndex)) tmp;
    tmp.first = c['i'];
    tmp.second = INT_MINIMUN;
    if (!BPTValue(trainIndex, tmp)) {
        cout << -1 << endl;
        return;
    }
    if (BPTContain(releasedTrainIndex, tmp.second)) {
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
    decltype(get_BPT_pointer_T(trainIndex)) tmp(c['i'], INT_MINIMUN);
    if (!BPTValue(trainIndex, tmp)) {
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
    decltype(get_BPT_pointer_T(seatIndex)) tmp2(pair<int, int>(d, tmp.second),
                                                INT_MINIMUN);
    if (BPTValue(seatIndex, tmp2)) {
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
                 << minute_to_time(t.leaveTime[i] % (60 * 24)) << " ";
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
    static decltype(get_BPT_pointer_T(leaveTrain)) tmp1;
    static decltype(get_BPT_pointer_T(arriveTrain)) tmp2;
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
    int d = date_to_day(c['d']); // 从<from>离开日
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
    static decltype(get_BPT_pointer_T(seatIndex)) tmp3;
    static decltype(get_BPT_pointer_T(releasedSeatNum)) tmp4;
    static int sn[100];
    if (cnt) {
        if (c['p'] == "time" || c['p'].empty()) {
            sort(0, cnt - 1, ans, bans, PairedTrain::cmpt);
        } else {
            sort(0, cnt - 1, ans, bans, PairedTrain::cmpp);
        }
        for (int i = 0; i < cnt; i++) {
            cout << ans[i].trainID << " ";
            cout << c['s'] << " " << c['d'] << " ";
            cout << minute_to_time(ans[i].leaveTime % DAY_MINUTES) << " -> ";
            cout << c['t'] << " ";
            cout << day_to_date(d - ans[i].leaveTime / DAY_MINUTES +
                                ans[i].arriveTime / DAY_MINUTES)
                 << " ";
            cout << minute_to_time(ans[i].arriveTime % DAY_MINUTES) << " ";
            cout << ans[i].price << " ";
            // cout << endl;
            tmp3.first.first = d - ans[i].leaveTime / DAY_MINUTES;
            tmp3.first.second = ans[i].trainIndex;
            tmp3.second = INT_MINIMUN;
            if (!BPTValue(seatIndex, tmp3)) {
                tmp4.first = ans[i].trainIndex;
                tmp4.second = INT_MINIMUN;
                BPTValue(releasedSeatNum, tmp4);
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
    static decltype(get_BPT_pointer_T(userIndex)) tmpu;
    tmpu.first = c['u'];
    tmpu.second = INT_MINIMUN;
    if (!BPTValue(userIndex, tmpu)) {
        throw "abab";
    }
    static decltype(get_BPT_pointer_T(trainIndex)) tmp1;
    tmp1.first = c['i'];
    tmp1.second = INT_MINIMUN;
    if (!BPTValue(trainIndex, tmp1)) {
        cout << -1 << endl;
        return;
    }
    if (!BPTContain(releasedTrainIndex, tmp1.second)) {
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
    int d = date_to_day(c['d']) - t.leaveTime[pos1] / DAY_MINUTES; // 发车日
    if (d < t.beginDay || d > t.endDay) {
        cout << -1 << endl;
        return;
    }
    static decltype(get_BPT_pointer_T(seatIndex)) tmp2;
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
        static void mk(order_detailed &od, int tt, Train &t, int ti, int pos1,
                       int pos2, int n, int sp, int d) {
            od.begin = t.stations[pos1];
            od.end = t.stations[pos2];
            od.leaveTime = t.leaveTime[pos1];
            od.arriveTime = t.arriveTime[pos2 - 1];
            od.num = n;
            od.price = sp;
            od.trainID = t.trainID;
            od.day = d;
            od.trainIndex = ti;
            od.beginPos = pos1;
            od.endPos = pos2;
            od.timestamp = tt;
        }
    };
    if (!BPTValue(seatIndex, tmp2)) {
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
        mkod::mk(od, c.timestamp, t, tmp1.second, pos1, pos2, n, sp, d);
        orderIndex->insert(pair<int, pair<int, int>>(
            tmpu.second, pair<int, int>(-c.timestamp, orderData->push(od))));
        cout << sp * n << endl;
    } else {
        // cout << "get it" << "\n";
        seatData->read(tmp2.second, sn);
        // for (int i = 0; i < t.stationNum; i++) {
        //     cout << sn[i] << ",";
        // }
        // cout << "\n";
        int mn = sn[pos1];
        for (int i = pos1 + 1; i < pos2; i++) {
            mn = min(sn[i], mn);
        }
        static decltype(get_BPT_pointer_T(pendingOrder)) tmpo;
        if (mn < n) {
            if (c['q'] == "true") {
                od.status = STATUS::PENDING;
                mkod::mk(od, c.timestamp, t, tmp1.second, pos1, pos2, n, sp, d);
                int oindex = orderData->push(od);
                orderIndex->insert(pair<int, pair<int, int>>(
                    tmpu.second, pair<int, int>(-c.timestamp, oindex)));
                tmpo.first.first = od.day;
                tmpo.first.second = od.trainIndex;
                tmpo.second.begin = od.beginPos;
                tmpo.second.end = od.endPos;
                tmpo.second.timestamp = od.timestamp;
                tmpo.second.num = od.num;
                tmpo.second.index = oindex;
                pendingOrder->insert(tmpo);
                cout << "queue" << endl;
                return;
            } else {
                cout << -1 << endl;
                return;
            }
        }
        for (int i = pos1; i < pos2; i++) {
            sn[i] -= n;
        }
        seatData->update(tmp2.second, sn);
        od.status = STATUS::SUCCESS;
        mkod::mk(od, c.timestamp, t, tmp1.second, pos1, pos2, n, sp, d);
        orderIndex->insert(pair<int, pair<int, int>>(
            tmpu.second, pair<int, int>(-c.timestamp, orderData->push(od))));
        cout << sp * n << endl;
    }
}
void query_order(Command &c) {
    if (loggeduser.find(c['u']) == loggeduser.end()) {
        cout << -1 << endl;
        return;
    }
    static decltype(get_BPT_pointer_T(userIndex)) tmp1;
    tmp1.first = c['u'];
    tmp1.second = INT_MINIMUN;
    if (!BPTValue(userIndex, tmp1)) {
        throw "unknown error in query_order";
    }
    static decltype(get_BPT_pointer_T(orderIndex)) tmp2;
    tmp2.first = tmp1.second;
    tmp2.second = pair<int, int>(INT_MINIMUN, INT_MINIMUN);
    orderIndex->Gpos = orderIndex->lower_bound(tmp2);
    int cnt = 0;
    while (!orderIndex->GposInvalid() &&
           orderIndex->Gvalue().first == tmp1.second) {
        cnt++;
        orderIndex->plusGpos();
    }
    cout << cnt << endl;
    orderIndex->Gpos = orderIndex->lower_bound(tmp2);
    static order_detailed od;
    while (!orderIndex->GposInvalid() &&
           orderIndex->Gvalue().first == tmp1.second) {
        int index = orderIndex->Gvalue().second.second;
        orderData->read(index, od);
        cout << od << endl;
        orderIndex->plusGpos();
    }
}
void refund_ticket(Command &c) {
    if (loggeduser.find(c['u']) == loggeduser.end()) {
        // cout << "tt";
        cout << -1 << endl;
        return;
    }
    static decltype(get_BPT_pointer_T(userIndex)) tmp1;
    tmp1.first = c['u'];
    tmp1.second = INT_MINIMUN;
    if (!BPTValue(userIndex, tmp1)) {
        throw "unknown error in refund_ticket";
    }
    static decltype(get_BPT_pointer_T(orderIndex)) tmp2;
    tmp2.first = tmp1.second;
    tmp2.second = pair<int, int>(INT_MINIMUN, INT_MINIMUN);
    orderIndex->Gpos = orderIndex->lower_bound(tmp2);
    int n;
    if (c['n'].empty()) {
        n = 1;
    } else {
        n = stoi(c['n']);
    }
    if (n < 1) {
        // cout << "bb";
        cout << -1 << endl;
        return;
    }
    while (!orderIndex->GposInvalid() &&
           orderIndex->Gvalue().first == tmp1.second) {
        n--;
        if (!n) {
            break;
        }
        orderIndex->plusGpos();
    }
    if (n) {
        // cout << "zz";
        cout << -1 << endl;
        return;
    }
    static order_detailed od;
    orderData->read(orderIndex->Gvalue().second.second, od);
    if (od.status == STATUS::REFUNDED) {
        // cout << "!!";
        cout << -1 << endl;
        return;
    }
    static decltype(get_BPT_pointer_T(pendingOrder)) tmp3;
    if (od.status == STATUS::PENDING) {
        od.status = STATUS::REFUNDED;
        orderData->update(orderIndex->Gvalue().second.second, od);
        tmp3.first.first = od.day;
        tmp3.first.second = od.trainIndex;
        // tmp3.second.index = orderIndex->Gvalue().second.second;
        // tmp3.second.begin = od.beginPos;
        // tmp3.second.end = od.endPos;
        // tmp3.second.num = od.num;
        tmp3.second.timestamp = od.timestamp;
        pendingOrder->del(tmp3);
        cout << 0 << endl;
        return;
    }
    od.status = STATUS::REFUNDED;
    orderData->update(orderIndex->Gvalue().second.second, od);
    static decltype(get_BPT_pointer_T(seatIndex)) tmp4;
    tmp4.first.first = od.day;
    tmp4.first.second = od.trainIndex;
    tmp4.second = INT_MINIMUN;
    if (!BPTValue(seatIndex, tmp4)) {
        throw "unknown error in refund_ticket3";
    }
    static int sn[100];
    seatData->read(tmp4.second, sn);
    for (int i = od.beginPos; i < od.endPos; i++) {
        sn[i] += od.num;
    }
    static decltype(get_BPT_pointer_T(pendingOrder)) tmp5;
    tmp5.first.first = od.day;
    tmp5.first.second = od.trainIndex;
    tmp5.second.timestamp = INT_MINIMUN;
    pendingOrder->Gpos = pendingOrder->lower_bound(tmp5);
    static int rmo[1000005];
    int rn = 0;
    while (!pendingOrder->GposInvalid() &&
           pendingOrder->Gvalue().first == tmp5.first) {
        order &o = pendingOrder->Gvalue().second;
        int mn = sn[o.begin];
        for (int i = o.begin + 1; i < o.end; i++) {
            mn = min(mn, sn[i]);
        }
        if (mn >= o.num) {
            orderData->read(o.index, od);
            if (od.status != STATUS::PENDING) {
                throw "pending error";
            }
            rmo[rn++] = o.timestamp;
            for (int i = o.begin; i < o.end; i++) {
                sn[i] -= o.num;
            }
            od.status = STATUS::SUCCESS;
            orderData->update(o.index, od);
        }
        pendingOrder->plusGpos();
    }
    for (int i = 0; i < rn; i++) {
        tmp5.second.timestamp = rmo[i];
        pendingOrder->del(tmp5);
    }
    seatData->update(tmp4.second, sn);
    cout << 0 << endl;
}
void query_transfer(Command &c) {
    // cout << 0 << endl;
    // return;
    if (c['s'] == c['t']) {
        throw "meaningless ticket";
    }
    static decltype(get_BPT_pointer_T(leaveTrain)) tmp1;
    static decltype(get_BPT_pointer_T(arriveTrain)) tmp2;
    tmp1.first = c['s'];
    tmp1.second.trainIndex = INT_MINIMUN;
    tmp2.first = c['t'];
    tmp2.first = c['t'];
    leaveTrain->Gpos = leaveTrain->lower_bound(tmp1);
    arriveTrain->Gpos = arriveTrain->lower_bound(tmp2);
    constexpr int maxpt = 20005;
    static Train ft, tt;
    static int day1[maxpt], beginPos[maxpt], endPos[maxpt], preprice[maxpt];
    // static LeaveTrain fl[maxpt];
    static int fi[maxpt], ti[maxpt];
    int fn = 0, tn = 0;
    int d = date_to_day(c['d']); // 离开<from>日
    while (!leaveTrain->GposInvalid() &&
           leaveTrain->Gvalue().first == tmp1.first) {
        fi[fn] = leaveTrain->Gvalue().second.trainIndex;
        trainData->read(fi[fn], ft);
        beginPos[fn] = leaveTrain->Gvalue().second.pos;
        day1[fn] = d - ft.leaveTime[beginPos[fn]] / DAY_MINUTES;
        fn++;
        if (fn >= maxpt) {
            throw "too much f";
        }
        leaveTrain->plusGpos();
    }
    maxPassTrain = max(maxPassTrain, fn);
    while (!arriveTrain->GposInvalid() &&
           arriveTrain->Gvalue().first == tmp2.first) {
        ti[tn] = arriveTrain->Gvalue().second.trainIndex;
        trainData->read(ti[tn], tt);
        endPos[tn] = arriveTrain->Gvalue().second.pos;
        preprice[tn] = 0;
        for (int i = 0; i < endPos[tn]; i++) {
            preprice[tn] += tt.prices[i];
        }
        tn++;
        if (tn >= maxpt) {
            throw "too much t";
        }
        arriveTrain->plusGpos();
    }
    maxPassTrain = max(maxPassTrain, tn);
    class Ans {
      public:
        // int trainIndex1;
        // int trainIndex2;
        int trainPos1;
        int trainPos2;
        int middlePos1; // mid 在ft[i]中位置
        int middlePos2;
        int price;
        int price1;
        int price2;
        int day2;
        int time;
        static bool cmp(const Ans &a, const Ans &b, Train &t1, Train &t2) {
            if (t1.trainID < t2.trainID)
                return 1;
            if (t1.trainID > t2.trainID)
                return 0;
            if (t1.trainID < t2.trainID)
                return 1;
            if (t1.trainID > t2.trainID)
                return 0;
            return 0;
        }
        static bool cmpt(const Ans &a, const Ans &b, Train &t1, Train &t2) {
            if (a.time < b.time)
                return 1;
            if (a.time > b.time)
                return 0;
            if (a.price < b.price)
                return 1;
            if (a.price > b.price)
                return 0;
            return cmp(a, b, t1, t2);
        }
        static bool cmpp(const Ans &a, const Ans &b, Train &t1, Train &t2) {
            if (a.price < b.price)
                return 1;
            if (a.price > b.price)
                return 0;
            if (a.time < b.time)
                return 1;
            if (a.time > b.time)
                return 0;
            return cmp(a, b, t1, t2);
        }
        static int getSeat(Train &t, int trainIndex, int day, int pos1,
                           int pos2) {
            static decltype(get_BPT_pointer_T(seatIndex)) tmp;
            tmp.first.first = day;
            tmp.first.second = trainIndex;
            tmp.second = INT_MINIMUN;
            if (!BPTValue(seatIndex, tmp)) {
                return t.seatNum;
            }
            static int sn[100];
            seatData->read(tmp.second, sn);
            int mn = sn[pos1];
            for (int i = pos1 + 1; i < pos2; i++) {
                mn = min(mn, sn[i]);
            }
            return mn;
        }
        static void mkod(order_detailed &od, int trainIndex, Train &t, int day,
                         int pos1, int pos2, int price) {
            od.trainID = t.trainID;
            od.day = day;
            od.begin = t.stations[pos1];
            od.end = t.stations[pos2];
            od.leaveTime = t.leaveTime[pos1];
            od.arriveTime = t.arriveTime[pos2 - 1];
            od.price = price;
            od.num = getSeat(t, trainIndex, day, pos1, pos2);
        }
    };
    bool fg = 1;
    static Ans ans, tmpa;
    for (int i = 0; i < fn; i++) {
        trainData->read(fi[i], ft);
        int day1 = d - ft.leaveTime[beginPos[i]] / DAY_MINUTES;
        if (day1 < ft.beginDay || day1 > ft.endDay) {
            continue;
        }
        for (int j = 0; j < tn; j++) {
            trainData->read(ti[j], tt);
            if (ft.trainID == tt.trainID) {
                continue;
            }
            int prep = 0;
            for (int I = beginPos[i] + 1; I < ft.stationNum; I++) {
                prep += ft.prices[I - 1];
                int subp = preprice[j];
                for (int J = 0; J < endPos[j]; subp -= tt.prices[J], J++) {
                    if (ft.stations[I] != tt.stations[J]) {
                        continue;
                    }
                    // int I = beginPos[i] + 1, J = 0;
                    // while (I < ft[i].stationNum && J < endPos[j]) {
                    // if (ft[i].stations[I] < tt[j].stations[J]) {
                    //     prep += ft[i].prices[I];
                    //     I++;
                    //     continue;
                    // }
                    // if (ft[i].stations[I] > tt[j].stations[J]) {
                    //     subp -= tt[j].prices[J];
                    //     J++;
                    //     continue;
                    // }
                    tmpa.day2 = day1 + ft.arriveTime[I - 1] / DAY_MINUTES -
                                tt.leaveTime[J] / DAY_MINUTES;
                    if (ft.arriveTime[I - 1] % DAY_MINUTES >
                        tt.leaveTime[J] % DAY_MINUTES) {
                        tmpa.day2++;
                    }
                    if (tmpa.day2 <= tt.endDay) {
                        tmpa.day2 = max(tmpa.day2, tt.beginDay);
                        tmpa.middlePos1 = I;
                        tmpa.middlePos2 = J;
                        tmpa.price = prep + subp;
                        tmpa.price1 = prep;
                        tmpa.price2 = subp;
                        tmpa.time = (tmpa.day2 - day1) * DAY_MINUTES +
                                    tt.arriveTime[endPos[j] - 1] -
                                    ft.leaveTime[beginPos[i]];
                        tmpa.trainPos1 = i;
                        tmpa.trainPos2 = j;
                        // cout << ft[i].stations[I] << "," << tt[j].stations[J]
                        //      << endl;
                        if (fg) {
                            fg = 0;
                            ans = tmpa;
                        } else {
                            if (c['p'] == "cost") {
                                if (Ans::cmpp(tmpa, ans, ft, tt)) {
                                    ans = tmpa;
                                }
                            } else {
                                if (Ans::cmpt(tmpa, ans, ft, tt)) {
                                    ans = tmpa;
                                }
                            }
                        }
                    }
                }
                // prep += ft[i].prices[I];
                // I++;
                // subp -= tt[j].prices[J];
                // J++;
            }
        }
    }
    if (fg) {
        cout << 0 << endl;
    } else {
        static order_detailed od;
        int i = ans.trainPos1, j = ans.trainPos2;
        trainData->read(fi[i], ft);
        trainData->read(ti[j], tt);
        Ans::mkod(od, fi[i], ft, d - ft.leaveTime[beginPos[i]] / DAY_MINUTES,
                  beginPos[i], ans.middlePos1, ans.price1);
        order_detailed::print(cout, od);
        cout << endl;
        Ans::mkod(od, ti[j], tt, ans.day2, ans.middlePos2, endPos[j],
                  ans.price2);
        order_detailed::print(cout, od);
        cout << endl;
        // cout << "time=" << ans.time << endl;
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
int maxPassTrain;