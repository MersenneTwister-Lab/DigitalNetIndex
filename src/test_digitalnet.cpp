#include "DigitalNet.hpp"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cerrno>

using namespace std;
using namespace DigitalNetNS;

static int test_digitalnet();
static int output_digitalnet(int argc, char * argv[]);

int main(int argc, char * argv[])
{
    if (argc == 1) {
        return test_digitalnet();
    } else {
        return output_digitalnet(argc, argv);
    }
}

static int test_digitalnet()
{
    size_t s = 4;
    size_t m = 10;
    DigitalNet<uint64_t> dn(NX, s, m);
    dn.pointInitialize();
    uint64_t count = UINT64_C(1) << m;
    for (size_t i = 0; i < count; i++) {
        for (size_t j = 0; j < s; j++) {
            double d = dn.getPoint(j);
            if (d <= 0 || d >= 1) {
                cout << "i = " << i << " j = " << j << " d = " << d << endl;
                return -1;
            }
        }
        dn.nextPoint();
    }
    return 0;
}

static int output_digitalnet(int argc, char * argv[])
{
    if (argc <= 2) {
        cout << argv[0] << " id n" << endl;
        return -1;
    }
    errno = 0;
    int id = strtol(argv[1], NULL, 10);
    int n = strtol(argv[2], NULL, 10);
    if (errno) {
        cout << "id and n should be a number" << endl;
    }
    if (id == 100) {
        if (n == 64) {
            DigitalNet<uint64_t> dn(cin);
            print(cout, dn);
            return 0;
        } else {
            DigitalNet<uint32_t> dn(cin);
            print(cout, dn);
            return 0;
        }
    }
    if (argc <= 4) {
        cout << argv[0] << " id n s m" << endl;
        return -1;
    }
    uint32_t s = strtoul(argv[3], NULL, 10);
    uint32_t m = strtoul(argv[4], NULL, 10);
    digital_net_id dnid = static_cast<digital_net_id>(id);
    if (n == 64) {
        DigitalNet<uint64_t> dn(dnid, s, m);
        print(cout, dn);
        dn.pointInitialize();
        const double *p;
        for (int i = 0; i < 2; i++) {
            p = dn.getPoint();
            for (uint32_t j = 0; j < s; j++) {
                cout << p[j] << " ";
            }
            cout << endl;
            dn.nextPoint();
        }
        return 0;
    } else {
        DigitalNet<uint32_t> dn(dnid, s, m);
        print(cout, dn);
        return 0;
    }
    return 0;
}
