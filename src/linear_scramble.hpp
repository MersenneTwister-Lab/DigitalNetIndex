#pragma once
#ifndef LINEAR_SCRAMBLE_HPP
#define LINEAR_SCRAMBLE_HPP
#include "DigitalNet.hpp"
#include "calc_wafom_lookup.hpp"
#include "calc_tvalue.hpp"
#include "cvmean.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>

//#define DEBUG 1

#if defined(DEBUG)
#include <iostream>
#include <iomanip>
#endif

/**
 * トップの何個かを保存する
 */
template<typename U>
class Bests {
public:
    Bests(size_t size, size_t s, size_t m) {
        this->size = size;
        save = new U*[size];
        value = new double[size];
        worst = 100;
        for (size_t i = 0; i < size; i++) {
            save[i] = new U[s * m];
            value[i] = worst;
        }
    }
    ~Bests() {
        delete[] value;
        for (size_t i = 0; i < size; i++) {
            delete[] save[i];
        }
        delete[] save;
    }

    void put(double wafom, const DigitalNetNS::DigitalNet<U>& dn) {
        if (wafom > worst) {
            return;
        }
        for (size_t i = 0; i < size; i++) {
            if (value[i] == worst) {
                value[i] = wafom;
                dn.saveBase(save[i], dn.getS() * dn.getM());
                break;
            }
        }
        worst = value[0];
        for (size_t i = 0; i < size; i++) {
            if (value[i] > worst) {
                worst = value[i];
            }
        }
    }

    size_t getSize() const {
        return size;
    }

    void get(size_t index, DigitalNetNS::DigitalNet<U>& dn) const {
        dn.restoreBase(save[index], dn.getS() * dn.getM());
    }
private:
    U ** save;
    size_t size;
    double * value;
    double worst;
};

/**
 * table は計算済みとする
 *
 */
template<typename U>
double random_linear_scramble(DigitalNetNS::DigitalNet<U>& dn,
                              Bests<U>& bests,
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
        bests.put(w, dn);
        if (w < wafom) {
#if defined(DEBUG)
            cout << fixed << setprecision(18);
            std::cout << "wafom change " << wafom << " -> "
                      << w << std::endl;
            cout << "tvalue:" << t << endl;
#endif
            count = 0;
            //dn.saveBase(save_base, s * m);
            wafom = w;
        }
    }
    //dn.restoreBase(save_base, s * m);
    return wafom;
}

template<typename U>
double hill_climb_linear_scramble(DigitalNetNS::DigitalNet<U>& dn,
                                  const lookup_t& table)
{
    using namespace std;
    double wafom = calc_wafom(dn, table);
#if defined(DEBUG)
    int t = calc_tvalue(dn);
#endif
    const int nmax = 20;

    int s = dn.getS();
    for (;;) {
        bool change = false;
        for (int i = 0; i < s; i++) {
            for (int j = 1; j < nmax - 1; j++) {
                for (int k = 0; k < j; k++) {
                    dn.hc_scramble(i, j, k);
                    dn.pointInitialize();
                    double w = calc_wafom(dn, table);
#if defined(DEBUG)
                    int tt = calc_tvalue(dn);
                    if (tt != t) {
                        cout << "error t = " << t << " tt = " << tt << endl;
                        cout << "i = " << i << endl;
                        cout << "j = " << j << endl;
                        cout << "k = " << k << endl;
                    }
#endif
                    if (w < wafom) {
                        wafom = w;
                        change = true;
                    } else {
                        // 同じことをもう一度やって戻す
                        dn.hc_scramble(i, j, k);
                    }
                }
            }
        }
        if (!change) {
            break;
        }
    }
    return wafom;
}

template<typename U>
void linear_scramble(DigitalNetNS::digital_net_id dnid, size_t s, size_t m,
                     U seed, int repeat, lookup_t& table)
{
    using namespace DigitalNetNS;
    using namespace std;

    //int repeat = 200;
    DigitalNet<U> dn(dnid, s, m);
    dn.setSeed(seed);
    Bests<U> bests(10, s, m);
    //print(cout, dn, false);
    double wafom = random_linear_scramble(dn, bests, repeat, table);
    U save[s * m];
    for (size_t i = 0; i < bests.getSize(); i++) {
        bests.get(i, dn);
        dn.pointInitialize();
        double w = hill_climb_linear_scramble(dn, table);
        if (w < wafom) {
            wafom = w;
            dn.saveBase(save, s * m);
        }
    }
    dn.restoreBase(save, s * m);
    dn.pointInitialize();
    print(cout, dn, false, ' ');
    cout << dec << scientific << setprecision(18);
    cout << log2(wafom) << " ";
    cout << calc_tvalue(dn) << endl;
}

#endif // LINEAR_SCRAMBLE_HPP
