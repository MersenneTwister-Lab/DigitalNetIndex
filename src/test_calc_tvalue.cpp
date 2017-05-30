#include "calc_tvalue.hpp"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cerrno>

using namespace std;
using namespace DigitalNetNS;

namespace {
    int test_tvalue();
    int output_tvalue(int argc, char * argv[]);
    struct check {
        int id;
        int n;
        int s;
        int m;
        int t;
    };
    check data[12] = {
        {0, 64, 4, 10, 1},
        {0, 64, 5, 10, 2},
        {0, 64, 6, 10, 3},
        {1, 64, 7, 11, 6},
        {1, 64, 8, 11, 6},
        {1, 64, 9, 11, 7},
        {3, 64, 10, 12, 7},
        {3, 64, 4, 12, 1},
        {3, 64, 5, 12, 2},
        {4, 64, 6, 13, 6},
        {4, 64, 7, 13, 6},
        {4, 64, 8, 13, 7}
    };
}

int main(int argc, char * argv[])
{
    if (argc == 1) {
        return test_tvalue();
    } else {
        return output_tvalue(argc, argv);
    }
}

namespace {
    int test_tvalue()
    {
        for (int i = 0; i < 12; i++) {
            int id = data[i].id;
            int n = data[i].n;
            int s = data[i].s;
            int m = data[i].m;
            int t = data[i].t;
            digital_net_id dnid = static_cast<digital_net_id>(id);
            DigitalNet<uint64_t> dn(dnid, s, m);
            int tvalue = calc_tvalue(dn);
            if (tvalue != t) {
                cout << "name = " << dn.getName() << endl;
                cout << "id = " << id << endl;
                cout << "n = " << n << endl;
                cout << "s = " << s << endl;
                cout << "m = " << m << endl;
                cout << "tvalue = " << tvalue << endl;
                cout << "expected tvalue = " << t << endl;
                return -1;
            }
        }
        return 0;
    }

    int output_tvalue(int argc, char * argv[])
    {
        if (argc <= 2) {
            cout << argv[0] << " id n " << endl;
            cout << "id = 100 means from stdin" << endl;
            return -1;
        }
        errno = 0;
        int id = strtol(argv[1], NULL, 10);
        int n = strtol(argv[2], NULL, 10);
        if (errno) {
            cout << "id n and c should be a number" << endl;
        }
        int tvalue;
        cout << "id = " << id << endl;
        cout << "n = " << n << endl;
        if (id == 100) {
            cout << "id = 100; from stdin" << endl;
            if (n == 64) {
                DigitalNet<uint64_t> dn(cin);
                tvalue = calc_tvalue(dn);
                cout << "tvalue = " << tvalue << endl;
                return 0;
            } else {
                DigitalNet<uint32_t> dn(cin);
                tvalue = calc_tvalue(dn);
                cout << "tvalue = " << tvalue << endl;
                return 0;
            }
        }
        if (argc <= 4) {
            cout << argv[0] << " id n s m" << endl;
            return -1;
        }
        uint32_t s = strtoul(argv[3], NULL, 10);
        uint32_t m = strtoul(argv[4], NULL, 10);
        cout << "s = " << s << endl;
        cout << "m = " << m << endl;
        digital_net_id dnid = static_cast<digital_net_id>(id);
        if (n == 64) {
            DigitalNet<uint64_t> dn(dnid, s, m);
            cout << "name = " << dn.getName() << endl;
            tvalue = calc_tvalue(dn);
            cout << "tvalue = " << tvalue << endl;
            return 0;
        } else {
            DigitalNet<uint32_t> dn(dnid, s, m);
            cout << "name = " << dn.getName() << endl;
            tvalue = calc_tvalue(dn);
            cout << "tvalue = " << tvalue << endl;
            return 0;
        }
        return 0;
    }
}
