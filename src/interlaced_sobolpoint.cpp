#include <inttypes.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include "sobolpoint.h"

//#define DEBUG 1

using namespace std;

namespace {
    /// \brief Reverse the bits in a std::uint32_t.
    ///
    /// \see Bit reverse code from Stanford Bit hacks page:
    /// https://graphics.stanford.edu/~seander/bithacks.html#ReverseParallel
    uint32_t bitreverse(std::uint32_t k)
    {
        std::uint32_t v = k;
        v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);  // swap odd and even bits
        v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);  // swap consecutive pairs
        v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);  // swap nibbles ...
        v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);  // swap bytes
        v = ( v >> 16             ) | ( v               << 16); // swap 2-byte long pairs
        return v;
    }

    /// \brief Reverse the bits in a std::uint64_t.
    /// Calls twice the version for std::uint32_t.
    /// \see bitreverse(std::uint32_t)
    uint64_t bitreverse(std::uint64_t k)
    {
        return (std::uint64_t(bitreverse(std::uint32_t(k))) << 32)
            | std::uint64_t(bitreverse(std::uint32_t(k >> 32)));
    }
}

namespace DigitalNetNS {
    bool get_interlaced_sobol_base(std::istream& is,
                                   uint32_t s, uint32_t m,  uint64_t base[])
    {
#if defined(DEBUG)
        cout << "in get_interlaced_sobol_base" << endl;
#endif
        uint64_t tmp;
        //uint64_t x[m][s];
        for (unsigned int i = 0; i < s; i++) {
            string line;
            if (!is) {
#if defined(DEBUG)
                cout << "not enough is" << endl;
#endif
                return false;
            }
            getline(is, line);
            stringstream ss(line);
            for (unsigned int j = 0; j < m; j++) {
#if defined(DEBUG)
                cout << "(i, j) = (" << dec << i << "," << j << ")" << endl;
#endif
                if (!ss) {
#if defined(DEBUG)
                    cout << "not enough ss" << endl;
#endif
                    return false;
                }
                ss >> tmp;
#if defined(DEBUG)
                cout << "DEBUG:tmp = " << dec << tmp << endl;
#endif
                tmp = bitreverse(tmp);
                //x[j][i] = tmp;
                base[j * s + i] = tmp;
            }
        }
#if 0
        int idx = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < s; j++) {
                base[idx++] = x[i][j];
            }
        }
#endif
#if defined(DEBUG)
        cout << "out get_interlaced_sobol_base" << endl;
#endif
        return true;
    }

    int get_interlaced_sobol_s_max(const std::string& path, int alpha)
    {
        switch (alpha) {
        case 1:
            return get_sobol_s_max(path);
        case 2:
            return 10600;
        case 3:
            return 7067;
        case 4:
            return 5300;
        case 5:
            return 4240;
        default:
            return 0;
        }
        return 0;
    }
    int get_interlaced_sobol_s_min(const std::string&, int)
    {
        return 2;
    }
    int get_interlaced_sobol_m_max(const std::string&, int, int)
    {
        return 31;
    }
    int get_interlaced_sobol_m_min(const std::string&, int, int)
    {
        return 8;
    }
}
