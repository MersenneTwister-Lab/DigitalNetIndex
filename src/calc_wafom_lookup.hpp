#pragma once
#ifndef CALC_WAFOM_LOOKUP_HPP
#define CALC_WAFOM_LOOKUP_HPP

#include "DigitalNet.hpp"
#include <cmath>
#include <inttypes.h>
#include "kahan.hpp"

const int el = 16;

void make_table(int n, double table[4][65536], double d);

template<typename U>
double wafom_sub(double table[4][65536], const U point[], int s)
{
    double prod = 1.0;
    int maxidx;
    if (sizeof(U) * 8 == 64) {
        maxidx = 4;
    } else {
        maxidx = 2;
    }
    for (int i = 0; i < s; i++) {
        for (int j = 0; j < maxidx; j++) {
            int32_t e = (point[i] >> ((maxidx - 1 - j) * el)) & 0xffffU;
            prod *= table[j][e];
        }
    }
    return prod - 1.0;
}

template<typename U>
double calc_wafom(DigitalNetNS::DigitalNet<U>& dn, double table[4][65536]) {
    using namespace std;
    int m = dn.getM();
    int s = dn.getS();
    dn.pointInitialize();
    Kahan kahan;
    uint64_t num = UINT64_C(1) << m;
    for (uint64_t i = 0; i < num; i++) {
        const U * point = dn.getPointBase();
        double sub = wafom_sub(table, point, s);
        if (isnan(sub)) {
            cout << "sub is nan" << endl;
        }
        kahan.add(sub);
        if (isnan(kahan.get())) {
            cout << "kahan is nan i = " << i << endl;
            cout << "sub = " << sub << endl;
            break;
        }
        dn.nextPoint();
    }
    if (isnan(kahan.get())) {
        cout << "kahan is nan" << endl;
    }
    return kahan.get() / num;
}

#endif // CALC_WAFOM_LOOKUP_HPP
