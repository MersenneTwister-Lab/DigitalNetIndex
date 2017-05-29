/**
 * @file digitalnet_index.cpp
 *
 * @brief main program for calculate WAFORM and t-value of digital net.
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
#include "calc_wafom.hpp"
#include "calc_tvalue.hpp"
#include "cvmean.h"
#include "DigitalNet.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <cerrno>
#include <getopt.h> // <cgetopt> is not accepted by gcc

//#define DEBUG

using namespace std;
using namespace DigitalNet;

struct cmd_option {
    bool wafom;
    bool meanwafom;
    bool tvalue;
    bool verbose;
    double c;
    //double cv;
    string dnfile;
};

static bool parse(cmd_option& opt, int argc, char * argv[]);
static void show_error_message(string& pgm);

int main(int argc, char * argv[])
{
    cmd_option opt;
    if (!parse(opt, argc, argv)) {
        return -1;
    }
#if defined(DEBUG)
    cout << "opt.dnfile = " << opt.dnfile << endl;
#endif
    ifstream dnstream(opt.dnfile);
    if (!dnstream) {
        cout << "can't open digital_net_file" << endl;
        return -1;
    }
#if 0
    int dmy;
    int s;
    int m;
    dnstream >> dmy;
    dnstream >> s;
    dnstream >> m;
//#if defined(DEBUG) && 0
    cout << "dmy = " << dmy << endl;
    cout << "s = " << s << endl;
    cout << "m = " << m << endl;
#endif
    DigitalNet<uint64_t> dn(dnstream);
    if (opt.verbose) {
        //dn.showStatus(cout);
        print(cout, dn);
    }
    //print_dn<uint64_t>(cout, dn);
    if (opt.wafom && (opt.c < -5.0 || opt.c > 5.0)) {
        cout << "c should be in the range -5.0 <= c <= 5.0" << endl;
        return -1;
    }
#if defined(DEBUG)
    cout << "c = " << dec << opt.c << endl;
#endif
    if (opt.wafom) {
        double c = opt.c;
        //double wafom = compute_WAFOM(dn, c);
        double wafom = calc_wafom(dn, c);
        cout << "c = " << opt.c << endl;
        cout << "wafom = " << fixed << setprecision(20) << wafom << endl;
    }
#if 0
    if (opt.meanwafom && (opt.cv >= 0)) {
        cout << "cv should be less than zero" << endl;
        return -1;
    }
#endif
    if (opt.meanwafom) {
        //double wafom = compute_WAFOM(dn, MEAN, -1);
        double c = calc_c_for_cvmean(dn.getS(), 64);
        double wafom = calc_wafom(dn, c);
        //cout << "cv = " << opt.cv << endl;
        cout << "mean-wafom = " << fixed << setprecision(18) << wafom << endl;
    }
    if (opt.tvalue) {
        int64_t tvalue = calc_tvalue(dn);
        cout << "tvalue = " << tvalue << endl;
        if (tvalue < 0) {
            return -3;
        }
    }
    return 0;
}

static bool parse(cmd_option& opt, int argc, char * argv[])
{
    opt.wafom = false;
    opt.meanwafom = false;
    opt.tvalue = false;
    opt.c = 0;
    //opt.cv = 0;
    opt.verbose = false;
    static struct option longopts[] = {
        {"wafom", required_argument, NULL, 'w'},
        {"mean-wafom", no_argument, NULL, 'm'},
        {"tvalue", no_argument, NULL, 't'},
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
        c = getopt_long(argc, argv, "?w:mtv", longopts, NULL);
        if (c < 0) {
            break;
        }
        switch (c) {
        case 'w':
            opt.wafom = true;
            opt.c = strtod(optarg, NULL);
            if (errno) {
                error = true;
                cout << "c must be a number" << endl;
            }
            break;
        case 'm':
            opt.meanwafom = true;
            //opt.cv = strtod(optarg, NULL);
            break;
        case 't':
            opt.tvalue = true;
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
    if (!opt.wafom && !opt.tvalue && !opt.meanwafom) {
        error = true;
    }
    if (error) {
        show_error_message(pgm);
        return false;
    }
    argc -= optind;
    argv += optind;
    if (argc <= 0) {
        error = true;
    } else {
        opt.dnfile = argv[0];
    }
    if (error) {
        show_error_message(pgm);
        return false;
    }
    return true;
}

static void show_error_message(string& pgm)
{
    cout << pgm << " [-t] [-m] [-w c] digital-net-datafile" << endl;
    cout << "\t--tvalue, -t\t\tcompute t-value of digitalnet" << endl;
    cout << "\t--mean-wafom, -m\tcompute mean wafom of digitalnet"
         << endl;
    cout << "\t--wafom, -w\t\tcompute wafom of digitalnet for specified c"
         << endl;
}
