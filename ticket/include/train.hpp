#pragma once
#include "BPT.hpp"
#include "Filer.hpp"
#include "arraystring.hpp"
#include "command.hpp"
#include "predef.hpp"
#include <type_traits>
typedef arraystring<21> TrainID;
typedef arraystring<31> StationName;
class Train {
  public:
    TrainID trainID;
    StationName stations[100];
    char type;
    int stationNum;
    int seatNum;
    int prices[100];
    int leaveTime[100];
    int arriveTime[100];
    int beginDay;
    int endDay;
    Train();
    void load(Command &c, int d1, int d2);
};
template <typename T> class is_arraystring {
  public:
    constexpr static bool value = false;
};
template <int maxlen> class is_arraystring<arraystring<maxlen>> {
  public:
    constexpr static bool value = true;
};
template <typename T> class is_integral_pointer {
  public:
    constexpr static bool value = false;
};
template <typename T> class is_integral_pointer<T *> {
  public:
    constexpr static bool value = std::is_integral_v<T>;
};
template <typename T> void split(T &a, const string &s) {
    if (s == "_") {
        return;
    }
    int i = 0;
    int j = s.find_first_of('|');
    for (int I = 0; i < s.length(); I++) {
        if constexpr (is_integral_pointer<T>::value) {
            a[I] = stoi(s.substr(i, j - i));
        }
        if constexpr (is_arraystring<T>::value) {
            a[I] = s.substr(i, j - i);
        }
        i = j + 1;
        j = s.find_first_of('|', i);
    }
}
int date_to_day(const string &s);
string day_to_date(int day);
int time_to_minute(string &s);
string minute_to_time(int m);
void add_train(Command &c);
void delete_train(Command &c);
void release_train(Command &c);
void query_train(Command &c);
extern Filer<Train> *trainData;
extern BPT<pair<TrainID, int>> *trainIndex;
extern BPT<int> *releasedTrainIndex;
extern Filer<int[100]> *seatData;
extern BPT<pair<pair<int, int>, int>> *seatIndex;
extern BPT<pair<int, int>> *releasedSeatNum;