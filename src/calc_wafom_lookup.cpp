#include "calc_wafom_lookup.hpp"

using namespace std;
using namespace DigitalNetNS;

namespace {

    const double factor = 1.0;


//In terms of the Walsh coefficients of f , Goda, Ohori, Suzuki, and Yoshiki [10] proposed a quality criterion W (P) for randomized quasi-Monte Carlo integration using digitally shifted digital nets.
//const double factor = 2.0;

/**
 * S. Harase, "A search for extensible low-WAFOM point sets",
 * Monte Carlo Methods and Applications, Volume 22, Issue 4 (Dec 2016),
 * Pages 349-357.
 * Formula (3.1) in Acceleration using lookup table.
 */

    /**
     * calculate (-1)^{ej}
     */
    inline int32_t m1_power(int32_t ej)
    {
        return -2 * ej + 1;
    }

    double calc_lookup(int32_t c, int32_t e, double d)
    {
        double product = 1.0;
        for (int j = 1; j <= el; j++) {
            int32_t ej = static_cast<int32_t>((e >> (el - j)) & 1);
            //m2_exp *= 0.5; // j = 1
            double m2_exp = pow(2.0, - factor * (c * el + j + 1 - d));
            product *= 1.0 + m1_power(ej) * m2_exp;
        }
        return product;
    }
}

void make_table(int n, double table[4][65536], double d)
{
    int w;
    if (n == 64) {
        w = 4;
    } else {
        w = 2;
    }
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < 65536; j++) {
            table[i][j] = calc_lookup(i, j, d);
        }
    }
    for (int i = w; i < 4; i++) {
        for (int j = 0; j < 65536; j++) {
            table[i][j] = 0;
        }
    }
}
