#pragma once
#ifndef BINARY_H
#define BINARY_H
#include <stdint.h>

#define MAGIC UINT64_C(0x36b5951d82b67241)

namespace DigitalNetNS {

    struct digital_net_header_t {
        uint32_t s;
        uint32_t m;
        uint32_t pos;
    };

    struct digital_net_data_t {
        uint32_t bit;
        uint32_t s;
        uint32_t m;
        uint32_t tvalue;
        double wafom;
    };

}

#endif // BINARY_H
