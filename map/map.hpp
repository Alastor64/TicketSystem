#pragma once

#include "exceptions.hpp"
#include "predef.hpp"

constexpr bool DEBUG = 1;
namespace sjtu {

template <class Key, class T, class Compare = std::less<Key>> class map {
  public:
    typedef pair<const Key, T> value_type;
    template <typename U> class base_iterator;
    typedef base_iterator<value_type> iterator;
    typedef base_iterator<const value_type> const_iterator;
    enum Color { RED, BLACK, DOUBLE };
    class Node;
    class Node {
      public:
        size_t chH() {
            size_t h[2];
            for (size_t i = 0; i < 2; i++)
                if (son[i])
                    h[i] = son[i]->chH();
                else
                    h[i] = 1;
            if (h[0] == -1 || h[1] == -1)
                return -1;
            if (h[0] != h[1])
                return -1;
            if (color == BLACK)
                return h[0] + 1;
            return h[0];
        }
        void write(Node *begin) {
            cout << this << ":";
            cout << "H=" << chH();
            if (color == RED)
                cout << "[ RED ]";
            else
                cout << "[BLACK]";
            cout << "(" << data.first << "," << data.second << ")  ";
            for (int i = 0; i < 2; i++)
                if (son[i])
                    cout << son[i] << "  ";
                else
                    cout << "NULL  ";
            cout << "\n";
            for (int i = 0; i < 2; i++)
                if (son[i])
                    son[i]->write(begin);
        }

      public:
        Node *son[2], *father;
        value_type data;
        Color color;
        Node(const value_type &_data, const Color &_color = RED)
            : data(_data), color(_color) {
            son[0] = son[1] = father = nullptr;
        }
        Node *getBrother() { return father->son[father->son[1] != this]; }
        Node *getUncle() { return father->getBrother(); }
        Node *&getSelfPointer() { return father->son[father->son[0] != this]; }
        void clear() { // clear all sons
            for (size_t i = 0; i < 2; i++)
                if (son[i]) {
                    son[i]->clear();
                    delete son[i];
                    son[i] = nullptr;
                }
        }
        Node *clone() {
            Node *tmp = new Node(data, color);
            for (size_t i = 0; i < 2; i++)
                if (son[i]) {
                    tmp->son[i] = son[i]->clone();
                    tmp->son[i]->father = tmp;
                } else
                    tmp->son[i] = nullptr;
            return std::move(tmp);
        }
        void rotate() {
            father->getSelfPointer() = this;
            size_t _ = father->son[1] != this;
            if (son[_]) {
                son[_]->father = father;
                getSelfPointer() = son[_];
                father = father->father;
                son[_] = son[_]->father;
                son[_]->father = this;
            } else {
                getSelfPointer() = nullptr;
                son[_] = father;
                father = father->father;
                son[_]->father = this;
            }
        }
        Node *extremum(const size_t &_) {
            Node *tmp = this;
            while (tmp->son[_])
                tmp = tmp->son[_];
            return std::move(tmp);
        }
    };
    void write() { Godfather->son[0]->write(Godfather->son[0]); }
    Color getNodeColor(Node *const &ptr) {
        if (ptr)
            return ptr->color;
        else
            return BLACK;
    }
    Node *Godfather; // virtual root && end()
    Node *Michael;   // min
    Node *Maximun;   // max
    size_t NodeNumber;
    map() {
        NodeNumber = 0;
        Godfather = (Node *)::operator new(sizeof(Node));
        Godfather->son[0] = Godfather->son[1] = Godfather->father = nullptr;
        Maximun = Michael = Godfather;
    }

    void copy(const map &A, map &B) { // B=A
        B.NodeNumber = A.NodeNumber;
        if (!A.Godfather->son[0])
            return;
        B.Godfather->son[0] = A.Godfather->son[0]->clone();
        B.Godfather->son[0]->father = B.Godfather;
    }
    void mcopy(map &&A, map &B) { // B=A
        B.NodeNumber = A.NodeNumber;
        if (!A.Godfather->son[0])
            return;
        A.NodeNumber = 0;
        B.Godfather->son[0] = A.Godfather->son[0];
        B.Godfather->son[0]->father = B.Godfather;
        A.Godfather->son[0] = nullptr;
    }

    map(const map &other) : map() {
        copy(other, *this);
        Michael = Godfather->extremum(0);
        if (Godfather->son[0])
            Maximun = Godfather->son[0]->extremum(1);
        else
            Maximun = Godfather;
    }
    map(map &&other) : map() {
        mcopy(std::move(other), *this);
        Michael = Godfather->extremum(0);
        if (Godfather->son[0])
            Maximun = Godfather->son[0]->extremum(1);
        else
            Maximun = Godfather;
    }
    map &operator=(const map &other) {
        if (this != &other) {
            clear();
            copy(other, *this);
            Michael = Godfather->extremum(0);
            if (Godfather->son[0])
                Maximun = Godfather->son[0]->extremum(1);
            else
                Maximun = Godfather;
        }
        return *this;
    }
    map &operator=(map &&other) {
        if (this != &other) {
            clear();
            mcopy(std::move(other), *this);
            Michael = Godfather->extremum(0);
            if (Godfather->son[0])
                Maximun = Godfather->son[0]->extremum(1);
            else
                Maximun = Godfather;
        }
        return *this;
    }

    void clear() {
        Godfather->clear();
        NodeNumber = 0;
        Maximun = Michael = Godfather;
    }

    ~map() {
        clear();
        ::operator delete(Godfather);
    }

    static bool isLeft(const Key &son, const Key &father) {
        static Compare CMP;
        return CMP(son, father);
    }
    static bool isRight(const Key &son, const Key &father) {
        static Compare CMP;
        return CMP(father, son);
    }
    static bool isEqual(const Key &son, const Key &father) {
        return !isLeft(son, father) && !isRight(son, father);
    }
    Node *_find(const Key &key) const { // return Godfather for not found
        Node *tmp = Godfather->son[0];
        while (tmp) {
            if (isEqual(key, tmp->data.first))
                return tmp;
            tmp = tmp->son[isRight(key, tmp->data.first)];
        }
        return Godfather;
    }
    Node *__find(const Key &key,
                 size_t &_) const { // return pos to ins for not found
        Node *tmp = Godfather;
        _ = 0;
        while (tmp->son[_]) {
            tmp = tmp->son[_];
            if (isEqual(key, tmp->data.first)) {
                _ = -1;
                return tmp;
            }
            _ = isRight(key, tmp->data.first);
        }
        return tmp;
    }
    T &at(const Key &key) {
        Node *tmp = _find(key);
        if (tmp != Godfather)
            return tmp->data.second;
        throw index_out_of_bound();
    }

    const T &at(const Key &key) const {
        Node *tmp = _find(key);
        if (tmp != Godfather)
            return tmp->data.second;
        throw index_out_of_bound();
    }

    T &operator[](const Key &key) {
        return (*insert(value_type(key, T())).first).second;
    }

    const T &operator[](const Key &key) const { return at(key); }

    iterator begin() { return iterator(Michael, this); }

    const_iterator cbegin() const { return const_iterator(Michael, this); }

    iterator end() { return iterator(Godfather, this); }

    const_iterator cend() const { return const_iterator(Godfather, this); }

    bool empty() const { return !NodeNumber; }

    size_t size() const { return NodeNumber; }

    void check(Node *ptr) {
        // write();
        while (1) {
            // cout << "pass\n";
            if (ptr->father == Godfather) {
                ptr->color = BLACK;
                return;
            }
            if (ptr->color != RED || ptr->father->color != RED) {
                return;
            }
            if (getNodeColor(ptr->getUncle()) == RED) {
                ptr->father->color = ptr->getUncle()->color = BLACK;
                ptr->father->father->color = RED;
                ptr = ptr->father->father;
                continue;
            }
            size_t _1 = ptr->father->son[0] != ptr,
                   _2 = ptr->father->father->son[0] != ptr->father;
            // ptr->father->color = BLACK;
            ptr->father->father->color = RED;
            if (_1 != _2) {
                ptr->father->color = RED;
                ptr->color = BLACK;
                ptr->rotate();
                ptr->rotate();
            } else {
                ptr->father->color = BLACK;
                ptr->father->rotate();
            }
            return;
        }
    }

    pair<iterator, bool> insert(const value_type &value) {
        size_t _;
        Node *tmp = __find(value.first, _);
        if (_ == -1)
            return pair<iterator, bool>(iterator(tmp, this), false);
        tmp->son[_] = new Node(value);
        tmp->son[_]->father = tmp;
        tmp = tmp->son[_];
        check(tmp);
        NodeNumber++;
        if (Michael == Godfather ||
            isLeft(tmp->data.first, Michael->data.first))
            Michael = tmp;
        if (Maximun == Godfather ||
            isRight(tmp->data.first, Maximun->data.first))
            Maximun = tmp;
        return pair<iterator, bool>(iterator(tmp, this), true);
    }

    template <typename U> void swap(U &A, U &B) {
        U tmp = B;
        B = A;
        A = tmp;
    }
    void erase(iterator pos) {
        if (pos.container != this)
            throw invalid_iterator();
        Node *tmp = pos.ptr, *tmp2;
        if (!tmp->father)
            throw invalid_iterator();
        NodeNumber--;
        if (Michael == tmp)
            Michael = (++iterator(Michael, this)).ptr;
        if (Maximun == tmp) {
            if (Michael != Godfather)
                Maximun = (--iterator(Maximun, this)).ptr;
            else
                Maximun = Godfather;
        }
        if (tmp->son[0] && tmp->son[1]) {
            tmp2 = tmp->son[1]->extremum(0);
            swap(tmp2->getSelfPointer(), tmp->getSelfPointer());
            swap(tmp2->father, tmp->father);
            for (int i = 0; i < 2; i++) {
                swap(tmp2->son[i], tmp->son[i]);
                if (tmp->son[i])
                    tmp->son[i]->father = tmp;
                if (tmp2->son[i])
                    tmp2->son[i]->father = tmp2;
            }
            swap(tmp2->color, tmp->color);
        }
        // write();
        // cout << "D:" << tmp->data.first << "\n";
        if (tmp->son[0] || tmp->son[1]) {
            size_t _ = bool(tmp->son[1]);
            tmp->son[_]->color = BLACK;
            tmp->son[_]->father = tmp->father;
            tmp->getSelfPointer() = tmp->son[_];
            delete tmp;
            return;
        }
        if (tmp->color == RED) {
            tmp->getSelfPointer() = nullptr;
            delete tmp;
            return;
        }
        tmp2 = tmp;
        while (tmp->father != Godfather) {
            if (tmp->color == RED) {
                tmp->color = BLACK;
                break;
            }
            Node *Bro = tmp->getBrother();
            if (Bro->color == BLACK) {
                if ((getNodeColor(Bro->son[0]) == BLACK &&
                     getNodeColor(Bro->son[1]) == BLACK)) {
                    Bro->color = RED;
                    tmp = tmp->father;
                    continue;
                }
                size_t _ = tmp->father->son[0] != tmp;
                if (getNodeColor(Bro->son[!_]) == RED) {
                    Bro->son[!_]->color = Bro->color;
                    Bro->color = tmp->father->color;
                    tmp->father->color = BLACK;
                    Bro->rotate();
                    break;
                }
                Bro->son[_]->color = tmp->father->color;
                tmp->father->color = BLACK;
                Bro->son[_]->rotate();
                Bro->father->rotate();
                break;
            } else {
                Bro->color = BLACK;
                tmp->father->color = RED;
                Bro->rotate();
                continue;
            }
        }
        tmp2->getSelfPointer() = nullptr;
        delete tmp2;
    }

    size_t count(const Key &key) const { return _find(key) != Godfather; }

    iterator find(const Key &key) { return iterator(_find(key), this); }

    const_iterator find(const Key &key) const {
        return const_iterator(_find(key), this);
    }

    template <typename U> class base_iterator {
      private:
        friend iterator;
        friend const_iterator;
        typedef map<Key, T, Compare> Container;
        friend Container;
        Node *ptr;
        const Container *container;
        base_iterator(Node *const &_ptr, const Container *const &_container) {
            ptr = _ptr;
            container = _container;
        }

      public:
        base_iterator() {
            ptr = nullptr;
            container = nullptr;
        }
        template <typename X> base_iterator(const base_iterator<X> &other) {
            ptr = other.ptr;
            container = other.container;
        }

        void walk(const size_t &_) {
            if (!ptr)
                throw invalid_iterator();
            if (!_ && !ptr->father) {
                ptr = container->Maximun;
                if (!ptr->father)
                    throw invalid_iterator();
                return;
            }
            if (ptr->son[_]) {
                ptr = ptr->son[_]->extremum(!_);
                return;
            }
            while (ptr->father) {
                if (ptr->father->son[_] != ptr) {
                    ptr = ptr->father;
                    return;
                }
                ptr = ptr->father;
            }
            ptr = ptr->father;
            throw invalid_iterator();
        }

        base_iterator operator++(int) {
            base_iterator tmp(ptr, container);
            this->operator++();
            return std::move(tmp);
        }

        base_iterator &operator++() {
            walk(1);
            return *this;
        }

        base_iterator operator--(int) {
            base_iterator tmp(ptr, container);
            this->operator--();
            return std::move(tmp);
        }

        base_iterator &operator--() {
            walk(0);
            return *this;
        }

        U &operator*() const {
            if (!ptr)
                throw invalid_iterator();
            if (ptr->father)
                return ptr->data;
            throw invalid_iterator();
        }

        template <typename X>
        bool operator==(const base_iterator<X> &rhs) const {
            return ptr == rhs.ptr;
        }

        template <typename X>
        bool operator!=(const base_iterator<X> &rhs) const {
            return ptr != rhs.ptr;
        }

        U *operator->() const noexcept {
            if (!ptr)
                throw invalid_iterator();
            if (ptr->father)
                return &ptr->data;
            else
                throw invalid_iterator();
        }
    };
};
} // namespace sjtu
using sjtu::map;