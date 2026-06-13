#include "order.hpp"
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
BPT<pair<int, int>> *orderIndex;
BPT<pair<pair<int, int>, order>> *pendingOrder;
Filer<order_detailed> *orderData;