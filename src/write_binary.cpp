#include "binary.h"
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
//#define DEBUG_STEP(x) do { cerr << "debug step " << x << endl;} while(0)
#define DEBUG_STEP(x)

namespace {
    struct digital_net_t {
        string name;
        int bit;
        int s;
        int m;
        double wafom;
        int tvalue;
        string base;
    };

    int selectdb(sqlite3 *db, const string& name, int bit, int s, int m,
                 digital_net_t& dn);
    int write_file(sqlite3 *db, ostream& ofs, string& netname,
                   int bit, int maxs);
    int write_header(FILE * wdfp, digital_net_header_t header[],
                     size_t size, bool need_rewind);
    int write_data(FILE * wdfp,
                   digital_net_header_t * header,
                   const digital_net_data_t& dn,
                   const uint64_t data[]);

    void printnet(digital_net_t& dn);
}

int main(int argc, char * argv[])
{
    if (argc < 6) {
        cout << argv[0] << " dbname digitalnetname filename bit maxs" << endl;
        return 1;
    }
    string dbname = argv[1];
    string netname = argv[2];
    string filename = argv[3];
    int bit;
    int maxs;
    int m;
    errno = 0;
    bit = strtol(argv[4], NULL, 10);
    if (errno) {
        cout << "bit must be number 64 or 32" << endl;
        return -1;
    }
    maxs = strtol(argv[5], NULL, 10);
    if (errno) {
        cout << "maxs must be a number" << endl;
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
    ofstream ofs(filename, ios::binary);
    if (!ofs) {
    }
    DEBUG_STEP(2);
    r = write_file(db, ofs, netname, bit, maxs);
    if (r == 0) {
        printnet(dn);
    }
    ofs.close();
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

namespace {
    int write_file(sqlite3 *db, ostream& ofs, string& netname,
                   int bit, int maxs)
    {
        digital_net_t dn;
        dn.name = name;
        dn.bit = bit;
        dn.s = s;
        dn.m = m;
        // prepare sql
        int r = 0;
        string strsql = "select dimr, dimf2, wafom, tvalue, data";
        strsql += " from digitalnet ";
        strsql += " where netname = ? "; // 1
        strsql += "  and bitsize = ? ";    // 2
        strsql += "  and dimr <= ? ";       // 3
        strsql += " order by dimr, dimf2;";
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
            r = sqlite3_bind_text(select_sql, 1, netname.c_str(),
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
            DEBUG_STEP(4.37);
            r = sqlite3_step(select_sql);
            while (r == SQLITE_ROW) {
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
                char * tmp = (char *)sqlite3_column_text(select_sql, 2);
                stringstream ss;
                stringstream ss2;
                ss << tmp;
                // ここで読み込む
                for (int i = 0; i < s * m; i++) {
                    uint64_t t;
                    ss >> t;
                    ss2 << t << " ";
                }
                getline(ss2, dn.base);
                // 次の行の読み出し
                r = sqlite3_step(select_sql);
            }
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

    int write_header(FILE * wdfp, digital_net_header_t header[],
                     size_t size, bool need_rewind)
    {
        uint64_t magic = MAGIC;
        //uint64_t magic = 0;
        if (need_rewind) {
            rewind(wdfp);
        }
        //cout << "DEBUG 1 ftell in write header = " << dec << ftell(wdfp) << endl;
        size_t cnt = 0;
        cnt = fwrite(&magic, sizeof(uint64_t), 1, wdfp);
        if (cnt != 1) {
            cout << "fwrite magic error" << endl;
            return -1;
        }
        //cout << "DEBUG 2 ftell in write header = " << dec << ftell(wdfp) << endl;
        cnt = fwrite(header, sizeof(digital_net_header_t), size, wdfp);
        if (cnt != size) {
            cout << "fwrite error cnt = " << dec << cnt << endl;
            return -1;
        }
        //cout << "DEBUG 3 ftell in write header = " << dec << ftell(wdfp) << endl;
        return 0;
    }

    int write_data(FILE * wdfp,
                   digital_net_header_t * header,
                   const digital_net_data_t& dn,
                   const uint64_t data[])
    {
        DEBUG_STEP(3);
        int32_t pos = ftell(wdfp);
        if (pos < 0) {
            cout << "ftell fail pos = " << dec << pos << endl;
            return -1;
        }
        header->s = dn.s;
        header->m = dn.m;
        header->pos = pos;
        size_t count = fwrite(&dn, sizeof(digital_net_data_t), 1, wdfp);
        if (count != 1) {
            cout << "fail to write dn struct" << endl;
            return -1;
        }
        size_t size = dn.s * dn.m;
        count = fwrite(data, sizeof(uint64_t), size, wdfp);
        if (count != size) {
            cout << "fail to write dn data count = " << dec << count
                 << " size = " << size << endl;
            return -1;
        }
        return 0;
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
}
