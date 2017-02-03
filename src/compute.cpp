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
#include "gray.hpp"
#include "powtwo.h"
#include "matrix_rank.hpp"
#include "bit_operator.h"
#include "DigitalNet.h"
#include <cmath>
#include <cstdlib>
#include <vector>

//#define DEBUG 1

using namespace std;

namespace {

#define TYL_MAX 65536

    struct TYLSONIA_T {
        double data[4][TYL_MAX];
    };
    typedef struct TYLSONIA_T tylsonia_t;

    const int c_min = -50;
    const int c_max = 50;
    const int cv_min = 4;
    const int cv_max = 32;

    int read_tyl(string& filename,
                 int min,
                 int c,
                 tylsonia_t& tyl);

    double tylsonia64(const uint64_t x[],
                      int size,
                      const tylsonia_t& table);

    double compute_WAFOM64(const DigitalNet<uint64_t>& dn,
                           const tylsonia_t& tyl);

    class Combination {
    public:
        Combination(int size);
        ~Combination();
        void reset(int init);
        bool next();
        // no range check
        int operator[](int p) const {
            return data[p];
        }
        void print(std::ostream& os);
    private:
        int size;
        int init;
        int * data;
    };
}


double compute_WAFOM64(const DigitalNet<uint64_t>& dn,
                       enum wafom_kind kind,
                       double c)
{
    tylsonia_t tyl;
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

double compute_WAFOM32(const DigitalNet<uint32_t>&,
                       enum wafom_kind,
                       double)
{
    return 0;
}

int64_t compute_tvalue64(const DigitalNet<uint64_t>& dn)
{
    using namespace std;

    uint32_t s = dn.getS();
    uint32_t m = dn.getM();
    uint32_t n = 64; //sizeof(T) * 8;
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
                    genMat[i][j] ^= powtwo(31-k);
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

int64_t compute_tvalue32(const DigitalNet<uint32_t>&)
{
    return -1;
}

namespace {
    double compute_WAFOM64(const DigitalNet<uint64_t>& dn,
                           const tylsonia_t& tyl)
    {
        uint32_t s = dn.getS();
        uint32_t m = dn.getM();
        uint64_t b[s];
        Kahan sum;
        for (uint32_t i = 0; i < s; ++i) {
            b[i] = 0;
        }
        //sum.add(tylsonia_function_table(b) - 1.0);
        sum.add(tylsonia64(b, s, tyl) - 1.0);
        Gray gray;
        for (uint64_t cnt = 1; cnt < powtwo(m); ++cnt) {
            gray.next();
            int bit = gray.index();
            for (uint32_t i = 0; i < s; ++i) {
                b[i] ^= dn.getBase(bit, i);
            }
            //sum.add(tylsonia_function_table(b) - 1.0);
            sum.add(tylsonia64(b, s, tyl) - 1.0);
        }
        return log2(sum.get()) - static_cast<double>(m);
    }

    int read_tyl(string& filename, int min, int c, tylsonia_t& tyl)
    {
        const char * mode = "rb";
        FILE *fp = fopen(filename.c_str(), mode);
        if (fp == NULL) {
            cout << "can't open file:" << filename << endl;
            return -1;
        }
        int index = c - min;
        fseek(fp, sizeof(tylsonia_t) * index, 0);
        size_t count = fread(&tyl, sizeof(tylsonia_t), 1, fp);
        if (count != 1) {
            cout << "count = " << dec << count << endl;
            return -1;
        } else {
            return 0;
        }
    }

    double tylsonia64(const uint64_t x[], int size, const tylsonia_t& table)
    {
        double ret = 1.0;
        for (int i = 0; i < size; ++i) {
            for (int l = 0; l < 4; ++l) {
                // 48 - 16 * l assumes 64-bit
                int idx = (x[i] >> (48 - 16 * l)) & 65535;
                ret *= table.data[l][idx];
            }
        }
        return ret;
    }

#if 0
    double tylsonia(const uint64_t x[], int size, const tylsonia_t& table)
    {
        double ret = 1.0;
        for (int i = 0; i < size; ++i) {
            for (int l = 0; l < 4; ++l) {
                // 48 - 16 * l assumes 64-bit
                int idx = (x[i] >> (48 - 16 * l)) & 65535;
                ret *= table.data[l][idx];
            }
        }
        return ret;
    }
#endif

    Combination::Combination(int size) {
        this->size = size;
        this->data = new int[size];
    }

    Combination::~Combination() {
        delete[] data;
    }

    void Combination::reset(int init) {
        this->init = init;
        for (int i = 0; i < size - 1; i++) {
            data[i] = 0;
        }
        data[size - 1] = init;
    }

    bool Combination::next() {
        if (data[0] == init) {
            return false;
        }
        int last = size - 1;
        if (data[last] != 0) {
            data[last]--;
            data[last - 1]++;
            return true;
        }
        for (int i = last - 1; i >= 1; i--) {
            if (data[i] != 0) {
                data[last] = data[i] - 1;
                data[i] = 0;
                data[i - 1]++;
                return true;
            }
        }
        return false;
    }

    void Combination::print(ostream& os) {
        os << "Combination:";
        for (int i = 0; i < size; i++) {
            os << dec << data[i] << ",";
        }
        os << endl;
    }

}
