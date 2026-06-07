#pragma once
#include <fstream>
#include <string>
using std::string;
template <typename T> void fileWrite(std::fstream &fs, const T &x) {
    fs.write(reinterpret_cast<const char *>(&x), sizeof(x));
}
template <typename T> void fileRead(std::fstream &fs, T &x) {
    fs.read(reinterpret_cast<char *>(&x), sizeof(x));
}
template <typename T, int headSize = 0> class Filer {
  public:
    using fstream = std::fstream;
    using ios = std::ios;
    static constexpr int headPos = 2 * sizeof(int);
    static constexpr int dataPos = headPos + headSize * sizeof(int);
    static constexpr int TSize = sizeof(T);
    int last;
    int tail;
    std::fstream fs;
    void init(const string &s) {
        fs.open(s, ios::binary | ios::out);
        last = 0;
        tail = dataPos;
        fileWrite(fs, tail);
        fileWrite(fs, last);
        int tmp = 0;
        for (int i = 0; i < headSize; i++) {
            fileWrite(fs, tmp);
        }
        fs.close();
        fs.open(s, ios::in | ios::out | ios::binary);
    }
    bool blank() { return tail == dataPos; }
    void load(const string &s) {
        fileRead(fs, tail);
        fileRead(fs, last);
    }
    Filer(string s) {
        s = s + ".data";
        fs.open(s, ios::out | ios::in | ios::binary);
        if (fs) {
            load(s);
        } else {
            init(s);
        }
    }
    void intUpdate(int x, int i = 0) {
        fs.seekp(headPos + i * sizeof(int));
        fileWrite(fs, x);
    }
    int intRead(int i = 0) {
        fs.seekg(headPos + i * sizeof(int));
        int tmp;
        fileRead(fs, tmp);
        return tmp;
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