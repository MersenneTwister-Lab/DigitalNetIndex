#pragma once
#ifndef COMPUTE_H
#define COMPUTE_H
/**
 * @file compute.h
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

#include "DigitalNet.h"

enum wafom_kind {
    BIG,
    MEAN
};

double compute_WAFOM64(const DigitalNet<uint64_t>& dn,
                       enum wafom_kind kind,
                       double c);
double compute_WAFOM32(const DigitalNet<uint32_t>& dn,
                       enum wafom_kind kind,
                       double c);

template<typename int_type>
static inline double compute_WAFOM(const DigitalNet<int_type>& dn,
                                   enum wafom_kind kind,
                                   double c)
{
    return compute_WAFOM(dn, kind, c);
}

template<>
double compute_WAFOM(const DigitalNet<uint64_t>& dn,
                                   enum wafom_kind kind,
                                   double c)
{
    return compute_WAFOM64(dn, kind, c);
}

template<>
double compute_WAFOM(const DigitalNet<uint32_t>& dn,
                     enum wafom_kind kind,
                     double c)
{
    return compute_WAFOM32(dn, kind, c);
}

int64_t compute_tvalue64(const DigitalNet<uint64_t>& dn);

template<typename T>
static inline int64_t compute_tvalue(const DigitalNet<T>& dn)
{
    return compute_tvalue(dn);
}

template<>
int64_t compute_tvalue(const DigitalNet<uint64_t>& dn)
{
    return compute_tvalue64(dn);
}

#endif // COMPUTE_H
