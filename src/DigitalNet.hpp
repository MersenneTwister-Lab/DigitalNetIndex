#pragma once
#ifndef DIGITAL_NET_HPP
#define DIGITAL_NET_HPP
/**
 * @file DigitalNet.hpp
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
#include "grayindex.hpp"
#include <stdint.h>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <cerrno>
#include <random>

namespace DigitalNetNS {

    enum digital_net_id {
        NX = 0,
        SO = 1,
        OLDSO = 2,
        NXLW = 3,
        SOLW = 4,
        RANDOM = -1
    };

    uint32_t getParameterSize();
    const std::string getDigitalNetName(uint32_t index);
    const std::string getDigitalNetConstruction(uint32_t index);
    int readDigitalNetData(digital_net_id id, uint32_t s, uint32_t m,
                           uint64_t base[],
                           int * tvalue, double * wafom);
    int readDigitalNetData(digital_net_id id, uint32_t s, uint32_t m,
                           uint32_t base[],
                           int * tvalue, double * wafom);
    int readDigitalNetHeader(std::istream& is, int * n,
                             uint32_t * s, uint32_t * m);
    int readDigitalNetData(std::istream& is, int n,
                           uint32_t s, uint32_t m,
                           uint64_t base[],
                           int * tvalue, double * wafom);
    int readDigitalNetData(std::istream& is, int n,
                           uint32_t s, uint32_t m,
                           uint32_t base[],
                           int * tvalue, double * wafom);
    uint32_t getSMax();
    uint32_t getSMin();
    uint32_t getMMax();
    uint32_t getMMin();

    template<typename U>
    class DigitalNet {
    private:
        // First of all, forbid copy and assign.
        DigitalNet(const DigitalNet<U>& that);
        DigitalNet<U>& operator=(const DigitalNet<U>&);

    public:
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
        DigitalNet(std::istream& is) {
            //using namespace std;
            int n;
            id = -100;
            int r = readDigitalNetHeader(is, &n, &s, &m);
            if (r != 0) {
                throw std::runtime_error("data type mismatch!");
            }
            base = new U[s * m]();
            r = readDigitalNetData(is, n, s, m, base,
                                   &tvalue, &wafom);
            if (r != 0) {
                throw std::runtime_error("data type mismatch!");
            }
            shift = NULL;
            point_base = NULL;
            point = NULL;
            count = 0;
        }

        DigitalNet(const digital_net_id& id, uint32_t s, uint32_t m) {
            using namespace std;
            this->s = s;
            this->m = m;
            this->id = static_cast<int>(id);
            base = new U[s * m]();
            int r = readDigitalNetData(id, s, m, base,
                                       &tvalue, &wafom);
            if (r != 0) {
                throw runtime_error("data type mismatch!");
            }
            shift = NULL;
            point_base = NULL;
            point = NULL;
            count = 0;
        }

        ~DigitalNet() {
            if (base != NULL) {
                delete[] base;
            }
            if (shift != NULL) {
                delete[] shift;
            }
            if (point_base != NULL) {
                delete[] point_base;
            }
            if (point != NULL) {
                delete[] point;
            }
        }

        U getBase(int i, int j) const {
            return base[i * s + j];
        }

        double getPoint(int i) const {
            return point[i];
        }

        const double * getPoint() const {
            return point;
        }

        const U * getPointBase() const {
            return point_base;
        }

        uint32_t getS() const {
            return s;
        }
        uint32_t getM() const {
            return m;
        }
        const std::string getName() {
            if (id >= 0) {
                return getDigitalNetName(id);
            } else {
                return "no name";
            }
        }
        void showStatus(std::ostream& os);
        void scramble();
        void pointInitialize() {
#if defined(DEBUG)
            using namespace std;
            cout << "in pointInitialize" << endl;
#endif
            if (shift == NULL) {
                shift = new U[s]();
            }
            if (point_base == NULL) {
                point_base = new U[s]();
            }
            if (point == NULL) {
                point = new double[s]();
            }
            for (uint32_t i = 0; i < s; ++i) {
                //point_base[i] = getBase(0, i);
                shift[i] = mt();
            }
            gray.clear();
            count = 0;
            count++;
            //nextPoint();
#if defined(DEBUG)
            cout << "out pointInitialize" << endl;
#endif
        }

        void nextPoint() {
#if defined(DEBUG)
            using namespace std;
            cout << "in nextPoint" << endl;
#endif
            if (count == (UINT64_C(1) << m)) {
                pointInitialize();
                //return;
            }
            int get_max;
            double factor;
            double eps;
            if (sizeof(U) * 8 == 64) {
                get_max = 64 - 53;
                factor = exp2(-53);
                eps = exp2(-64);
            } else {
                get_max = 0;
                factor = exp2(-32);
                eps = exp2(-33);
            }
            int bit = gray.index();
#if defined(DEBUG)
            cout << "bit = " << bit << endl;
            cout << "before boint_base:" << endl;
            for (size_t i = 0; i < s; i++) {
                cout << point_base[i] << " ";
            }
            cout << endl;
#endif
            for (uint32_t i = 0; i < s; ++i) {
                point_base[i] ^= getBase(bit, i);
                // shift して1を立てている
                uint64_t tmp = (point_base[i] ^ shift[i]) >> get_max;
                point[i] = static_cast<double>(tmp) * factor + eps;
            }
            if (count == (UINT64_C(1) << m)) {
                count = 0;
                gray.clear();
            } else {
                gray.next();
                count++;
            }
#if defined(DEBUG)
            cout << "after boint_base:" << endl;
            for (size_t i = 0; i < s; i++) {
                cout << point_base[i] << " ";
            }
            cout << endl;
            cout << "out nextPoint" << endl;
#endif
        }
        void setSeed(U seed);
        double getWAFOM() {
            return wafom;
        }

        int64_t getTvalue() {
            return tvalue;
        }
    private:
        void setBase(int i, int j, U value) {
            base[i * s + j] = value;
        }
        void scramble(int i, int j, int l);
        int id;
        uint32_t s;
        uint32_t m;
        uint64_t count;
        double wafom;
        int tvalue;
        GrayIndex gray;
        std::mt19937_64 mt;
        U * base;
        U * point_base;
        U * shift;
        double * point;
    };


    template<typename T>
    void print(std::ostream& os, const DigitalNet<T>& dn)
    {
        using namespace std;
        int s = dn.getS();
        int m = dn.getM();
        os << "n = " << (sizeof(T) * 8) << endl;
        os << "s = " << s << endl;
        os << "m = " << m << endl;
        for (int k = 0; k < m; k++) {
            for (int i = 0; i < s; i++) {
                os << "base[" << dec << k << "][" << i << "]"
                   << hex << dn.getBase(k, i) << endl;
            }
            //os << endl;
        }
    }
}

#endif // DIGITAL_NET_HPP
