#pragma once
#ifndef POWTWO_H
#define POWTWO_H
/**
 * @file powtwo.h
 *
 * @brief calculate power of two. (2^n)
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
 *
 */

#include <iostream>
#include <iomanip>
#include <inttypes.h>

template<typename U = uint64_t>
static inline U exp2ul(int index) {
    const U one = 1;
    return one << index;
}

#endif // POWTWO_H
