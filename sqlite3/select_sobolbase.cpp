#include <inttypes.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cerrno>
#include <sqlite3.h>
#include <cstring>

using namespace std;
//#define DEBUG_STEP(x) do { cerr << "debug step " << x << endl;} while(0)
#define DEBUG_STEP(x)

struct digital_net_t {
    string name;
    int bit;
    int s;
    int m;
    double wafom;
    int tvalue;
    string base;
};

const int max_data = 50;

bool select_sobol_base(const std::string& path,
                       uint32_t s, uint32_t m,  uint64_t base[]);
bool read_data(sqlite3_stmt* select_sql, uint32_t data[]);
void printnet(int s, int m, uint64_t base[]);

bool select_sobol_base(const std::string& path,
                       uint32_t s, uint32_t m,  uint64_t base[])
{
    // db open
    sqlite3 *db;
    int r = 0;
    DEBUG_STEP(1);
    r = sqlite3_open_v2(path.c_str(), &db, SQLITE_OPEN_READONLY, NULL);
    if (r != SQLITE_OK) {
        cout << "sqlite3_open error code = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return false;
    }
    DEBUG_STEP(2);
    sqlite3_stmt *select_sql = NULL;
    //r = select_bind(db, &select_sql);
    string strsql = "select d, s, a, mi from sobolbase ";
    strsql += "order by d;";
    r = sqlite3_prepare_v2(db, strsql.c_str(), -1, &select_sql, NULL);
    if (r != SQLITE_OK) {
        cout << "sqlite3_prepare error code = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        r = sqlite3_close_v2(db);
        return false;
    }
    if (select_sql == NULL) {
        cout << "sqlite3_prepare null statement" << endl;
        r = sqlite3_close_v2(db);
        return false;
    }

    uint32_t D = s + 1;
    uint32_t L = m;
    //uint32_t N = UINT32_C(1) << (m - 1);
    uint32_t col = 0;
    uint64_t V[L + 1];
    for (unsigned i=1;i<=L;i++) {
        V[i] = UINT64_C(1) << (64 - i); // all m's = 1
    }
#if defined(DEBUG)
    cout << "col = " << dec << col << endl;
    for (uint32_t i = 1; i <= L; i++) {
        cout << "V[" << dec << i << "] = " << hex << V[i] << endl;
    }
#endif
    for (uint32_t i = 1; i <= L; i++) {
        base[(i - 1) * s + col] = V[i];
    }
    uint32_t data[max_data];
    for (uint32_t c = 1; c < D - 1; c++) {
        bool success = read_data(select_sql, data);
        if (! success) {
            cerr << "data format error" << endl;
            //throw runtime_error("data format error");
            return false;
        }
        col++;
        //uint32_t d_sobol = data[0];
        uint32_t s_sobol = data[0];
        uint32_t a_sobol = data[1];
        uint32_t *m_sobol = &data[1]; // index from 1
#if defined(DEBUG)
        //cout << "d = " << dec << d_sobol << endl;
        cout << "s = " << dec << s_sobol << endl;
        cout << "a = " << dec << a_sobol << endl;
        cout << "L = " << dec << L << endl;
#endif
        if (L <= s_sobol) {
            for (unsigned i=1;i<=L;i++) {
                V[i] = static_cast<uint64_t>(m_sobol[i]) << (64 - i);
            }
        } else {
            for (unsigned i = 1; i <= s_sobol; i++) {
                V[i] = static_cast<uint64_t>(m_sobol[i]) << (64 - i);
            }
            for (unsigned i = s_sobol + 1; i <= L; i++) {
                V[i] = V[i - s_sobol] ^ (V[i - s_sobol] >> s_sobol);
                // s
                for (unsigned k=1; k <= s_sobol-1; k++) {
                    V[i] ^= (((a_sobol >> (s_sobol-1-k)) & 1) * V[i-k]);
                }
            }
        }
#if defined(DEBUG)
        cout << "col = " << dec << col << endl;
        for (uint32_t i = 1; i <= L; i++) {
            cout << "V[" << dec << i << "] = " << hex << V[i] << endl;
        }
#endif
        for (uint32_t i = 1; i <= L; i++) {
            base[(i - 1) * s + col] = V[i];
        }
    }
    r = sqlite3_finalize(select_sql);
    if (r != SQLITE_OK) {
        cout << "error finalize r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
    }
    sqlite3_close_v2(db);
    if (r != SQLITE_OK) {
        return false;
    }
    for (uint32_t i = L - 1; i >= 1; i--) {
        for (uint32_t j = 0; j < s; j++) {
            base[i * s + j] ^= base[(i - 1) * s + j];
        }
    }
    return true;
}

bool read_data(sqlite3_stmt* select_sql, uint32_t data[])
{
    DEBUG_STEP(4.37);
    int r = sqlite3_step(select_sql);
    if (r != SQLITE_ROW) {
        return false;
    }
    //sobolbase.d = sqlite3_column_int(select_sql, 0); // d
    data[0] = sqlite3_column_int(select_sql, 1); // s
    data[1] = sqlite3_column_int(select_sql, 2); // a
    char * tmp = (char *)sqlite3_column_text(select_sql, 3); // mi
    stringstream ss;
    ss << tmp;
    for (int i = 2; ss.good(); i++) {
        ss >> data[i];
#if defined(DEBUG)
        cout << "data[" << dec << i << "] = " << data[i] << endl;
#endif
    }
    DEBUG_STEP(4.38);
    return true;
}

int main(int argc, char * argv[])
{
    if (argc < 6) {
        cout << argv[0] << " dbname digitalnetname bit s m" << endl;
        return 1;
    }
    string dbname = argv[1];
    string netname = argv[2];
    int bit;
    int s;
    int m;
    errno = 0;
    bit = strtol(argv[3], NULL, 10);
    if (errno || (bit != 32 && bit != 64)) {
        cout << "bit must be number 64 or 32" << endl;
        return -1;
    }
    s = strtol(argv[4], NULL, 10);
    if (errno) {
        cout << "s must be a number" << endl;
        return -1;
    }
    m = strtol(argv[5], NULL, 10);
    if (errno) {
        cout << "m must be a number" << endl;
        return -1;
    }
    uint64_t base[s * m];
    bool success = select_sobol_base(dbname, s, m, base);
    if (success) {
        printnet(s, m, base);
    }
    return 0;
}

void printnet(int s, int m, uint64_t base[])
{
    cout << "digital net" << dec << endl;
    cout << "s:" << s << endl;
    cout << "m:" << m << endl;
    cout << "data:" << endl;
    for (int i = 0; i < s * m; i++) {
        cout << base[i] << " ";
    }
    cout << endl;
}
