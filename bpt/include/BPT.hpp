#pragma once
#include "Filer.hpp"
#include "predef.hpp"
#include <cstdio>
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
        T a[degree];
        // sorted increasingly
        // a[i] is the maximun element of son[i]
        int son[degree + 1];
        int size;
        int next; // for not leaf next=END for the most right leaf next=0
        node() {}
        node(int _next) {
            next = _next;
            size = 0;
        }
        void add(const T &x, int _son = 0) {
            a[size++] = x;
            son[size] = _son;
        }
        void ins(const T &x, int index, int _son = 0) {
            // |*|*|
            // we ins *|
            for (int i = size; i > index; i--) {
                a[i] = a[i - 1];
                son[i + 1] = son[i];
            }
            size++;
            a[index] = x;
            son[index + 1] = _son;
        }
        void push_front(const T &x, int _son) {
            for (int i = size; i > 0; i--) {
                a[i] = a[i - 1];
            }
            for (int i = size + 1; i > 0; i--) {
                son[i] = son[i - 1];
            }
            a[0] = x;
            son[0] = _son;
            size++;
        }
        void dela(int index) {
            for (int i = index; i < size - 1; i++) {
                a[i] = a[i + 1];
            }
        }
        void delson(int index) {
            for (int i = index; i < size; i++) {
                son[i] = son[i + 1];
            }
        }
        void merge(node &b) {
            // this need to be leaf or something like(|*|*|*,size=3)
            // add |*|*|*|
            son[size] = b.son[0];
            for (int i = 0; i < b.size; i++)
                add(b.a[i], b.son[i + 1]);
        }
    } Gtmp;
    int split(int index, node &tmp, T &back) {
        // return new node index,back is the element to push up
        // remember check if next == END
        // for leaf remember to adjust next
        // this function do change data!
        node nw(END);
        if (tmp.next != END) {
            back = tmp.a[tmp.size / 2 - 1];
            nw.next = tmp.next;
        } else {
            back = tmp.a[tmp.size / 2];
        }
        nw.son[0] = tmp.son[tmp.size / 2 + 1];
        for (int i = (tmp.next != END ? tmp.size / 2 : tmp.size / 2 + 1);
             i < tmp.size; i++) {
            nw.add(tmp.a[i], tmp.son[i + 1]);
        }
        tmp.size /= 2;
        int R = data.push(nw);
        if (tmp.next != END) {
            tmp.next = R;
        }
        data.update(index, tmp);
        return R;
    }

  public:
    Filer<node, 1> data;
    int rootIndex;
    BPT(const char *s) : data(s) {
        // note: rootIndex=0 for empty
        rootIndex = data.intRead();
    }
    ~BPT() { data.intUpdata(rootIndex); }
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
    void insert(const T &x) {
        if (rootIndex) {
            node tmp;
            data.read(rootIndex, tmp);
            // printf("%d\n", rootIndex);
            bool needUpdata = insert(x, rootIndex, tmp);
            if (tmp.size >= degree) {
                T md;
                int L = rootIndex, R = split(rootIndex, tmp, md);
                node tmp2(END);
                tmp2.add(md);
                tmp2.son[0] = L;
                tmp2.son[1] = R;
                rootIndex = data.push(tmp2);
            } else {
                if (needUpdata) {
                    data.update(rootIndex, tmp);
                }
            }
        } else {
            node tmp(0);
            tmp.add(x);
            rootIndex = data.push(tmp);
        }
    }
    bool insert(const T &x, int index, node &val) {
        for (int i = 0; i <= val.size; i++) {
            if (i == val.size || x <= val.a[i]) {
                if (val.next != END) {
                    val.ins(x, i);
                    return 1;
                } else {
                    node tmp;
                    data.read(val.son[i], tmp);
                    bool needUpdata = insert(x, val.son[i], tmp);
                    if (tmp.size >= degree) {
                        T A;
                        int R = split(val.son[i], tmp, A);
                        val.ins(A, i, R);
                        return 1;
                    } else {
                        if (needUpdata) {
                            data.update(val.son[i], tmp);
                        }
                        return 0;
                    }
                }
            }
        }
    }
    void del(const T &x) {
        if (rootIndex) {
            node tmp;
            data.read(rootIndex, tmp);
            bool needUpdata = del(x, rootIndex, tmp);
            // cout << needUpdata << endl;
            if (!tmp.size) {
                data.pop(rootIndex);
                if (tmp.next != END) {
                    rootIndex = 0;
                } else {
                    rootIndex = tmp.son[0];
                }
            } else {
                if (needUpdata) {
                    data.update(rootIndex, tmp);
                }
            }
        }
    }
    bool del(const T &x, int index, node &val) {
        for (int i = 0; i <= val.size; i++) {
            if (i == val.size || x <= val.a[i]) {
                if (val.next != END) {
                    if (i < val.size && x == val.a[i]) {
                        val.dela(i);
                        val.size--;
                        return 1;
                    } else {
                        return 0;
                    }
                } else {
                    node tmp;
                    data.read(val.son[i], tmp);
                    bool needUpdata = del(x, val.son[i], tmp);
                    if (tmp.size * 2 < degree - 1) {
                        bool isL;
                        if (i) {
                            isL = 1;
                            data.read(val.son[i - 1], Gtmp);
                            if (Gtmp.size * 2 > degree) {
                                if (tmp.next == END) {
                                    tmp.push_front(val.a[i - 1],
                                                   Gtmp.son[Gtmp.size]);
                                    val.a[i - 1] = Gtmp.a[Gtmp.size - 1];
                                } else {
                                    tmp.push_front(Gtmp.a[Gtmp.size - 1], 0);
                                    val.a[i - 1] = Gtmp.a[Gtmp.size - 2];
                                }
                                Gtmp.size--;
                                data.update(val.son[i], tmp);
                                data.update(val.son[i - 1], Gtmp);
                                return 1;
                            }
                        }
                        if (i < val.size) {
                            isL = 0;
                            data.read(val.son[i + 1], Gtmp);
                            if (Gtmp.size * 2 > degree) {
                                if (tmp.next == END) {
                                    tmp.ins(val.a[i], tmp.size, Gtmp.son[0]);
                                    val.a[i] = Gtmp.a[0];
                                } else {
                                    tmp.ins(Gtmp.a[0], tmp.size, 0);
                                    val.a[i] = tmp.a[tmp.size - 1];
                                }
                                Gtmp.dela(0);
                                Gtmp.delson(0);
                                Gtmp.size--;
                                data.update(val.son[i], tmp);
                                data.update(val.son[i + 1], Gtmp);
                                return 1;
                            }
                        }
                        if (isL) {
                            if (tmp.next == END) {
                                Gtmp.add(val.a[i - 1]);
                            } else {
                                Gtmp.next = tmp.next;
                            }
                            Gtmp.merge(tmp);
                            data.update(val.son[i - 1], Gtmp);
                            data.pop(val.son[i]);
                            val.dela(i - 1);
                            val.delson(i);
                            val.size--;
                            return 1;
                        } else {
                            if (tmp.next == END) {
                                tmp.add(val.a[i]);
                            } else {
                                tmp.next = Gtmp.next;
                            }
                            tmp.merge(Gtmp);
                            data.update(val.son[i], tmp);
                            data.pop(val.son[i + 1]);
                            val.dela(i);
                            val.delson(i + 1);
                            val.size--;
                            return 1;
                        }
                    } else {
                        if (needUpdata) {
                            data.update(val.son[i], tmp);
                        }
                        return 0;
                    }
                }
            }
        }
    }
};
