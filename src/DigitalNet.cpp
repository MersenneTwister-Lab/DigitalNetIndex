/**
 * @file DigitalNet.cpp
 *
 * @brief DigitalNet class for Quasi Monte-Carlo Method.
 *
 * @note Currently only 64-bit DigitalNet is implemented.
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
#include "digital.h"
//#include "powtwo.h"
#include "bit_operator.h"
#include "DigitalNet.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <stdlib.h>
#include <cstring>
#include <cstdio>
#include <cerrno>

using namespace std;

/*
 * Unnamed NameSpace for file scope things.
 */
namespace {
    using namespace DigitalNetNS;
    //const int N = 64;
    //const int S_MIN = 4;
    //const int S_MAX = 10;
    //const int M_MIN = 10;
    //const int M_MAX = 18;

    const string digital_net_path = "DIGITAL_NET_PATH";
    struct digital_net_name {
        std::string name;
        std::string abb;
        std::string construction;
    };

    const digital_net_name digital_net_name_data[] = {
        {"NX", "nx", "Niederreiter-Xing"},
        {"Sobol", "so", "Sobol"},
        {"Old_Sobol", "oldso", "Old Sobol"},
        {"NX_LowWAFOM", "nxlw", "NX+LowWAFOM, CV = (max(CV) + min(CV))/2"},
        {"Sobol_LowWAFOM", "solw", "Sobol+LowWAFOM, CV = (max(CV) + min(CV))/2"}
    };

    const uint32_t digital_net_name_data_size = 5;

    const char * getDataPath()
    {
        return getenv(digital_net_path.c_str());
    }


    template<typename U>
    int read_digital_net_data(std::istream& is, int n,
                              uint32_t s, uint32_t m,
                              U base[],
                              int * tvalue, double * wafom)
    {
        uint64_t data[s * m];
        uint64_t tmp;
        uint32_t i = 0;
        uint32_t j = 0;
        for (i = 0; i < m; i++) {
            for (j = 0; j < s; j++) {
                if (!is) {
                    cerr << "too less data i = " << dec << i
                         << " j = " << j
                         << " s = " << s << " m = " << m
                         << endl;
                    return -1;
                }
                is >> tmp;
                data[i * s + j] = tmp;
            }
        }
        if (i * s + j < s * m) {
            cerr << "too less data i = " << dec << i
                 << " s = " << s << " m = " << m
                 << endl;
            return -1;
        }
        if (is) {
            is >> *wafom;
        }
        if (is) {
            is >> *tvalue;
        }
        if (n == sizeof(U) * 8) {
            for (size_t i = 0; i < s * m; i++) {
                base[i] = static_cast<U>(data[i]);
            }
        } else if (n == 32) { // U is uint64_t
            for (size_t i = 0; i < s * m; i++) {
                base[i] = data[i] << 32;
            }
        } else { // n == 64 U is uint32_t
            for (size_t i = 0; i < s * m; i++) {
                base[i] = static_cast<U>((data[i] >> 32)
                                        & UINT32_C(0xffffffff));
            }
        }
        return 0;
    }

    template<typename U>
    int read_digital_net_data(digital_net_id id, uint32_t s, uint32_t m,
                              U base[],
                              int * tvalue, double * wafom) {
#if defined(DEBUG)
        cout << "in read_digital_net_data" << endl;
#endif
        string path;
        const char * cpath = getDataPath();
        if (cpath == NULL) {
            path = "../data";
        } else {
            path = cpath;
        }
        if (path.back() != '/') {
            path += "/";
        }
        //string name = digital_net_name_data[id].name;
        string name = digital_net_name_data[id].abb;
        path += name;
        path += ".dat";
#if defined(DEBUG)
        cout << "fname = " << path << endl;
#endif
        const char * mode = "rb";
        size_t count;
        errno = 0;
        FILE *fp = fopen(path.c_str(), mode);
        if (fp == NULL) {
            cerr << path.c_str() << ":" << strerror(errno) << endl;
            throw runtime_error("can't open");
        }
        uint64_t dmy;
        count = fread(&dmy, sizeof(uint64_t), 1, fp);
        if (count != 1) {
            cerr << "fail to read magic number" << endl;
            throw runtime_error("fail to read magic number");
        }
        if (dmy != DIGITAL_MAGIC) {
            cerr << "magic number mismatch" << endl;
            throw runtime_error("magic number mismatch");
        }
        digital_net_header_t header;
        for (;;) {
            count = fread(&header, sizeof(digital_net_header_t), 1, fp);
            if (count != 1) {
                cerr << "fail to read header s = "
                     << dec << s << " m = " << m << endl;
                throw runtime_error("can't read header");
            }
            if (header.s > s && header.m > m) {
                cerr << "header s = " << dec << header.s << " m = " << header.m
                     << endl;
                throw runtime_error("can't find s and m in header");
            }
            if (header.s == s && header.m == m) {
                break;
            }
        }
        fseek(fp, header.pos, SEEK_SET);
        size_t size = s * m * sizeof(uint64_t);
        digital_net_data_t dn;
        count = fread(&dn, sizeof(digital_net_data_t), 1, fp);
        if (count != 1) {
            cerr << "fail to read digital net data" << endl;
            throw runtime_error("fail to read digital net data");
        }
        uint64_t data[s * m];
        count = fread(data, size, 1, fp);
        if (count != 1) {
            cout << "fail to read digtal net array" << endl;
            throw runtime_error("fail to read digtal net array");
        }
#if defined(DEBUG)
        cout << "read_digital_net_data after read" << endl;
        cout << "data:" << endl;
        for (size_t i = 0; i < s; i++) {
            cout << data[i] << " ";
        }
        cout << endl;
#endif
        *wafom = dn.wafom;
        *tvalue = dn.tvalue;
        //uint64_t * data = dn.data;
        if (sizeof(U) * 8  == 32) {
            for (size_t i = 0; i < s * m; i++) {
                base[i] = static_cast<U>((data[i] >> 32)
                                         & UINT32_C(0xffffffff));
            }
        } else {
            for (size_t i = 0; i < s * m; i++) {
                base[i] = data[i];
            }
        }
#if defined(DEBUG)
        cout << "out read_digital_net_data" << endl;
        cout << "base:" << endl;
        for (size_t i = 0; i < s; i++) {
            cout << base[i] << " ";
        }
        cout << endl;
#endif
        return 0;
    }

}

namespace DigitalNetNS {
    const string getDigitalNetName(uint32_t index)
    {
        if (index < digital_net_name_data_size) {
            return digital_net_name_data[index].name;
        } else {
            return "";
        }
    }

    int readDigitalNetHeader(std::istream& is, int * n,
                             uint32_t * s, uint32_t * m)
    {
        is >> *n;
        is >> *s;
        is >> *m;
        if (is.good()) {
            return 0;
        } else {
            return -1;
        }
    }

/**
 * Constructor from input stream
 *
 * File Format:
 * separator: white space, blank char, tab char, cr, lf, etc.
 * the first item: bit size, integer fixed to 64, currently.
 * the second item: s, unsigned integer.
 * the third item: m, unsigned integer.
 * from fourth: s * m number of 64-bit unsigned integers.
 * the last but one: wafom double precision number, optional.
 * the last: t-value, integer, optional.
 * @param is input stream, from where digital net data are read.
 * @exception runtime_error, when can't read data from is.
 */

    int readDigitalNetData(std::istream& is, int n,
                           uint32_t s, uint32_t m,
                           uint64_t base[],
                           int * tvalue, double * wafom)
    {
        return read_digital_net_data(is, n, s, m, base, tvalue, wafom);
    }

    int readDigitalNetData(std::istream& is, int n,
                           uint32_t s, uint32_t m,
                           uint32_t base[],
                           int * tvalue, double * wafom)
    {
        return read_digital_net_data(is, n, s, m, base, tvalue, wafom);
    }

/**
 * Constructor from reserved data
 *
 * file are searched from environment variable DIGITAL_NET_PATH
 *
 * @param name name of digital net
 * @param s s value
 * @param m m value
 * @exception runtime_error, when can't read data from is.
 */
    int readDigitalNetData(digital_net_id id, uint32_t s, uint32_t m,
                           uint64_t base[],
                           int * tvalue, double * wafom)
    {
        return read_digital_net_data(id, s, m, base, tvalue, wafom);
    }

    int readDigitalNetData(digital_net_id id, uint32_t s, uint32_t m,
                           uint32_t base[],
                           int * tvalue, double * wafom)
    {
        return read_digital_net_data(id, s, m, base, tvalue, wafom);
    }

#if 0
    DigitalNet<uint64_t>::DigitalNet(const digital_net_id& id,
                                     uint32_t s,
                                     uint32_t m)
    {
    }



    DigitalNet<uint64_t>::~DigitalNet()
    {
        delete[] base;
        delete[] shift;
        if (point_base != NULL) {
            delete[] point_base;
        }
        if (point != NULL) {
            delete[] point;
        }
    }

    void DigitalNet<uint64_t>::showStatus(std::ostream& os)
    {
        os << "n = " << N << endl;
        os << "s = " << s << endl;
        os << "m = " << m << endl;
        for (uint32_t k = 0; k < m; ++k) {
            for (uint32_t i = 0; i < s; ++i) {
                os << "base[" << k << "][" << i << "] = "
                   << getBase(k, i) << ' ';
                os << ' ';
            }
            os << endl;
        }
        os << "WAFOM-value = " << wafom << endl;
        os << "t-value = " << tvalue << endl;
    }

    void DigitalNet<uint64_t>::pointInitialize()
    {
        if (point_base == NULL) {
            point_base = new uint64_t[s];
        }
        if (point == NULL) {
            point = new double[s];
        }
        for (uint32_t i = 0; i < s; ++i) {
            point_base[i] = getBase(0, i);
            shift[i] = mt();
        }
        gray.clear();
        count++;
        nextPoint();
    }

    void DigitalNet<uint64_t>::nextPoint()
    {
        if (count == 0 ) {
            pointInitialize();
            return;
        }
        static const int get_max = 64 - 53;
        static const double factor = exp2(-53);
        static const double eps = exp2(-64);
        int bit = gray.index();
        for (uint32_t i = 0; i < s; ++i) {
            point_base[i] ^= getBase(bit, i);
            // shift して1を立てている
            uint64_t tmp = (point_base[i] ^ shift[i]) >> get_max;
            point[i] = static_cast<double>(tmp) * factor + eps;
        }
        if (count == (1ULL << m)) {
            count = 0;
        } else {
            gray.next();
            count++;
        }
    }
#endif
#if 0
    void DigitalNet<uint64_t>::scramble()
    {
        uint64_t LowTriMat[N];
        uint64_t tmp;

        for (uint32_t i = 0; i < s; ++i) {
            for (uint32_t j = 0; j < N; ++j) {
                LowTriMat[j] = (mt() << (N - j - 1))
                    | powtwo(N - j - 1);
            }
            for (uint32_t k = 0; k < m; ++k) {
                tmp = INT64_C(0);
                for (int j = 0; j < N; ++j) {
                    int bit = innerProduct(LowTriMat[j], getBase(k, i));
                    if (bit == 1) {
                        tmp ^= powtwo(N - j - 1);
                    }
                }
                setBase(k, i, tmp);
            }
        }
    }
#endif
#if 0
/*
  指定されたi(0<=i<s)に対し, C_iのみにL_iをかける操作をする
  ただし, L_iは下三角行列かつ正則で, 指定されたj, l(n>j>l>=0)に対し
  第(j, l)成分(と対角成分)が1, その他が0の行列である.
  もう一度同じL_iをC_iにかけることで元のC_iに戻る.
  L_i(j, l)をC_iにかけると, C_iのj行にl行を足した(XOR)ものとなる.
*/
    void DigitalNet<uint64_t>::scramble(const int i, const int j, const int l)
    {
        for (uint32_t k = 0; k < m; ++k) {
            if (getBit(getBase(k, i), N - 1 - l) == 1) {
                setBase(k, i, getBase(k, i) ^ powtwo(N - 1 - j));
            }
        }
    }
#endif
#if 0
    void DigitalNet<uint64_t>::setSeed(uint64_t seed)
    {
        mt.seed(seed);
    }

    uint32_t DigitalNet<uint64_t>::getParameterSize()
    {
        return digital_net_name_data_size;
    }

    const string DigitalNet<uint64_t>::getDigitalNetConstruction(uint32_t index)
    {
        if (index < digital_net_name_data_size) {
            return digital_net_name_data[index].construction;
        } else {
            return "";
        }
    }

    uint32_t DigitalNet<uint64_t>::getSMax()
    {
        return S_MAX;
    }

    uint32_t DigitalNet<uint64_t>::getSMin()
    {
        return S_MIN;
    }

    uint32_t DigitalNet<uint64_t>::getMMax()
    {
        return M_MAX;
    }

    uint32_t DigitalNet<uint64_t>::getMMin()
    {
        return M_MIN;
    }
#endif
}
