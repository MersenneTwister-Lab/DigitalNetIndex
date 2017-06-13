#include <inttypes.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <sqlite3.h>

using namespace std;
//define DEBUG_STEP(x) do { cerr << "debug step " << x << endl;} while(0)
#define DEBUG_STEP(x)

int import(sqlite3 *db, sqlite3_stmt* insert_sql, const string& fname);
int insertdb(sqlite3 *db, sqlite3_stmt* insert_sql,
             int d, int s, int a, const string& mi);

int main(int argc, char * argv[])
{
    if (argc <= 2)  {
        cout << argv[0] << " dbname fname " << endl;
        return 1;
    }
    string dbname = argv[1];
    string fname = argv[2];
    sqlite3 *db;
    int r = 0;
    DEBUG_STEP(1);
    r = sqlite3_open_v2(dbname.c_str(), &db, SQLITE_OPEN_READWRITE, NULL);
    if (r != SQLITE_OK) {
        cout << "sqlite3_open error code = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return -1;
    }

    DEBUG_STEP(2);
    // prepare sql
    string strsql = "insert into sobolbase (";
    strsql += "d, s, a, mi) values (";
    strsql += "?, ?, ?, ?);";
    sqlite3_stmt* insert_sql = NULL;
    r = sqlite3_prepare_v2(db, strsql.c_str(), -1, &insert_sql, NULL);
    if (r != SQLITE_OK) {
        cout << "sqlite3_prepare error code = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        r = sqlite3_close_v2( db );
        return -2;
    }
    DEBUG_STEP(3);

    // transaction start
    r = sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);
    DEBUG_STEP(4);
    if (r != SQLITE_OK) {
        cout << "error begin r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
    }
    r = import(db, insert_sql, fname);
    if (r != 0) {
        return r;
    }
    DEBUG_STEP(5);
    // transaction end
    r = sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    if (r != SQLITE_OK) {
        cout << "error commit r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
    }
    DEBUG_STEP(6);
    // release sql
    r = sqlite3_finalize(insert_sql);
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


int import(sqlite3 *db, sqlite3_stmt* insert_sql, const string& fname)
{
    DEBUG_STEP(4.1);
    int r = 0;
    ifstream sbase(fname);
    if (!sbase) {
        cout << fname << " does not exist." << endl;
        return -1;
    }
    while (sbase) {
        uint32_t d;
        uint32_t s;
        uint32_t a;
        string mi;
        sbase >> d;
        sbase >> s;
        sbase >> a;
        getline(sbase, mi);
#if defined(DEBUG)
        cout << "d = " << d << endl;
        cout << "s = " << s << endl;
        cout << "a = " << a << endl;
        cout << "mi = " << mi << endl;
#endif
        if (!sbase.good()) {
            break;
        }
        DEBUG_STEP(4.3);
        r = insertdb(db, insert_sql, d, s, a, mi);
        DEBUG_STEP(4.4);
        cout << "d,s,a:" << dec << d << "," << s << "," << a;
        if (r != 0) {
            cout << " fail" << endl;
            return -10;
        } else {
            cout << " success" << endl;
        }
    }
    return 0;
}

int insertdb(sqlite3 *db, sqlite3_stmt* insert_sql,
             int d, int s, int a, const string& mi)
{
    DEBUG_STEP(4.31);
    int r = 0;
    r = sqlite3_reset(insert_sql);
    if (r != SQLITE_OK) {
        cout << "error reset r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return r;
    }
    DEBUG_STEP(4.32);
    //cout << "insertdb step 4" << endl;
    r = sqlite3_bind_int(insert_sql, 1, d);
    if (r != SQLITE_OK) {
        cout << "error bind d r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return r;
    }
    DEBUG_STEP(4.33);
    //cout << "insertdb step 5" << endl;
    r = sqlite3_bind_int(insert_sql, 2, s);
    if (r != SQLITE_OK) {
        cout << "error bind s r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return r;
    }
    //cout << "insertdb step 6" << endl;
    r = sqlite3_bind_int(insert_sql, 3, a);
    if (r != SQLITE_OK) {
        cout << "error bind a r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return r;
    }
    DEBUG_STEP(4.34);
    //cout << "insertdb step 7" << endl;
    r = sqlite3_bind_text(insert_sql, 4, mi.c_str(),
                          -1, SQLITE_STATIC);
    if (r != SQLITE_OK) {
        cout << "error bind mi r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return r;
    }
    DEBUG_STEP(4.37);
    //cout << "insertdb step 11" << endl;
    r = sqlite3_step(insert_sql);
    if (r != SQLITE_DONE) {
        cout << "error step r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return r;
    }
    DEBUG_STEP(4.38);
    r = sqlite3_clear_bindings(insert_sql);
    if (r != SQLITE_OK) {
        cout << "error clear bindigs r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return r;
    }
    return r;
}
