#include "calc_wafom.hpp"
#include "calc_wafom_lookup.hpp"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cerrno>
#include <cmath>
#include <cfloat>

using namespace std;
using namespace DigitalNetNS;

namespace {
    int test_wafom();
    int output_wafom(int argc, char * argv[]);
    struct check {
        int id;
        int n;
        double c;
        int s;
        int m;
        double wafom;
    };
#define DATA_NUM 24
    check data[DATA_NUM] = {
        {0, 64, 1.1, 4, 11, 0.003061420111993816},
        {0, 64, 1.1, 5, 11, 0.016754481282423610},
        {0, 64, 1.1, 6, 11, 0.066418429041965252},
        {1, 64, 1.2, 4, 12, 0.003231439197273305},
        {1, 64, 1.2, 5, 12, 0.013289109548992551},
        {1, 64, 1.2, 6, 12, 0.047812420678247189},
        {3, 64, 1.0, 4, 10, 0.003018448843766871},
        {3, 64, 1.0, 5, 10, 0.020904742052930664},
        {3, 64, 1.0, 6, 10, 0.089295665147268208},
        {4, 64, 1.3, 4, 13, 0.000301522180988170},
        {4, 64, 1.3, 5, 13, 0.004028031307309140},
        {4, 64, 1.3, 6, 13, 0.025458776013060007},
        {5, 64, 1.3, 4, 13, 0.000859204},
        {5, 64, 1.3, 5, 13, 0.00719905},
        {5, 64, 1.3, 6, 13, 0.0344029},
        {6, 64, 1.3, 4, 13, 0.000749531},
        {6, 64, 1.3, 5, 13, 0.00668744},
        {6, 64, 1.3, 6, 13, 0.0377829},
        {7, 64, 1.3, 4, 13, 0.00109089},
        {7, 64, 1.3, 5, 13, 0.00699069},
        {7, 64, 1.3, 6, 13, 0.0767591},
        {8, 64, 1.3, 4, 13, 0.000842315},
        {8, 64, 1.3, 5, 13, 0.0160629},
        {8, 64, 1.3, 6, 13, 0.0511619},
    };
}

int main(int argc, char * argv[])
{
    if (argc == 1) {
        return test_wafom();
    } else {
        return output_wafom(argc, argv);
    }
}

namespace {
    int test_wafom()
    {
        static lookup_t table;
        bool error = false;
        for (int i = 0; i < DATA_NUM; i++) {
            int id = data[i].id;
            int n = data[i].n;
            double c = data[i].c;
            int s = data[i].s;
            int m = data[i].m;
            double w = data[i].wafom;
            make_table(n, table, c);
            digital_net_id dnid = static_cast<digital_net_id>(id);
            DigitalNet<uint64_t> dn(dnid, s, m);
            dn.pointInitialize();
            double wafom1 = calc_wafom(dn, table);
            dn.pointInitialize();
            double wafom2 = calc_wafom(dn, c);
            if (abs(wafom1 - w) > FLT_EPSILON) {
                error = true;
                cout << "name = " << dn.getName() << endl;
                cout << "id = " << id << endl;
                cout << "n = " << n << endl;
                cout << "c = " << c << endl;
                cout << "s = " << s << endl;
                cout << "m = " << m << endl;
                cout << "wafom lookup = " << wafom1 << endl;
                cout << "expected wafom = " << w << endl;
            }
            if (abs(wafom2 - w) > FLT_EPSILON) {
                error = true;
                cout << "name = " << dn.getName() << endl;
                cout << "id = " << id << endl;
                cout << "n = " << n << endl;
                cout << "c = " << c << endl;
                cout << "s = " << s << endl;
                cout << "m = " << m << endl;
                cout << "wafom        = " << wafom2 << endl;
                cout << "expected wafom = " << w << endl;
            }
            if (error) {
                return -1;
            }
        }
        return 0;
    }

    int output_wafom(int argc, char * argv[])
    {
        if (argc <= 3) {
            cout << argv[0] << " id n c" << endl;
            cout << "id = 100 means from stdin" << endl;
            return -1;
        }
        errno = 0;
        int id = strtol(argv[1], NULL, 10);
        int n = strtol(argv[2], NULL, 10);
        double c = strtod(argv[3], NULL);
        if (errno) {
            cout << "id n and c should be a number" << endl;
        }
        static lookup_t table;
        make_table(n, table, c);
        double wafom1;
        double wafom2;
        cout << "id = " << id << endl;
        cout << "n = " << n << endl;
        cout << "c = " << c << endl;
        //cout << scientific << setprecision(18);
        cout << fixed << setprecision(18);
        if (id == 100) {
            cout << "id = 100; from stdin" << endl;
            if (n == 64) {
                DigitalNet<uint64_t> dn(cin);
                wafom1 = calc_wafom(dn, table);
                dn.pointInitialize();
                wafom2 = calc_wafom(dn, c);
                cout << "wafom        = " << wafom2 << endl;
                cout << "wafom lookup = " << wafom1 << endl;
                return 0;
            } else {
                DigitalNet<uint32_t> dn(cin);
                wafom1 = calc_wafom(dn, table);
                dn.pointInitialize();
                wafom2 = calc_wafom(dn, c);
                cout << "wafom        = " << wafom2 << endl;
                cout << "wafom lookup = " << wafom1 << endl;
                return 0;
            }
        }
        if (argc <= 5) {
            cout << argv[0] << " id n c s m" << endl;
            return -1;
        }
        uint32_t s = strtoul(argv[4], NULL, 10);
        uint32_t m = strtoul(argv[5], NULL, 10);
        cout << "s = " << s << endl;
        cout << "m = " << m << endl;
        digital_net_id dnid = static_cast<digital_net_id>(id);
        if (n == 64) {
            DigitalNet<uint64_t> dn(dnid, s, m);
            wafom1 = calc_wafom(dn, table);
            dn.pointInitialize();
            wafom2 = calc_wafom(dn, c);
            cout << "name = " << dn.getName() << endl;
            cout << "wafom        = " << wafom2 << endl;
            cout << "wafom lookup = " << wafom1 << endl;
            return 0;
        } else {
            DigitalNet<uint32_t> dn(dnid, s, m);
            wafom1 = calc_wafom(dn, table);
            dn.pointInitialize();
            wafom2 = calc_wafom(dn, c);
            cout << "name = " << dn.getName() << endl;
            cout << "wafom        = " << wafom2 << endl;
            cout << "wafom lookup = " << wafom1 << endl;
            return 0;
        }
        return 0;
    }
}
