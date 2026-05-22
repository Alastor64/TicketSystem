#pragma once
#include "Filer.hpp"
#include "predef.hpp"
constexpr int readSize = 500;
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
        // |*|*|*
        // * is a
        // | is son
      public:
        T a[degree + 1];
        // sorted increasingly
        // a[i] is the maximun element of son[i]
        int son[degree + 1];
        int size;
        int next; // for not leaf next=END for the most right leaf next=0
        node() {
            size = 0;
            next = END;
        }
        node(int _next) {
            next = _next;
            size = 0;
        }
        void add(const T &x, int _son = 0) {
            a[size] = x;
            son[size++] = _son;
        }
        void insa(const T &x, int index) {
            for (int i = size; i > index; i--) {
                a[i] = a[i - 1];
            }
            a[index] = x;
        }
        void inss(int _son, int index) {
            for (int i = size; i > index; i--) {
                son[i] = son[i - 1];
            }
            son[index] = _son;
        }
        void dela(int index) {
            for (int i = index; i < size - 1; i++) {
                a[i] = a[i + 1];
            }
        }
        void delson(int index) {
            for (int i = index; i < size - 1; i++) {
                son[i] = son[i + 1];
            }
        }
        void merge(node &b) {
            // add |*|*|*
            if (next != END) {
                next = b.next;
            }
            for (int i = 0; i < b.size; i++) {
                add(b.a[i], b.son[i]);
            }
        }
    } Gtmp;
    int split(int index, node &tmp, T &back) {
        // return new node index,back is the element to push up
        // remember check if next == END
        // for leaf remember to adjust next
        // this function do change data!
        node nw(END);
        back = tmp.a[tmp.size / 2 - 1];
        if (tmp.next != END) {
            nw.next = tmp.next;
        }
        for (int i = tmp.size / 2; i < tmp.size; i++) {
            nw.add(tmp.a[i], tmp.son[i]);
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
    ~BPT() { data.intUpdate(rootIndex); }
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
        return END;
    }
    void insert(const T &x) {
        if (rootIndex) {
            node tmp;
            data.read(rootIndex, tmp);
            // printf("%d\n", rootIndex);
            bool needUpdata = insert(x, rootIndex, tmp);
            if (tmp.size > degree) {
                T La;
                T Ra = tmp.a[tmp.size - 1];
                int L = rootIndex, R = split(rootIndex, tmp, La);
                node tmp2(END);
                tmp2.add(La, L);
                tmp2.add(Ra, R);
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
        for (int i = 0; i < val.size; i++) {
            if (x <= val.a[i] || i + 1 == val.size) {
                bool flag = 0;
                if (x > val.a[i]) {
                    if (val.next == END) {
                        val.a[i] = x;
                        flag = 1;
                    } else {
                        val.add(x);
                        return 1;
                    }
                }
                if (val.next != END) {
                    val.insa(x, i);
                    val.inss(0, i);
                    val.size++;
                    return 1;
                } else {
                    node tmp;
                    data.read(val.son[i], tmp);
                    bool needUpdata = insert(x, val.son[i], tmp);
                    if (tmp.size > degree) {
                        T A;
                        int R = split(val.son[i], tmp, A);
                        val.insa(A, i);
                        val.inss(R, i + 1);
                        val.size++;
                        return 1;
                    } else {
                        if (needUpdata) {
                            data.update(val.son[i], tmp);
                        }
                        return flag;
                    }
                }
            }
        }
        // will always return in for cycle
    }
    void del(const T &x) {
        if (rootIndex) {
            node tmp;
            data.read(rootIndex, tmp);
            bool needUpdata = del(x, tmp);
            // cout << needUpdata << endl;
            if (tmp.size < 2) {
                if (tmp.next == END) {
                    data.pop(rootIndex);
                    rootIndex = tmp.son[0];
                } else {
                    if (tmp.size < 1) {
                        data.pop(rootIndex);
                        rootIndex = 0;
                    } else {
                        data.update(rootIndex, tmp);
                    }
                }
            } else {
                if (needUpdata) {
                    data.update(rootIndex, tmp);
                }
            }
        }
    }
    bool del(const T &x, node &val) {
        for (int i = 0; i < val.size; i++) {
            if (x <= val.a[i]) {
                if (val.next != END) {
                    if (x == val.a[i]) {
                        val.dela(i);
                        val.size--;
                        return 1;
                    } else {
                        return 0;
                    }
                } else {
                    node tmp;
                    data.read(val.son[i], tmp);
                    bool needUpdata = del(x, tmp);
                    if (tmp.size * 2 < degree) {
                        bool isL; // needn't init
                        if (i) {
                            isL = 1;
                            data.read(val.son[i - 1], Gtmp);
                            if (Gtmp.size * 2 > degree + 1) {
                                tmp.insa(Gtmp.a[Gtmp.size - 1], 0);
                                tmp.inss(Gtmp.son[Gtmp.size - 1], 0);
                                tmp.size++;
                                Gtmp.size--;
                                val.a[i - 1] = Gtmp.a[Gtmp.size - 1];
                                data.update(val.son[i], tmp);
                                data.update(val.son[i - 1], Gtmp);
                                return 1;
                            }
                        }
                        if (i < val.size - 1) {
                            isL = 0;
                            data.read(val.son[i + 1], Gtmp);
                            if (Gtmp.size * 2 > degree + 1) {
                                tmp.add(Gtmp.a[0], Gtmp.son[0]);
                                Gtmp.dela(0);
                                Gtmp.delson(0);
                                Gtmp.size--;
                                val.a[i] = tmp.a[tmp.size - 1];
                                data.update(val.son[i], tmp);
                                data.update(val.son[i + 1], Gtmp);
                                return 1;
                            }
                        }
                        if (isL) {
                            Gtmp.merge(tmp);
                            data.update(val.son[i - 1], Gtmp);
                            data.pop(val.son[i]);
                            val.dela(i - 1);
                            val.delson(i);
                            val.size--;
                            return 1;
                        } else {
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
                            if (tmp.a[tmp.size - 1] != val.a[i]) {
                                val.a[i] = tmp.a[tmp.size - 1];
                                return 1;
                            }
                        }
                        return 0;
                    }
                }
            }
        }
        return 0;
    }
};
