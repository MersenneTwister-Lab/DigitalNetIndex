#pragma once
#ifndef SOBOL_POINT_H
#define SOBOL_POINT_H

#include <iostream>
#include <inttypes.h>

namespace DigitalNetNS {
    bool get_sobol_base(std::istream& is,
                        uint32_t s, uint32_t m,  uint64_t base[]);
    bool select_sobol_base(const std::string& path, int bitsize,
                           uint32_t s, uint32_t m,  uint64_t base[]);
}
#endif //SOBOL_POINT_H
