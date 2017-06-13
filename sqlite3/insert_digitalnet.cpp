#include <inttypes.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <sqlite3.h>

using namespace std;
//#define DEBUG_STEP(x) do { cerr << "debug step " << x << endl;} while(0)
#define DEBUG_STEP(x)

int import(sqlite3 *db, sqlite3_stmt* insert_sql, int id,
           const string& fname);
int insertdb(sqlite3 *db, sqlite3_stmt* insert_sql, int id,
             int bit, int s, int m,
             double wafom, int tvalue, string& base);
void message(string pgm, string dbname);
int get_id(sqlite3 * db, string& name);

int main(int argc, char * argv[])
{
    if (argc < 4) {
        if (argc >= 2) {
            message(argv[0], argv[1]);
        } else {
            message(argv[0], "");
        }
        return 1;
    }
    string dbname = argv[1];
    string dataname = argv[2];
    int files = argc - 3;
    string fname[files];
    for (int i = 0; i < files; i++) {
        fname[i] = argv[3 + i];
    }
    // db open
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
    int id = get_id(db, dataname);
    if (id < 0) {
        message(argv[0], argv[1]);
        return -1;
    }
    string strsql = "insert into digitalnet (";
    strsql += "id, bitsize, dimr, dimf2, wafom, tvalue, data) values (";
    strsql += "?, ?, ?, ?, ?, ?, ?);";
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
    for (int i = 0; i < files; i++) {
        r = import(db, insert_sql, id, fname[i]);
        if (r != 0) {
            break;
        }
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
#if 0
    if (r != SQLITE_OK) {
        cout << "error finalize r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
    }
#endif
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


int import(sqlite3 *db, sqlite3_stmt* insert_sql, int id,
           const string& fname)
{
    DEBUG_STEP(4.1);
    int r = 0;
    ifstream dn(fname);
    if (!dn) {
        cout << fname << " does not exist." << endl;
        return -1;
    }
    while (dn) {
        uint32_t bit;
        uint32_t s;
        uint32_t m;
        dn >> bit;
        dn >> s;
        dn >> m;
        if (!dn.good()) {
            break;
        }
        string base;
        uint64_t tmp;
        stringstream ss;
        DEBUG_STEP(4.2);
#if defined(DEBUG)
        cout << "s = " << s << endl;
        cout << "m = " << m << endl;
#endif
        //DEBUG_STEP(m);
        //DEBUG_STEP(s);
        for (uint32_t k = 0; k < m; k++) {
            for (uint32_t i = 0; i < s; i++) {
                //DEBUG_STEP((k * s + i));
                dn >> tmp;
                ss << tmp << " ";
            }
        }
        getline(ss, base);
        DEBUG_STEP(4.21);
#if defined(DEBUG)
        cout << "base = " << base << endl;
#endif
        double wafom = -1.0;
        int tvalue = -1;
        string str;
        ss.str("");
        ss.clear(stringstream::goodbit);
        getline(dn, str);
        if (!str.empty()) {
            ss << str;
            if (ss) {
                ss >> wafom;
            }
            if (ss) {
                ss >> tvalue;
            }
        }
        ss.str("");
        ss.clear(stringstream::goodbit);
        DEBUG_STEP(4.3);
        r = insertdb(db, insert_sql, id, bit, s, m, wafom, tvalue, base);
        DEBUG_STEP(4.4);
        cout << "s,m:" << dec << s << "," << m;
        if (r != 0) {
            cout << " fail" << endl;
            //return -10;
        } else {
            cout << " success" << endl;
        }
    }
    return 0;
}

int insertdb(sqlite3 *db, sqlite3_stmt* insert_sql,
             int id,
             int bit, int s, int m,
             double wafom, int tvalue, string& base)
{
#if defined(DEBUG)
    cout << "id = " << id << endl;
    cout << "bit = " << bit << endl;
    cout << "s = " << s << endl;
    cout << "m = " << m << endl;
    cout << "wafom = " << wafom << endl;
    cout << "tvalue = " << tvalue << endl;
    cout << "base = " << base << endl;
#endif
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
    r = sqlite3_bind_int(insert_sql, 1, id);
    if (r != SQLITE_OK) {
        cout << "error bind dataname r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return r;
    }
    DEBUG_STEP(4.33);
    //cout << "insertdb step 5" << endl;
    r = sqlite3_bind_int(insert_sql, 2, bit);
    if (r != SQLITE_OK) {
        cout << "error bind n r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return r;
    }
    //cout << "insertdb step 6" << endl;
    r = sqlite3_bind_int(insert_sql, 3, s);
    if (r != SQLITE_OK) {
        cout << "error bind s r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return r;
    }
    DEBUG_STEP(4.34);
    //cout << "insertdb step 7" << endl;
    r = sqlite3_bind_int(insert_sql, 4, m);
    if (r != SQLITE_OK) {
        cout << "error bind m r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return r;
    }
    //cout << "insertdb step 8" << endl;
    if (tvalue >= 0) {
        r = sqlite3_bind_double(insert_sql, 5, wafom);
    } else {
        r = sqlite3_bind_null(insert_sql, 5);
    }
    if (r != SQLITE_OK) {
        cout << "error bind wafom r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return r;
    }
    DEBUG_STEP(4.35);
    //cout << "insertdb step 9" << endl;
    if (tvalue >= 0) {
        r = sqlite3_bind_int(insert_sql, 6, tvalue);
    } else {
        r = sqlite3_bind_null(insert_sql, 6);
    }
    if (r != SQLITE_OK) {
        cout << "error bind tvalue r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return r;
    }
    DEBUG_STEP(4.36);
    //cout << "insertdb step 10" << endl;
    r = sqlite3_bind_text(insert_sql, 7, base.c_str(),
                          base.length(), SQLITE_TRANSIENT);
    if (r != SQLITE_OK) {
        cout << "error bind data r = " << dec << r << endl;
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

void message(string pgm, string dbname)
{
    cout << pgm << " dbname dataname fname fname ..." << endl;
    if (dbname.empty()) {
        return;
    }
    sqlite3 *db;
    int r = sqlite3_open_v2(dbname.c_str(), &db, SQLITE_OPEN_READONLY, NULL);
    if (r != SQLITE_OK) {
        cout << "sqlite3_open error code = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return;
    }
    string strsql = "select name, longname from digitalnet_id;";
    sqlite3_stmt* select_sql = NULL;
    r = sqlite3_prepare_v2(db, strsql.c_str(), -1, &select_sql, NULL);
    if (r != SQLITE_OK) {
        cout << "sqlite3_prepare error code = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        r = sqlite3_close_v2(db);
        return;
    }
    cout << "dataname is one of:" << endl;
    for (;;) {
        r = sqlite3_step(select_sql);
        if (r != SQLITE_ROW) {
            break;
        }
        char * name = (char *)sqlite3_column_text(select_sql, 0);
        char * longname = (char *)sqlite3_column_text(select_sql, 1);
        cout << name << ":" << longname << endl;
    }
}

int get_id(sqlite3 * db, string& name)
{
    string strsql = "select id from digitalnet_id where name = ?;";
    sqlite3_stmt* select_sql = NULL;
    int r = sqlite3_prepare_v2(db, strsql.c_str(), -1, &select_sql, NULL);
    if (r != SQLITE_OK) {
        cout << "sqlite3_prepare error code = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        r = sqlite3_close_v2(db);
        return -2;
    }
    r = sqlite3_bind_text(select_sql, 1, name.c_str(),
                          name.length(), SQLITE_TRANSIENT);
    if (r != SQLITE_OK) {
        cout << "error bind name r = " << dec << r << endl;
        cout << sqlite3_errmsg(db) << endl;
        return r;
    }
    r = sqlite3_step(select_sql);
    if ( r != SQLITE_ROW) {
        return -1;
    }
    int id = sqlite3_column_int(select_sql, 0);
    return id;
}
