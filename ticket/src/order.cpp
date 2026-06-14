#include "order.hpp"
#include "predef.hpp"
#include "train.hpp"
#include <iostream>
bool order::operator<(const order &x) const { return timestamp < x.timestamp; }
bool order::operator>(const order &x) const { return timestamp > x.timestamp; }
bool order::operator==(const order &x) const {
    return timestamp == x.timestamp;
}
bool order::operator!=(const order &x) const {
    return timestamp != x.timestamp;
}
bool order::operator<=(const order &x) const {
    return timestamp <= x.timestamp;
}
bool order::operator>=(const order &x) const {
    return timestamp >= x.timestamp;
}
ostream &operator<<(ostream &output, const order_detailed &od) {
    output << "[";
    if (od.status == STATUS::SUCCESS) {
        output << "success";
    }
    if (od.status == STATUS::PENDING) {
        output << "pending";
    }
    if (od.status == STATUS::REFUNDED) {
        output << "refunded";
    }
    output << "] ";
    output << od.trainID << " ";
    output << od.begin << " ";
    output << day_to_date(od.day + od.leaveTime / DAY_MINUTES) << " ";
    output << minute_to_time(od.leaveTime % DAY_MINUTES) << " ";
    output << "-> ";
    output << od.end << " ";
    output << day_to_date(od.day + od.arriveTime / DAY_MINUTES) << " ";
    output << minute_to_time(od.arriveTime % DAY_MINUTES) << " ";
    output << od.price << " ";
    output << od.num;
    return output;
}
BPT<pair<int, pair<int, int>>> *orderIndex;
BPT<pair<pair<int, int>, order>> *pendingOrder;
Filer<order_detailed> *orderData;