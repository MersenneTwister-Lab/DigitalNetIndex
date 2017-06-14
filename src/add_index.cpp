/**
 * @file add_index.cpp
 *
 * 1行に1つデジタルネットのデータが書かれたファイルを読み込んで、その後ろにwafom(mean)とtvalue
 * を追加する
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
#include "calc_wafom_lookup.hpp"
#include "calc_tvalue.hpp"
#include "cvmean.h"
#include "DigitalNet.hpp"
#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <cerrno>

using namespace std;
using namespace DigitalNetNS;

int main(int argc, char * argv[])
{
    if (argc <= 1) {
        cout << argv[0] << " digitalnet-filename" << endl;
        return -1;
    }
    ifstream dnstream(argv[1]);
    string line;
    if (!dnstream) {
        cout << "can't open digital_net_file" << endl;
        return -1;
    }
    static lookup_t table;
    while (dnstream) {
        getline(dnstream, line);
        //cout << "line:" << line << endl;
        if (!dnstream) {
            //cout << "EOF" << endl;
            return 0;
        }
        stringstream ss;
        ss << line;
        DigitalNet<uint64_t> dn(ss);
        print(cout, dn, false, ' ');
        //cout << "before calc_c_for_cvmean" << endl;
        double c = calc_c_for_cvmean(dn.getS(), 64);
        make_table(64, table, c);
        //cout << "before calc_wafom" << endl;
        double wafom = calc_wafom(dn, table);
        cout << " " << log2(wafom);
        int64_t tvalue = calc_tvalue(dn);
        cout << " " << tvalue << endl;
    }
    return 0;
}
