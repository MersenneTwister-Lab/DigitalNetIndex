#include "DigitalNet.hpp"
#include "linear_scramble.hpp"
#include "calc_wafom_lookup.hpp"
#include "calc_tvalue.hpp"
#include "cvmean.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cerrno>
#include <getopt.h>
#include <cmath>

using namespace std;
using namespace DigitalNetNS;

int main(int argc, char * argv[])
{
    if (argc <= 3) {
        cout << argv[0] << " id s m [repeat]" << endl;
        return -1;
    }
    int id = strtoul(argv[1], NULL, 10);
    int s = strtoul(argv[2], NULL, 10);
    int m = strtoul(argv[3], NULL, 10);
    size_t repeat = 1000;
    if (argc >= 5) {
        repeat = strtoul(argv[4], NULL, 10);
    }
    digital_net_id dnid = static_cast<digital_net_id>(id);
    //print(cout, dn, false);
    int n = 64;
    static lookup_t table;
    double c = calc_c_for_cvmean(s, n);
    make_table(64, table, c);
#if 0
    for (int m = 10; m <= 18; m++) {
        linear_scramble<uint64_t>(dnid, s, m, table);
    }
#else
    DigitalNet<uint64_t> dn(dnid, s, m);
    Bests<uint64_t> bests(10, s, m);
    double wafom = random_linear_scramble(dn, bests, repeat, table);
    dn.pointInitialize();
    cout << "after random linear scramble wafom = " << wafom << endl;
    int t = calc_tvalue(dn);
    cout << "t = " << t << endl;
#if 1
    uint64_t save[s * m];
    for (size_t i = 0; i < bests.getSize(); i++) {
        bests.get(i, dn);
        dn.pointInitialize();
        double w = hill_climb_linear_scramble(dn, table);
        cout << "w = " << w << endl;
        dn.pointInitialize();
        //t = calc_tvalue(dn);
        //cout << "t = " << t << endl;
        if (w < wafom) {
            wafom = w;
            dn.saveBase(save, s * m);
        }
    }
#else
    wafom = hill_climb_linear_scramble(dn, table);
#endif
    cout << "after hill climb linear scramble wafom = " << wafom << endl;
    cout << "log2(wafom) = " << scientific << log2(wafom) << endl;
    //print(cout, dn, false);
#endif
}
