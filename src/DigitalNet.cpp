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
#include "powtwo.h"
#include "bit_operator.h"
#include "DigitalNet.h"
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
    const int N = 64;
    const int S_MIN = 4;
    const int S_MAX = 10;
    const int M_MIN = 10;
    const int M_MAX = 18;

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
    const double powhalftable[67] =
    {
        1.00000000000000000000e+00,
        5.00000000000000000000e-01,
        2.50000000000000000000e-01,
        1.25000000000000000000e-01,
        6.25000000000000000000e-02,
        3.12500000000000000000e-02,
        1.56250000000000000000e-02,
        7.81250000000000000000e-03,
        3.90625000000000000000e-03,
        1.95312500000000000000e-03,
        9.76562500000000000000e-04,
        4.88281250000000000000e-04,
        2.44140625000000000000e-04,
        1.22070312500000000000e-04,
        6.10351562500000000000e-05,
        3.05175781250000000000e-05,
        1.52587890625000000000e-05,
        7.62939453125000000000e-06,
        3.81469726562500000000e-06,
        1.90734863281250000000e-06,
        9.53674316406250000000e-07,
        4.76837158203125000000e-07,
        2.38418579101562500000e-07,
        1.19209289550781250000e-07,
        5.96046447753906250000e-08,
        2.98023223876953125000e-08,
        1.49011611938476562500e-08,
        7.45058059692382812500e-09,
        3.72529029846191406250e-09,
        1.86264514923095703125e-09,
        9.31322574615478515625e-10,
        4.65661287307739257812e-10,
        2.32830643653869628906e-10,
        1.16415321826934814453e-10,
        5.82076609134674072266e-11,
        2.91038304567337036133e-11,
        1.45519152283668518066e-11,
        7.27595761418342590332e-12,
        3.63797880709171295166e-12,
        1.81898940354585647583e-12,
        9.09494701772928237915e-13,
        4.54747350886464118958e-13,
        2.27373675443232059479e-13,
        1.13686837721616029739e-13,
        5.68434188608080148697e-14,
        2.84217094304040074348e-14,
        1.42108547152020037174e-14,
        7.10542735760100185871e-15,
        3.55271367880050092936e-15,
        1.77635683940025046468e-15,
        8.88178419700125232339e-16,
        4.44089209850062616169e-16,
        2.22044604925031308085e-16,
        1.11022302462515654042e-16,
        5.55111512312578270212e-17,
        2.77555756156289135106e-17,
        1.38777878078144567553e-17,
        6.93889390390722837765e-18,
        3.46944695195361418882e-18,
        1.73472347597680709441e-18,
        8.67361737988403547206e-19,
        4.33680868994201773603e-19,
        2.16840434497100886801e-19,
        1.08420217248550443401e-19,
        5.42101086242752217004e-20,
        2.71050543121376108502e-20,
        1.35525271560688054251e-20
    };

    // powhalf = {0.5^i | 0 <= i <= 33};
    // powhalf[j] = 1/2^j
    double powhalf(int index)
    {
        if (index < 0 || index > 66) {
            cerr << "index out of range in powhalf. index = " << dec << index
                 << "\nindex r should be in the range 0 <= r <= 66" << endl;
            throw invalid_argument("index out of range in powhalf");
        }
        return powhalftable[index];
    }

    int ones32(uint32_t x) {
        x -= ((x >> 1) & 0x55555555);
        x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
        x = (((x >> 4) + x) & 0x0f0f0f0f);
        x += (x >> 8);
        x += (x >> 16);
        return(x & 0x0000003f);
    }
    int bitPos(uint32_t x) {
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);
        x |= (x >> 8);
        x |= (x >> 16);
        return(ones32(x >> 1));
    }

}

DigitalNet<uint64_t>::Gray::Gray()
{
    count = 0;
    pre = 0;
    gray = 0;
}

void DigitalNet<uint64_t>::Gray::clear() {
    count = 0;
    pre = 0;
    gray = 0;
}
uint32_t DigitalNet<uint64_t>::Gray::next() {
    count++;
    pre = gray;
    gray = count ^ (count >> 1);
    return gray;
}
int DigitalNet<uint64_t>::Gray::index() {
    return bitPos(pre ^ gray);
}

#if 0
DigitalNet<uint64_t>::DigitalNet(int s, int m)
{
    this->s = s;
    this->m = m;
    this->base = new uint64_t[s * m];
    this->point_base = NULL;
    this->point = NULL;
    this->count = 0;
}
#endif

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
DigitalNet<uint64_t>::DigitalNet(std::istream& is)
{
    int dmy;
    is >> dmy;
    if (dmy != N) {
        cerr << "data type mismatch! N should be 64 but " << dec << dmy << endl;
        throw runtime_error("data type mismatch!");
    }
    is >> s;
    is >> m;
    uint64_t data[s * m];
    uint64_t tmp;
    uint32_t i;
    uint32_t j;
    for (i = 0; i < m; i++) {
        for (j = 0; j < s; j++) {
            if (!is) {
                cerr << "too less data i = " << dec << i
                     << " j = " << j
                     << " s = " << s << " m = " << m
                     << endl;
                throw runtime_error("data type mismatch!");
            }
            is >> tmp;
            data[i * s + j] = tmp;
        }
    }
    if (i * s + j < s * m) {
        cerr << "too less data i = " << dec << i
             << " s = " << s << " m = " << m
             << endl;
        throw runtime_error("data type mismatch!");
    }
    if (is) {
        is >> wafom;
    }
    if (is) {
        is >> tvalue;
    }
    base = new uint64_t[s * m];
    for (uint32_t i = 0; i < s * m; i++) {
        base[i] = data[i];
    }
    this->point_base = NULL;
    this->point = NULL;
    this->count = 0;
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
DigitalNet<uint64_t>::DigitalNet(const digital_net_id& id,
                                 uint32_t s,
                                 uint32_t m)
{
    this->s = s;
    this->m = m;
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
    string name = digital_net_name_data[id].name;
    path += name;
    path += ".dat";
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
    count = fread(dn.data, size, 1, fp);
    if (count != 1) {
        cout << "fail to read digtal net array" << endl;
        throw runtime_error("fail to read digtal net array");
    }
    wafom = dn.wafom;
    tvalue = dn.tvalue;
    base = new uint64_t[s * m];
    uint64_t * data = dn.data;
    for (uint32_t i = 0; i < s * m; i++) {
        base[i] = data[i];
    }
    this->point_base = NULL;
    this->point = NULL;
    this->count = 0;
}

#if 0
DigitalNet<uint64_t>::DigitalNet(const DigitalNet<uint64_t>& that)
{
    s = that.s;
    m = that.m;
    wafom = that.wafom;
    tvalue = that.tvalue;
    base = new uint64_t[s * m];
    for (int i = 0; i < s * m; i++) {
        base[i] = that.base[i];
    }
    if (that.point_base == NULL) {
        this->point_base = NULL;
    } else {
        this->point_base = new uint64_t[s];
        for (int i = 0; i < s; i++) {
            this->point_base[i] = that.point_base[i];
        }
    }
    if (that.point == NULL) {
        this->point = NULL;
    } else {
        this->point = new double[s];
        for (int i = 0; i < s; i++) {
            this->point[i] = that.point[i];
        }
    }
    this->count = that.count;
}
#endif

DigitalNet<uint64_t>::~DigitalNet()
{
    delete[] base;
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
    shift = mt();
    for (uint32_t i = 0; i < s; ++i) {
        point_base[i] = getBase(0, i);
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

    int bit = gray.index();
    for (uint32_t i = 0; i < s; ++i) {
        point_base[i] ^= getBase(bit, i);
        // shift して1を立てている
        point[i] = static_cast<double>(point_base[i] ^ shift)
            * powhalf(N) + powhalf(N + 1);
    }
    if (count == powtwo(m)) {
        count = 0;
    } else {
        gray.next();
        count++;
    }
}

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

void DigitalNet<uint64_t>::setSeed(uint64_t seed)
{
    mt.seed(seed);
}

const char * DigitalNet<uint64_t>::getDataPath()
{
    return getenv(digital_net_path.c_str());
}

uint32_t DigitalNet<uint64_t>::getParameterSize()
{
    return digital_net_name_data_size;
}

const string DigitalNet<uint64_t>::getDigitalNetName(uint32_t index)
{
    if (index < digital_net_name_data_size) {
        return digital_net_name_data[index].name;
    } else {
        return "";
    }
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