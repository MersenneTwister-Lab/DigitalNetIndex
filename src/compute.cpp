/**
 * @file compute.cpp
 *
 * @brief compute WAFOM and t-value of Digital Net.
 *
 * @author Shinsuke Mori (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 * @author Mutsuo Saito
 *
 * Copyright (C) 2017 Shinsuke Mori, Makoto Matsumoto, Mutsuo Saito
 * and Hiroshima University.
 * All rights reserved.
 *
 * The GPL ver.3 is applied to this software, see
 * COPYING
 */
#include "config.h"
#include "compute.h"
#include "kahan.hpp"
#include "grayindex.hpp"
#include "matrix_rank.hpp"
#include "bit_operator.h"
#include "DigitalNet.h"
#include "combination.hpp"
#include <cmath>
#include <cstdlib>
#include <vector>

//#define DEBUG 1

using namespace std;

namespace {

    /** element length for look up table */
    const int element_length = 16;
    /** factor of WAFOM 1.0 or 2.0 */
    const double factor = 1.0;

#define LOOKUP_MAX 65536

    struct lookup_t {
        int n;
        double c;
        double data[4][LOOKUP_MAX];
    };

    const int c_min = -50;
    //const int c_max = 50;
    const int cv_min = 4;
    //const int cv_max = 32;

    void make_lookup(double c, int n, lookup_t& tyl);

    template<typename U>
    double lookup64(const U x[], int size, const lookup_t& table);

    double compute_WAFOM64(const DigitalNet<uint64_t>& dn,
                           const lookup_t& table);

}


double compute_WAFOM64(const DigitalNet<uint64_t>& dn,
                       enum wafom_kind kind,
                       double c)
{
    lookup_t tyl;
    string path;
    const char * cstr = DigitalNet<uint64_t>::getDataPath();
    if (cstr == NULL) {
        path = "../data";
    } else {
        path = cstr;
    }
    if (path.back() != '/') {
        path += "/";
    }
    int min;
    int index;
    if (kind == MEAN) {
        path += "meantyl.dat";
        min = cv_min;
        index = dn.getS();
    } else {
        path += "bigtyl.dat";
        min = c_min;
        index = lround(c * 10);
    }
#if defined(DEBUG)
    cout << "index = " << dec << index << endl;
    cout << "read tyl file:" << path << endl;
#endif
    int r = read_tyl(path, min, index, tyl);
    if (r != 0) {
        cerr << "read_tyl_error:" << endl;
        cerr << "path = " << path << endl;
        cerr << "min = " << dec << min << endl;
        cerr << "index = " << dec << index << endl;
        throw runtime_error("read_tyl error");
    }
    return compute_WAFOM64(dn, tyl);
}

template<typename U>
int64_t compute_tvalue64(const DigitalNet<U>& dn)
{
    using namespace std;

    uint32_t s = dn.getS();
    uint32_t m = dn.getM();
    uint32_t n = sizeof(U) * 8;
    uint32_t genMat[s][n];
    //transform_from_base_into_getMat();
    for (uint32_t i = 0; i < s; i++) {
        for (uint32_t j = 0; j < n; j++) {
            genMat[i][j] = 0;
        }
    }
    /*
      genMat[i][j]の左からk(0<=k<m)番目の要素(\in F_2)
      =
      base[k][i]の左からj(0<=j<n)番目の要素(\in F_2)
    */
    for (uint32_t k = 0; k < m; ++k) {
        for (uint32_t i = 0; i < s; ++i) {
            for (uint32_t j = 0; j < n; ++j) {
                if ( getBit(dn.getBase(k, i), n-1-j) == 1 ) {
                    //genMat[i][j] ^= powtwo(31-k);
                    genMat[i][j] ^= 1 << (31 - k);
                }
            }
        }
    }
    //transform_from_base_into_getMat();
    //const int r = s - 1;
    //vector<int> num;
    Combination index(s);
    uint64_t t = 0;
    for (; t <= m; ++t) {
        index.reset(m - t);
#if defined(DEBUG)
        cout << "t = " << dec << t << endl;
#endif
        vector<uint32_t> v;
        bool pass = true;
        for (;;) {
            v.clear();
#if defined(DEBUG)
            index.print(cout);
#endif
            for (uint32_t i = 0; i < s; ++i) {
                for (int j = 0; j < index[i]; ++j) {
                    v.push_back(genMat[i][j]);
                }
            }
            if ( matrix_rank(v) != static_cast<int32_t>(m-t) ) {
                pass = false;
                break;
            }
            bool hasNext = index.next();
            if (!hasNext) {
                break;
            }
        }
        if (pass) {
            return t;
        }
    }
    cerr << "error: tvalue is not calculated: t = " << dec << t << endl;
    return -1;
}

namespace {
    double compute_WAFOM64(const DigitalNet<uint64_t>& dn,
                           const lookup_t& tyl)
    {
        uint32_t s = dn.getS();
        uint32_t m = dn.getM();
        uint64_t maxcount = UINT64_C(1) << m;
        uint64_t b[s];
        Kahan sum;
        for (uint32_t i = 0; i < s; ++i) {
            b[i] = 0;
        }
        //sum.add(lookup_function_table(b) - 1.0);
        sum.add(lookup64(b, s, tyl) - 1.0);
        GrayIndex gray;
        for (uint64_t cnt = 1; cnt < maxcount; ++cnt) {
            //gray.next();
            int bit = gray.index();
            for (uint32_t i = 0; i < s; ++i) {
                b[i] ^= dn.getBase(bit, i);
            }
            gray.next();
            //sum.add(lookup_function_table(b) - 1.0);
            sum.add(lookup64(b, s, tyl) - 1.0);
        }
        return log2(sum.get()) - static_cast<double>(m);
    }

    int read_tyl(string& filename, int min, int c, lookup_t& tyl)
    {
        const char * mode = "rb";
        FILE *fp = fopen(filename.c_str(), mode);
        if (fp == NULL) {
            cout << "can't open file:" << filename << endl;
            return -1;
        }
        int index = c - min;
        fseek(fp, sizeof(lookup_t) * index, 0);
        size_t count = fread(&tyl, sizeof(lookup_t), 1, fp);
        if (count != 1) {
            cout << "count = " << dec << count << endl;
            return -1;
        } else {
            return 0;
        }
    }



}

namespace {
    /**
     * calculate (-1)^{ej}
     */
    inline int32_t m1_power(int32_t ej)
    {
        return -2 * ej + 1;
    }

    double calc_lookup(int32_t idx, int32_t e, double c)
    {
        double product = 1.0;
        for (int j = 1; j <= element_length; j++) {
            int32_t ej = static_cast<int32_t>((e >> (element_length - j)) & 1);
            //m2_exp *= 0.5; // j = 1
            double m2_exp = exp2(- factor * (idx * element_length + j + 1 - c));
            product *= 1.0 + m1_power(ej) * m2_exp;
        }
        return product;
    }

    void make_lookup(double c, int n, lookup_t& table)
    {
        table.n = n;
        table.c = c;
        int idx_max;
        if (n = 32) {
            idx_max = 2;
        } else {
            idx_max = 4;
        }
        for (int i = 0; i < idx_max; i++) {
            for (int j = 0; j < LOOKUP_MAX; j++) {
                table.table[i][j] = calc_lookup(i, j, c);
            }
        }
    }

    template<typename U>
    double lookup(const U x[], int size, const lookup_t& table)
    {
        int bitsize = sizeof(U) * 8;
        int maxindex = bitsize / element_length;
        double ret = 1.0;
        for (int i = 0; i < size; ++i) {
            for (int l = 0; l < maxindex; ++l) {
                //int idx = (x[i] >> (48 - 16 * l)) & 65535;
                int sh = (maxindex - l) * element_length
                int idx = (x[i] >> sh) & 0xffff;
                ret *= table.data[l][idx];
            }
        }
        return ret;
    }

    template <typename U>
    double wafom_sub(const lookup_t& lookup, const U point[], int s)
    {
        int maxindex = (sizeof(U) * 8) / element_length;
        double prod = 1.0;
        for (int i = 0; i < s; i++) {
            for (int j = 0; j < maxindex; j++) {
                int sh = (maxindex - j) * element_length
                int32_t e = (point[i] >> sh) & 0xffffU;
                prod *= lookup.table[j][e];
            }
        }
        return prod - 1.0;
    }

    template<typename U>
    double calc_wafom(DigitalNet<U>& dn, const lookup_t& table) {
        int m = dn.getM();
        int s = dn.getS();
        dn.pointInitialize();
        Kahan kahan;
        kahan.clear();
        uint64_t num = UINT64_C(1) << m;
        for (uint64_t i = 0; i < num; i++) {
            const U * point = dn.getPointBase();
            double sub = wafom_sub(table, point, s);
#if defined(DEBUG)
            if (isnan(sub)) {
                cout << "sub is nan" << endl;
            }
#endif
            kahan.add(sub);
#if defined(DEBUG)
            if (isnan(kahan.get())) {
                cout << "kahan is nan i = " << i << endl;
                cout << "sub = " << sub << endl;
                break;
            }
#endif
            dn.nextPoint();
        }
        if (isnan(kahan.get())) {
            cout << "kahan is nan" << endl;
        }
        return kahan.get() / num;
    }

}
