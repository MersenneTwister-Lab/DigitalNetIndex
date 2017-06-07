#include <inttypes.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cerrno>

using namespace std;
//#define DEBUG_STEP(x) do { cerr << "debug step " << x << endl;} while(0)
#define DEBUG_STEP(x)

int read(string& fname, int matrix[], int s, int m);
int print(int matrix[], int s, int m, int maxm);

int main(int argc, char * argv[])
{
    if (argc <= 4) {
        cout << argv[0] << " nx-fname s m flag" << endl;
        return 1;
    }
    string fname = argv[1];
    int s = strtoul(argv[2], NULL, 10);
    int m = strtoul(argv[3], NULL, 10);
    int maxm = strtoul(argv[4], NULL, 10);
    int matrix[s * m * m];
    int r = read(fname, matrix, s, m);
    if (r < 0) {
        return -1;
    }
    r = print(matrix, s, m, maxm);
    return r;
}

int read(string& fname, int matrix[], int s, int m)
{
    ifstream dn(fname);
    if (!dn) {
        cout << fname << " does not exist." << endl;
        return -1;
    }
    int ss;
    int mm;
    dn >> mm;
    dn >> ss;
    uint64_t tmp;
    int d = 64 - m;
    const uint64_t one = 1;
    if (mm != m || ss != s) {
        cout << "s = " << s << endl;
        cout << "m = " << m << endl;
        cout << "read s = " << ss << endl;
        cout << "read m = " << mm << endl;
        return -1;
    }
    for (int i = 0; i < s; i++) {
        for (int j = 0; j < m; j++) {
            if (!dn.good()) {
                return -1;
            }
            dn >> tmp;
            for (int k = 0; k < m; k++) {
                int idx = i * m * m + j * m + k;
                if (tmp & (one << (64 - 1 - k - d))) {
                    matrix[idx] = 1;
                } else {
                    matrix[idx] = 0;
                }
            }
        }
    }
    return 0;
}

int print(int matrix[], int s, int m, int maxm)
{
    //int d = 64 - m;
    cout << 64 << " ";
    cout << s << " ";
    cout << m << " ";
#if 0
    if (flag == 0) {
        for (int i = 0; i < s; i++) {
            for (int j = 0; j < m; j++) {
                uint64_t tmp = 0;
                for (int k = 0; k < m; k++) {
                    int idx = i * m * m + j * m + k;
                    tmp |= static_cast<uint64_t>(matrix[idx])
                        << (64 - 1 - k - d);
                }
                cout << dec << tmp << " ";
            }
        }
        cout << endl;
        return 0;
    }
#endif
//    if (flag == 1) {
        for (int j = 0; j < maxm; j++) {
            for (int i = 0; i < s; i++) {
                uint64_t tmp = 0;
                for (int k = 0; k < m; k++) {
                    int idx = i * m * m + k * m + j;
                    tmp |= static_cast<uint64_t>(matrix[idx])
                        << (64 - 1 - k);
                }
                cout << dec << tmp << " ";
            }
        }
        cout << endl;
        return 0;
//    }
//    return 0;
}
