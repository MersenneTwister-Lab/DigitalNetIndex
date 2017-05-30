#pragma once
#ifndef CALC_TVALUE_HPP
#define CALC_TVALUE_HPP

#include "DigitalNet.hpp"
#include "matrix_rank.hpp"
#include "combination.hpp"

template<typename U>
int64_t calc_tvalue(const DigitalNetNS::DigitalNet<U>& dn)
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

#endif // CALC_TVALUE_HPP
