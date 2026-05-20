#pragma once
#include "Filer.hpp"
constexpr int readSize = 1000000;
template <typename T> constexpr int getDegree() {
    constexpr int tmp = readSize / sizeof(T);
    if constexpr (!(tmp & 1)) {
        return tmp - 1;
    } else {
        return tmp;
    }
}
template <typename T, int degree = getDegree<T>()> class BPT {
  public:
    static constexpr int END = -1;
    class node {
      public:
        T a[degree - 1];
        // sorted increasingly
        // a[i] is the maximun element of son[i]
        int son[degree];
        int size;
        int next; // for not leaf next=END for the most right leaf next=0
        node() = default;
        node(int _next) {
            next = _next;
            size = 0;
        }
    } Gtmp;

  public:
    Filer<node> data;
    int rootIndex;
    BPT(const char *s) : data(s) {
        // note: rootIndex=0 for empty
        rootIndex = data.intRead();
    }
    int lower_bound(const T &x, int index = 0) {
        if (!index) {
            if (!rootIndex) {
                return END;
            } else {
                return lower_bound(x, rootIndex);
            }
        }
        data.read(index, Gtmp);
        for (int i = 0; i < Gtmp.size; i++) {
            if (x <= Gtmp.a[i]) {
                if (Gtmp.next == END) {
                    return lower_bound(x, Gtmp.son[i]);
                } else {
                    return i;
                }
            }
        }
        if (Gtmp.next == END) {
            return lower_bound(x, Gtmp.son[Gtmp.size]);
        } else {
            return END;
        }
    }
};
