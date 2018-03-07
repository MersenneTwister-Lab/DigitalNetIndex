#pragma once
#ifndef DIFF_WAFOM_HPP
#define DIFF_WAFOM_HPP

#include "DigitalNet.hpp"
#include "calc_wafom_lookup.hpp"

namespace DigitalNetNS {

    template <typename U>
    class DiffWAFOM {
    public:
        DiffWAFOM(int s, int m) {
            this->s = s;
            this->m = m;
            point_num = 1 << m;
            sprod = new double[point_num];
            if (sizeof(U) * 8 == 64) {
                maxidx = 4;
            } else {
                maxidx = 2;
            }
        }
        ~DiffWAFOM() {
            delete[] sprod;
        }
        void calcSprod(const lookup_t& table, DigitalNet<U>& dn, int sindex) {
            dn.pointInitialize();
            for (int i = 0; i < point_num; i++) {
                const U * point = dn.getPointBase();
                sprod[i] = sprod_except(table, point, sindex);
                dn.nextPoint();
            }
        }
        double calcWafom(DigitalNet<U>& dn, const lookup_t& table, int sidx) {
            dn.pointInitialize();
            Kahan kahan;
            for (int i = 0; i < point_num; i++) {
                const U * point = dn.getPointBase();
                double sub = wafom_sprod(table, point, i, sidx);
                kahan.add(sub);
                dn.nextPoint();
            }
            return kahan.get() / point_num;
        }
    private:
        int s;
        int m;
        int point_num;
        int maxidx;
        double *sprod;
        double sprod_except(const lookup_t& table, const U point[], int sidx) {
            double prod = 1.0;
            for (int i = 0; i < s; i++) {
                if (i == sidx) {
                    continue;
                }
                for (int j = 0; j < maxidx; j++) {
                    int32_t e = (point[i] >> ((maxidx - 1 - j) * el)) & 0xffffU;
                    prod *= table.table[j][e];
                }
            }
            //return prod - 1.0;
            return prod;
        }
        double wafom_sprod(const lookup_t& table, const U point[],
                           int pidx, int sidx) {
            double prod = sprod[pidx];
            for (int j = 0; j < maxidx; j++) {
                int32_t e = (point[sidx] >> ((maxidx - 1 - j) * el)) & 0xffffU;
                prod *= table.table[j][e];
            }
            return prod - 1.0;
        }

    };

    template<typename U>
    double calc_wafom_diff(DigitalNet<U>& dn, const lookup_t& table) {
        DiffWAFOM<U> dw(dn.getS(), dn.getM());
        dw.calcSprod(table, dn, 0);
        return dw.calcWafom(dn, table, 0);
    }

} // end of namespace

#endif // DIFF_WAFOM_HPP
