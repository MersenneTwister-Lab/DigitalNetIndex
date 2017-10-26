#include "nb64.h"
#include <inttypes.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cerrno>
#include <sqlite3.h>
#include <cstring>
#include <cmath>

using namespace std;
using namespace DigitalNetNS;
//#define DEBUG_STEP(x) do { cerr << "debug step " << x << endl;} while(0)
#define DEBUG_STEP(x)

struct digital_net_t {
    int s;
    int m;
    double wafom;
    int tvalue;
};

int selectdb(sqlite3 *db, int s, int m, digital_net_t& dn, uint64_t base[]);
void printnet(digital_net_t& dn, uint64_t base[]);

int main(int argc, char * argv[])
{
    if (argc < 4) {
        cout << argv[0] << " dbname s m" << endl;
        return 1;
    }
    string dbname = argv[1];
    int s;
    int m;
    errno = 0;
    s = strtol(argv[2], NULL, 10);
    if (errno) {
        cout << "s must be a number" << endl;
        return -1;
    }
    m = strtol(argv[3], NULL, 10);
    if (errno) {
        cout << "m must be a number" << endl;
        return -1;
    }
    // db open
    sqlite3 *db;
    int r = 0;
    DEBUG_STEP(1);
    r = sqlite3_open_v2(dbname.c_str(), &db, SQLITE_OPEN_READONLY, NULL);
    if (r != SQLITE_OK) {
        cout << "sqlite3_open error code = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return -1;
    }
    DEBUG_STEP(2);
    digital_net_t dn;
    uint64_t base[s * m];
    r = selectdb(db, s, m, dn, base);
    if (r == 0) {
        printnet(dn, base);
    }
    DEBUG_STEP(6);
    // db close
    //cout << "insertdb step 15" << endl;
    DEBUG_STEP(7);
    r = sqlite3_close_v2(db);
    if (r != SQLITE_OK) {
        cout << "error in sqlite3_close code = " << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return -10;
    }
    return r;
}

int selectdb(sqlite3 *db, int s, int m, digital_net_t& dn, uint64_t base[])
{
    dn.s = s;
    dn.m = m;
    // prepare sql
    int r = 0;
    string strsql = "select wafom, tvalue, data";
    strsql += " from digitalnet ";
    strsql += " where dimr = ? ";       // 1
    strsql += "and dimf2 = (select min(dimf2) from digitalnet ";
    strsql += "where dimr = ? ";      // 2
    strsql += "and dimf2 >= ?);";   // 3
    sqlite3_stmt* select_sql = NULL;
    r = sqlite3_prepare_v2(db, strsql.c_str(), -1, &select_sql, NULL);
    if (r != SQLITE_OK) {
        cout << "sqlite3_prepare error code = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        r = sqlite3_close_v2(db);
        return -2;
    }
    if (select_sql == NULL) {
        cout << "sqlite3_prepare null statement" << endl;
        r = sqlite3_close_v2(db);
        return -3;
    }
    do {
        r = sqlite3_bind_int(select_sql, 1, s);
        if (r != SQLITE_OK) {
            cout << "error bind dimr r = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            return r;
        }
        r = sqlite3_bind_int(select_sql, 2, s);
        if (r != SQLITE_OK) {
            cout << "error bind dimr r = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            return r;
        }
        DEBUG_STEP(4.36);
        r = sqlite3_bind_int(select_sql, 3, m);
        if (r != SQLITE_OK) {
            cout << "error bind dimf2 r = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            return r;
        }
        DEBUG_STEP(4.37);
        r = sqlite3_step(select_sql);
        if (r != SQLITE_ROW) {
            cout << "not found" << endl;
            break;
        }
        if (sqlite3_column_type(select_sql, 0) == SQLITE_NULL) {
            dn.wafom = NAN;
        } else {
            dn.wafom = sqlite3_column_double(select_sql, 0);
        }
        if (sqlite3_column_type(select_sql, 1) == SQLITE_NULL) {
            dn.tvalue = -1;
        } else {
            dn.tvalue = sqlite3_column_int(select_sql, 1);
        }
        if (sqlite3_column_type(select_sql, 2) == SQLITE_NULL) {
            cout << "data is null" << endl;
            break;
        }
        unsigned int len = sqlite3_column_bytes(select_sql, 2);
        if (len != s * m * sizeof(uint64_t)) {
            cout << "blob size mismatch" << endl;
            break;
        }
        uint8_t * tmp = (uint8_t *)sqlite3_column_blob(select_sql, 2);
        nb64tohost(tmp, len, base);
    } while (false);
    // release sql
    r = sqlite3_finalize(select_sql);
#if 1
    if (r != SQLITE_OK) {
        cout << "error finalize r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
    }
#endif
    return r;
}

void printnet(digital_net_t& dn, uint64_t base[])
{
    cout << "digital net" << endl;
    cout << "s:" << dn.s << endl;
    cout << "m:" << dn.m << endl;
    cout << "waform:" << scientific << setprecision(20) << dn.wafom << endl;
    cout << "tvalue:" << dn.tvalue << endl;
    cout << "data:" << endl;
    for (int i = 0; i < dn.s * dn.m; i++) {
        cout << base[i] << " ";
    }
    cout << endl;
}
