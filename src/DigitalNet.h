#pragma once
#ifndef DIGITAL_NET_H
#define DIGITAL_NET_H
/**
 * @file DigitalNet.h
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
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <cerrno>
#include <random>

template<typename T>
class DigitalNet {
public:
    static const char * getDataPath();
    static uint32_t getParameterSize();
    static const std::string getDigitalNetName(uint32_t index);
    static const std::string getDigitalNetConstruction(uint32_t index);
    static uint32_t getSMax();
    static uint32_t getSMin();
    static uint32_t getMMax();
    static uint32_t getMMin();
private:
    ~DigitalNet();
    DigitalNet(int s, int m);
    T dmy;
};

// Explicit specialization of template class
template<>
class DigitalNet<uint64_t> {
private:
    // First of all, forbid copy and assign.
    DigitalNet(const DigitalNet<uint64_t>& that);
    DigitalNet<uint64_t>& operator=(const DigitalNet<uint64_t>&);
    class Gray {
    public:
        Gray();
        void clear();
        uint32_t next();
        int index();
    private:
        uint32_t count;
        uint32_t gray;
        uint32_t pre;
    };

public:
    enum digital_net_id {
        NX = 0,
        SO = 1,
        OLDSO = 2,
        NXLW = 3,
        SOLW = 4,
        RANDOM = -1
    };

    DigitalNet(std::istream& is);
    DigitalNet(const digital_net_id& id, uint32_t s, uint32_t m);
    ~DigitalNet();
    uint64_t getBase(int i, int j) const {
        return base[i * s + j];
    }
    double getPoint(int i) const {
        return point[i];
    }
    const double * getPoint() const {
        return point;
    }
    uint32_t getS() const {
        return s;
    }
    uint32_t getM() const {
        return m;
    }
    void showStatus(std::ostream& os);
    void scramble();
    void pointInitialize();
    void nextPoint();
    void setSeed(uint64_t seed);
    double getWAFOM() {
        return wafom;
    }
    int64_t getTvalue() {
        return tvalue;
    }
    static const char * getDataPath();
    static uint32_t getParameterSize();
    static const std::string getDigitalNetName(uint32_t index);
    static const std::string getDigitalNetConstruction(uint32_t index);
    static uint32_t getSMax();
    static uint32_t getSMin();
    static uint32_t getMMax();
    static uint32_t getMMin();
private:
    void setBase(int i, int j, uint64_t value) {
        base[i * s + j] = value;
    }
    void scramble(int i, int j, int l);
    uint32_t s;
    uint32_t m;
    uint64_t shift;
    uint64_t count;
    double wafom;
    int64_t tvalue;
    Gray gray;
    std::mt19937_64 mt;
    uint64_t * base;
    uint64_t * point_base;
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
               << dn.get(k, i) << " ";
        }
        os << endl;
    }
}

#endif // DIGITAL_NET_HPP
