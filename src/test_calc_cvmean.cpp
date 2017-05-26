#include "cvmean.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cerrno>
#include <cmath>
#include <cfloat>

using namespace std;

namespace {
    int test_cvmean();
    int output_cvmean(int argc, char * argv[]);
    struct check {
        int s;
        double c;
    };
    check data[10] = {
        {1, 1.7058},
        {4, -0.41728},
        {10, -1.8942},
        {22, -3.0862},
        {30, -3.5455},
        {32, -3.6406},
        {40, -3.968594074249267578},
        {100, -5.304970741271972656},
        {200, -6.309754848480224609},
        {300, -6.896309852600097656}
    };
}

int main(int argc, char * argv[])
{
    if (argc == 1) {
        return test_cvmean();
    } else {
        return output_cvmean(argc, argv);
    }
}

namespace {
    int test_cvmean()
    {
        for (int i = 0; i < 10; i++) {
            int s = data[i].s;
            double d = data[i].c;
            double c = calc_c_for_cvmean(s, 64);
            if (abs(c - d) >= FLT_EPSILON) {
                cout << "s = " << s << endl;
                cout << "c = " << c << endl;
                cout << "d = " << d << endl;
                return -1;
            }
        }
        return 0;
    }

    int output_cvmean(int argc, char * argv[])
    {
        if (argc <= 2) {
            cout << argv[0] << " s n" << endl;
            return -1;
        }
        errno = 0;
        int s = strtol(argv[1], NULL, 10);
        int n = strtol(argv[2], NULL, 10);
        if (errno) {
            cout << "s and n should be number" << endl;
        }
        cout << fixed << setprecision(18);
        cout << "s = " << s << endl;
        cout << "n = " << n << endl;
        double c = calc_c_for_cvmean(s, n);
        cout << "c for cvmean = " << c << endl;
        return 0;
    }
}
