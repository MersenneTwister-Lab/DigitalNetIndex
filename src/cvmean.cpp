#include <cfloat>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include "cvmean.h"

using namespace std;

namespace {

    const int table_max = 32;
    const double cv_mean_c[33] = {
        0,                          // s = 0 dummy
        1.7058,                     // s = 1
        0.73028,
        0.069273,
        -0.41728,
        -0.78992,
        -1.0880,
        -1.3354,
        -1.5466,
        -1.7308,
        -1.8942,                    // s = 10
        -2.0409,
        -2.1741,
        -2.2960,
        -2.4084,
        -2.5127,
        -2.6099,
        -2.7010,
        -2.7867,
        -2.8676,
        -2.9442,                    // s = 20
        -3.0169,
        -3.0862,
        -3.1522,
        -3.2154,
        -3.2759,
        -3.3340,
        -3.3899,
        -3.4436,
        -3.4954,
        -3.5455,                    // s = 30
        -3.5938,
        -3.6406                     // s = 32
    };

#if defined(DEBUG)
#include <cerrno>
#include <cstdlib>
#endif

    /**
     * c = -∞ のときのCVを計算する
     * @param m F2での次元
     * @param n ビット数 32 または 64
     * @param s Rでの次元
     */
    double calc_cv_minus_inf(int m, int n, int s)
    {
        double x = pow(2.0, m) - 1.0;
        x = x / (3.0 * s);
        x = sqrt(x);
        double t = pow(2.0, -n);
        double y = sqrt((1.0 + t) / (1.0 - t));
        return x * y;
    }

    /**
     * CVを計算する。今回はCが変わるのでキャッシュできないんだなぁ。
     * @param c
     * @param n ビット数 32 または 64
     * @param s Rでの次元
     */
    double calc_cv(int m, double c, int s, int n)
    {
        double mp = sqrt(pow(2.0, m) - 1.0);
        double p22 = pow(2.0, 2.0 * (c - 1.0));
        double p2 = pow(2.0, c - 1.0);
        double pr2 = 1.0;
        double pr1 = 1.0;

        for (int j = 1; j <= n; j++) {
            //cout << p22 << " " << p2 << " " << pr2 << " " << pr1 << endl;
#if defined(DEBUG)
            if (isnan(p22) || isnan(p2) || isnan(pr2) || isnan(pr1)) {
                cout << p22 << " " << p2 << " " << pr2 << " " << pr1 << endl;
            }
#endif
            p22 = p22 / 4.0;
            p2 = p2 / 2.0;
            pr2 *= pow(1.0 + p22, s);
            pr1 *= pow(1.0 + p2, s);
        }
        pr2 = sqrt(pr2 - 1.0);
        pr1 = pr1 - 1.0;
#if defined(DEBUG)
        if (c >= 21.0) {
            cout << "pr2 = " << pr2 << endl;
            cout << "pr1 = " << pr1 << endl;
            cout << "pr2 / pr1 = " << (pr2 / pr1) << endl;
        }
#endif
        return mp * pr2 / pr1;
    }

    int calc_c_and_cvmean(double& c, double& cvmean,
                          int s, int n)
    {
        int m = 5;
        double c_min = -10;
        if (s <= 32 && n == 64) {
            c_min = -4.0;
        }
        double c_max = 10.0;
        //double c_max =calc_c_cv_min(m, s, n);
        //double cv_max = calc_cv_minus_inf_except_m(n, s);
        double cv_max = calc_cv_minus_inf(m, n, s);
        //double cv_min = calc_c_cv_min(m, s, n, c_max);
        double cv_min = 0; // not a bug
#if defined(DEBUG)
        cout << "cv_max = " << cv_max << endl;
        cout << "cv_min = " << cv_min << endl;
        cout << "c_max = " << c_max << endl;
        cout << "c_min = " << c_min << endl;
#endif
        if (isnan(cv_max) || isnan(cv_min)) {
            return -1;
        }
        const double cv_mean = (cv_max + cv_min) / 2;
        cvmean = cv_mean;
        const double eps = FLT_EPSILON; // not a bug
        //cout << "cv_mean = " << setprecision(18) << fixed << cv_mean << endl;
        for (int i = 0; i < 1000; i++) {
            double c_tmp = (c_max + c_min) / 2.0;
            //double cv_tmp = calc_cv_except_m(c_tmp, s, n);
            double cv_tmp = calc_cv(m, c_tmp, s, n);
            if (abs(cv_tmp - cv_mean) < eps) {
#if defined(DEBUG)
                cout << "cv_tmp  = " << cv_tmp << endl;
                cout << "c for cv_mean = " << c_tmp << endl;
#endif
                c = c_tmp;
                return 0;
            }
#if defined(DEBUG)
            cout << "cv_mean = " << cv_mean << endl;
            cout << "cv_tmp = " << cv_tmp << endl;
#endif
            if (cv_tmp < cv_mean) {
                c_max = c_tmp;
#if defined(DEBUG)
                cout << "*c_max = " << c_max << endl;
                cout << "c_min = " << c_min << endl;
#endif
            } else {
                c_min = c_tmp;
#if defined(DEBUG)
                cout << "c_max = " << c_max << endl;
                cout << "*c_min = " << c_min << endl;
#endif
            }
        }
        cout << "can't calculate" << endl;
        return -1;
    }
}

double calc_c_for_cvmean(int s, int n)
{
    double c = 1.0;
    double cvmean;
    if (s <= 0) {
        //throw runtime_error("s <= 0");
        throw "s <= 0";
    }
    if (s <= table_max) {
        return cv_mean_c[s];
    } else {
        int r = calc_c_and_cvmean(c, cvmean, s, n);
        if (r != 0) {
            //throw runtime_error("calc_c_and_cvmean error");
            throw "calc_c_and_cvmean error";
        }
        return c;
    }
}
