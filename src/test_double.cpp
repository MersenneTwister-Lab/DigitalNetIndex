#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

int main(int argc, char * argv[])
{
    uint64_t max = UINT64_C(0xffffffffffffffff);
    static const double eps = exp2(-65);
    cout << setprecision(18);
    for (int i = 0; i < 15; i++) {
        uint64_t tmp = max >> i;
        double factor = exp2(-(64-i));
        double d = static_cast<double>(tmp) * factor + eps;
        cout << i << ":" << d << endl;
    }
    return 0;
}
