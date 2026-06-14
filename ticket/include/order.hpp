#pragma once
#include "BPT.hpp"
#include "Filer.hpp"
#include "train.hpp"
class order {
  public:
    int begin;
    int end;
    int num;
    int index;
    int timestamp;
    bool operator<(const order &x) const;
    bool operator>(const order &x) const;
    bool operator==(const order &x) const;
    bool operator!=(const order &x) const;
    bool operator<=(const order &x) const;
    bool operator>=(const order &x) const;
};
enum class STATUS {
    SUCCESS,
    PENDING,
    REFUNDED,
};
class order_detailed {
  public:
    STATUS status;
    TrainID trainID;
    int trainIndex;
    StationName begin, end;
    int beginPos, endPos;
    int day; // 发车日
    int leaveTime, arriveTime;
    int price;
    int num;
    int timestamp;
    friend ostream &operator<<(ostream &output, const order_detailed &od);
    static ostream &print(ostream &output, const order_detailed &od);
};
extern BPT<pair<int, pair<int, int>>> *orderIndex;
extern BPT<pair<pair<int, int>, order>> *pendingOrder;
extern Filer<order_detailed> *orderData;