#pragma once
#ifndef MATRIX_RANK_HPP
#define MATRIX_RANK_HPP
/**
 * @file matrix_rank.hpp
 *
 * @brief calculate matrix rank
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

#include "bit_operator.h"
#include <vector>

template <typename int_type>
int matrix_rank(std::vector<int_type> vec)
{
    const int ROW_SIZE = vec.size();
    const int COL_SIZE = 8 * sizeof(int_type);

    int ret = 0;
    for (int bit = 0; bit < COL_SIZE; ++bit) {
        bool found_one = false;
        for (int i = ret; i < ROW_SIZE; ++i) {
            if ( getBit(vec[i], bit) == 1 ) {
                found_one = true;
                int_type tmp = vec[ret];
                vec[ret] = vec[i];
                vec[i] = tmp;
                break;
            }
        }
        if ( !found_one ) {
            continue;
        }
        for (int i = ret + 1; i < ROW_SIZE; ++i) {
            if ( getBit(vec[i], bit) == 1 ) {
                vec[i] ^= vec[ret];
            }
        }
        ret++;
        if ( ret == ROW_SIZE ) {
            return ret;
        }
    }
    return ret;
}

#endif // MATRIX_RANK_HPP
