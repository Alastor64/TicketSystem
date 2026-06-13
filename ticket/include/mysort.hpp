#pragma once
template <typename T> void swap(T &a, T &b) {
    static T c;
    c = b;
    b = a;
    a = c;
}
template <typename T>
void sort(int l, int r, T a[], T b[], bool (*cmp)(const T &, const T &)) {
    if (l >= r) {
        return;
    }
    int md = (l + r) >> 1;
    sort(l, md, a, b, cmp);
    sort(md + 1, r, a, b, cmp);
    int i = l, k = l, j = md + 1;
    while (i <= md && j <= r) {
        if (cmp(a[i], a[j])) {
            b[k++] = a[i++];
        } else {
            b[k++] = a[j++];
        }
    }
    while (i <= md) {
        b[k++] = a[i++];
    }
    while (j <= r) {
        b[k++] = a[j++];
    }
    for (i = l; i <= r; i++) {
        a[i] = b[i];
    }
}