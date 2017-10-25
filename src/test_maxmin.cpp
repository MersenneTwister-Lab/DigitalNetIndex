#include "DigitalNet.hpp"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cerrno>

using namespace std;
using namespace DigitalNetNS;

namespace {
    int test_maxmin();
    int output_maxmin(int argc, char * argv[]);
    struct check {
        int id;
        int s_min;
        int s_max;
        int m_min;
        int m_max;
    };
    check data[8] = {
        {0, 4, 32, 32, 32},
        {1, 2, 21201, 8, 31},
        {3, 4, 10, 10, 18},
        {4, 4, 100, 10, 15},
        {5, 2, 10600, 8, 31},
        {6, 2, 7067, 8, 31},
        {7, 2, 5300, 8, 31},
        {8, 2, 4240, 8, 31}
    };
}

int main(int argc, char * argv[])
{
    if (argc == 1) {
        return test_maxmin();
    } else {
        return output_maxmin(argc, argv);
    }
}

namespace {
    int test_maxmin()
    {
        for (int i = 0; i < 8; i++) {
            digital_net_id id = static_cast<digital_net_id>(data[i].id);
            int s_min = getSMin(id);
            int s_max = getSMax(id);
            if (s_min > data[i].s_min || s_max < data[i].s_max) {
                cout << "name = " << getDigitalNetName(data[i].id) << endl;
                cout << "id = " << data[i].id << endl;
                cout << "s_min = " << s_min << " should be "
                     << data[i].s_min << endl;
                cout << "s_max = " << s_max << " should be "
                     << data[i].s_max << endl;
                return -1;
            }
            int m_min = getMMin(id, s_max);
            int m_max = getMMax(id, s_max);
            if (m_min > data[i].m_min || m_max < data[i].m_max) {
                cout << "name = " << getDigitalNetName(data[i].id) << endl;
                cout << "id = " << data[i].id << endl;
                cout << "s = " << s_max << endl;
                cout << "m_min = " << m_min << " should <= "
                     << data[i].m_min << endl;
                cout << "m_max = " << m_max << " should >= "
                     << data[i].m_max << endl;
                return -1;
            }
        }
        return 0;
    }

    int output_maxmin(int, char **)
    {
        digital_net_id ids[] = {NX, SOBOL, NXLW, SOLW,
                                ISOBOL_A2, ISOBOL_A3, ISOBOL_A4, ISOBOL_A5};
        for (int i = 0; i < 8; i++) {
            cout << getDigitalNetName(static_cast<int>(ids[i])) << endl;
            int min_s = getSMin(ids[i]);
            int max_s = getSMax(ids[i]);
            cout << "min s = " << min_s << endl;
            cout << "max s = " << max_s << endl;
            cout << "min m for " << max_s << " = " << getMMin(ids[i], max_s)
                 << endl;
            cout << "max m for " << max_s << " = " << getMMax(ids[i], max_s)
                 << endl;
        }
        return 0;
    }
}
