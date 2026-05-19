#pragma once
#include <fstream>
template <typename T> void fileWrite(std::fstream &fs, const T &x) {
    fs.write(reinterpret_cast<const char *>(&x), sizeof(x));
}
template <typename T> void fileRead(std::fstream &fs, T &x) {
    fs.read(reinterpret_cast<char *>(&x), sizeof(x));
}
template <typename T> class Filer {
  public:
    using fstream = std::fstream;
    using ios = std::ios;
    static constexpr int headPos = 2 * sizeof(int);
    static constexpr int TSize = sizeof(T);
    int last;
    int tail;
    std::fstream fs;
    void init(const char *s) {
        fs.open(s, ios::binary | ios::out);
        last = 0;
        tail = headPos;
        fileWrite(fs, tail);
        fileWrite(fs, last);
        fs.close();
        fs.open(s, ios::in | ios::out | ios::binary);
    }
    void load(const char *s) {
        fileRead(fs, tail);
        fileRead(fs, last);
    }
    Filer(const char *s) {
        fs.open(s, ios::out | ios::in | ios::binary);
        if (fs)
            load(s);
        else
            init(s);
    }
    int push(const T &x) {
        int tmp;
        if (last) {
            tmp = last;
            fs.seekg(tmp);
            fileRead(fs, last);
            fs.seekp(tmp);
        } else {
            tmp = tail;
            fs.seekp(tail);
            tail += TSize;
        }
        fileWrite(fs, x);
        return tmp;
    }
    void update(int index, const T &x) {
        fs.seekp(index);
        fileWrite(fs, x);
    }
    void read(int index, T &x) {
        fs.seekg(index);
        fileRead(fs, x);
    }
    void pop(int index) {
        fs.seekp(index);
        fileWrite(fs, last);
        last = index;
    }
    ~Filer() {
        fs.seekp(0);
        fileWrite(fs, tail);
        fileWrite(fs, last);
        fs.close();
    }
};