#pragma once
#ifndef LINEAR_SCRAMBLE_HPP
#define LINEAR_SCRAMBLE_HPP
#include "DigitalNet.hpp"
#include "calc_wafom_lookup.hpp"
#include "calc_tvalue.hpp"
#include "cvmean.h"
#include <cstdlib>

//#define DEBUG 1

#if defined(DEBUG)
#include <iostream>
#include <iomanip>
#endif

/**
 * table は計算済みとする
 *
 */
template<typename U>
double random_linear_scramble(DigitalNetNS::DigitalNet<U>& dn,
                              size_t repeat,
                              const lookup_t& table)
{
    using namespace std;
    double wafom = calc_wafom(dn, table);
    size_t s = dn.getS();
    size_t m = dn.getM();
    size_t count = 0;
    U save_base[s * m];
    dn.saveBase(save_base, s * m);
    while (count < repeat) {
        count++;
        dn.scramble();
        dn.pointInitialize();
        double w =calc_wafom(dn, table);
#if defined(DEBUG)
        int t = calc_tvalue(dn);
#endif
        if (w < wafom) {
#if defined(DEBUG)
            cout << fixed << setprecision(18);
            std::cout << "wafom change " << wafom << " -> "
                      << w << std::endl;
            cout << "tvalue:" << t << endl;
#endif
            count = 0;
            dn.saveBase(save_base, s * m);
            wafom = w;
        }
    }
    dn.restoreBase(save_base, s * m);
    return wafom;
}

template<typename U>
double hill_climb_linear_scramble(DigitalNetNS::DigitalNet<U>& dn,
                                  const lookup_t& table)
{
    double wafom = calc_wafom(dn, table);
    const int nmax = 20;
    int s = dn.getS();
    for (int i = 0; i < s; i++) {
        for (int j = 0; j < nmax - 1; j++) {
            for (int k = j + 1; k < nmax; k++) {
                dn.hc_scramble(i, j, k);
                dn.pointInitialize();
                double w = calc_wafom(dn, table);
                if (w < wafom) {
                    wafom = w;
                } else {
                    // 同じことをもう一度やって戻す
                    dn.hc_scramble(i, j, k);
                }
            }
        }
    }
    return wafom;
}
#endif // LINEAR_SCRAMBLE_HPP
