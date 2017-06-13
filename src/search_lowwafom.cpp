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

struct cmd_option {
    digital_net_id id;
    int start_s;
    int end_s;
    int start_m;
    int end_m;
    uint64_t seed;
    int repeat;
    bool verbose;
};

static bool parse(cmd_option& opt, int argc, char * argv[]);
static void show_error_message(string& pgm);

int main(int argc, char * argv[])
{
    cmd_option opt;
    if (!parse(opt, argc, argv)) {
        return -1;
    }
    digital_net_id dnid = opt.id;
    //print(cout, dn, false);
    int n = 64;
    static lookup_t table;
    for (int s = opt.start_s; s <= opt.end_s; s++) {
        double c;
        if (s <= 2) {
            c = 2.0;
        } else {
            c = calc_c_for_cvmean(s, n);
        }
        make_table(64, table, c);
        for (int m = opt.start_m; m <= opt.end_m; m++) {
            linear_scramble<uint64_t>(dnid, s, m, opt.seed, opt.repeat, table);
        }
    }
    return 0;
#if 0
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

static bool parse(cmd_option& opt, int argc, char * argv[])
{
    opt.id = SOBOL;
    opt.start_s = 5;
    opt.end_s = 5;
    opt.start_m = 10;
    opt.end_m = 18;
    opt.seed = 0;
    opt.repeat = 200;
    opt.verbose = false;
    static struct option longopts[] = {
        {"id", required_argument, NULL, 'i'},
        {"start-s", required_argument, NULL, 's'},
        {"end-s", required_argument, NULL, 'S'},
        {"start-m", required_argument, NULL, 'm'},
        {"end-m", required_argument, NULL, 'M'},
        {"repeat", required_argument, NULL, 'r'},
        {"seed", required_argument, NULL, 'q'},
        {"verbose", no_argument, NULL, 'v'},
        {NULL, 0, NULL, 0}};
    string pgm = argv[0];
    int c;
    bool error = false;
    errno = 0;
    for (;;) {
        if (error) {
            break;
        }
        c = getopt_long(argc, argv, "?i:s:S:m:M:r:q:v", longopts, NULL);
        if (c < 0) {
            break;
        }
        switch (c) {
        case 'i':
            opt.id = static_cast<digital_net_id>(strtol(optarg, NULL, 10));
            if (errno) {
                error = true;
                cout << "id must be a number" << endl;
            }
            break;
        case 's':
            opt.start_s = strtoul(optarg, NULL, 10);
            if (errno) {
                error = true;
                cout << "start_s must be a number" << endl;
            }
            opt.end_s = opt.start_s;
            break;
        case 'S':
            opt.end_s = strtoul(optarg, NULL, 10);
            if (errno) {
                error = true;
                cout << "end_s must be a number" << endl;
            }
            break;
        case 'm':
            opt.start_m = strtoul(optarg, NULL, 10);
            if (errno) {
                error = true;
                cout << "start_m must be a number" << endl;
            }
            opt.end_m = opt.start_m;
            break;
        case 'M':
            opt.end_m = strtoul(optarg, NULL, 10);
            if (errno) {
                error = true;
                cout << "end_m must be a number" << endl;
            }
            break;
        case 'r':
            opt.repeat = strtoul(optarg, NULL, 10);
            if (errno) {
                error = true;
                cout << "repeat must be a number" << endl;
            }
            break;
        case 'q':
            opt.seed = strtoul(optarg, NULL, 10);
            if (errno) {
                error = true;
                cout << "seed must be a number" << endl;
            }
            break;
        case 'v':
            opt.verbose = true;
            break;
        case '?':
        default:
            error = true;
            break;
        }
    }
    if (error) {
        show_error_message(pgm);
        return false;
    }
    if (error) {
        show_error_message(pgm);
        return false;
    }
    return true;
}

static void show_error_message(string& pgm)
{
    cout << pgm << " [-i id] [-s start_s] [-S end_s] [-m start_m] [-M end_m]"
         << "[-r repeat] [-q seed] [-v]" << endl;
}
