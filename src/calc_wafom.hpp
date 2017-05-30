#pragma once
#ifndef CALC_WAFOM_HPP
#define CALC_WAFOM_HPP

#include <cstdint>
#include <cmath>
#include "DigitalNet.hpp"
#include "kahan.hpp"

const double factor = 1.0;
//const double factor = 2.0;

inline int m1p(int x) {
    return 1 - 2 * x;
}

template<typename U>
double calc_wafom_sub(double c, int s, const U point[])
{
    int N = sizeof(U) * 8;
    double prod = 1.0;
#if defined(DEBUG)
    using namespace std;
    for (int i = 0; i < s; i++) {
        cout << point[i] << " ";
    }
#endif
    for (int i = 0; i < s; i++) {
        uint64_t w = point[i];
        for (int j = 0; j < N; j++) {
            int bij = (w >> (N - j - 1)) & 1;
            double p = 1.0 + m1p(bij) * exp2(-factor * (j + 2 - c));
#if defined(DEBUG)
            if (p <= 0) {
                cout << "p <= 0" << p << endl;
            }
#endif
            prod *= p;
#if defined(DEBUG)
            if (isinf(prod)) {
                cout << "prod is " << prod << endl;
                cout << "i = " << i << endl;
                cout << "j = " << j << endl;
                cout << "m1p(bij) = " << m1p(bij) << endl;
                cout << "pow(2.0, j + 2 - c) = " << exp2(-factor * (j + 2 - c))
                     << endl;
                break;
            }
#endif
        }
    }
#if defined(DEBUG)
    cout << prod << endl;
    if (isnan(prod)) {
        cout << "prod is nan" << endl;
    }
    if (isinf(prod)) {
        cout << "prod is " << prod << endl;
    }
#endif
    return prod - 1.0;
}

template<typename U>
double calc_wafom(DigitalNetNS::DigitalNet<U>& dn, double c) {
    using namespace std;
    int m = dn.getM();
    int s = dn.getS();
    dn.pointInitialize();
    //dn.nextPoint();
    Kahan kahan;
    kahan.clear();
    uint64_t num = UINT64_C(1) << m;
    for (uint64_t i = 0; i < num; i++) {
        const U * point = dn.getPointBase();
        double sub = calc_wafom_sub(c, s, point);
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
#endif // CALC_WAFOM_HPP
