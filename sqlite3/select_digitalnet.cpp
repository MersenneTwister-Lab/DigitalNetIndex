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

struct DIGITAL_NET_T {
    string name;
    int bit;
    int s;
    int m;
    double wafom;
    int tvalue;
    string base;
};

typedef struct DIGITAL_NET_T digital_net_t;

int selectdb(sqlite3 *db, const string& name, int bit, int s, int m,
             digital_net_t& dn);
void printnet(digital_net_t& dn);

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
    if (errno) {
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
    r = selectdb(db, netname, bit, s, m, dn);
    if (r == 0) {
        printnet(dn);
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

int selectdb(sqlite3 *db, const string& name, int bit, int s, int m,
             digital_net_t& dn)
{
    dn.name = name;
    dn.bit = bit;
    dn.s = s;
    dn.m = m;
    // prepare sql
    int r = 0;
    string strsql = "select wafom, tvalue, data";
    strsql += " from digitalnet ";
    strsql += " where netname = ? "; // 1
    strsql += "and bitsize = ? ";    // 2
    strsql += "and dimr = ? ";       // 3
    strsql += "and dimf2 = (select min(dimf2) from digitalnet ";
    strsql += "where netname = ? "; // 4
    strsql += "and bitsize = ? ";   // 5
    strsql += "and dimr = ? ";      // 6
    strsql += "and dimf2 >= ?);";   // 7
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
        r = sqlite3_bind_text(select_sql, 1, name.c_str(),
                              -1, SQLITE_STATIC);
        if (r != SQLITE_OK) {
            cout << "error bind netname r = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            return r;
        }
        DEBUG_STEP(4.33);
        //cout << "insertdb step 5" << endl;
        r = sqlite3_bind_int(select_sql, 2, bit);
        if (r != SQLITE_OK) {
            cout << "error bind bitsize r = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            return r;
        }
        //cout << "insertdb step 6" << endl;
        r = sqlite3_bind_int(select_sql, 3, s);
        if (r != SQLITE_OK) {
            cout << "error bind dimr r = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            return r;
        }
        DEBUG_STEP(4.34);
        //cout << "insertdb step 7" << endl;
        r = sqlite3_bind_text(select_sql, 4, name.c_str(),
                              -1, SQLITE_STATIC);
        if (r != SQLITE_OK) {
            cout << "error bind netname r = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            return r;
        }
        DEBUG_STEP(4.35);
        //cout << "insertdb step 5" << endl;
        r = sqlite3_bind_int(select_sql, 5, bit);
        if (r != SQLITE_OK) {
            cout << "error bind bitsize r = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            return r;
        }
        //cout << "insertdb step 6" << endl;
        r = sqlite3_bind_int(select_sql, 6, s);
        if (r != SQLITE_OK) {
            cout << "error bind dimr r = " << dec << r << endl;
            cout << sqlite3_errmsg(db) << endl;
            return r;
        }
        DEBUG_STEP(4.36);
        r = sqlite3_bind_int(select_sql, 7, m);
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
        dn.wafom = sqlite3_column_double(select_sql, 0);
        dn.tvalue = sqlite3_column_int(select_sql, 1);
        char * tmp = (char *)sqlite3_column_text(select_sql, 2);
        stringstream ss;
        stringstream ss2;
        ss << tmp;
        for (int i = 0; i < s * m; i++) {
            uint64_t t;
            ss >> t;
            ss2 << t << " ";
        }
        getline(ss2, dn.base);
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

void printnet(digital_net_t& dn)
{
    cout << "digital net" << endl;
    cout << "name:" << dn.name << endl;
    cout << "bit:" << dn.bit << endl;
    cout << "s:" << dn.s << endl;
    cout << "m:" << dn.m << endl;
    cout << "waform:" << scientific << setprecision(20) << dn.wafom << endl;
    cout << "tvalue:" << dn.tvalue << endl;
    cout << "data:" << dn.base << endl;
#if 0
    for (int k = 0; k < dn.m; k++) {
        for (int i = 0; i < dn.s; i++) {
            cout << dn.base[k * dn.s + i] << " ";
        }
        cout << endl;
    }
#endif
}
