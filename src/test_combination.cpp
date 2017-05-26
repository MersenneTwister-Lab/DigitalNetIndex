#include "combination.hpp"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cerrno>

using namespace std;
static int test_combination();
static int output_combination(int argc, char * argv[]);
static int check_table[10][3] = {
    {0, 0, 3},
    {0, 1, 2},
    {0, 2, 1},
    {0, 3, 0},
    {1, 0, 2},
    {1, 1, 1},
    {1, 2, 0},
    {2, 0, 1},
    {2, 1, 0},
    {3, 0, 0},
};

int main(int argc, char * argv[])
{
    if (argc == 1) {
        return test_combination();
    } else {
        return output_combination(argc, argv);
    }
}

static int test_combination()
{
    int size = 3;
    int init = 3;
    Combination combi(size);
    combi.reset(init);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 3; j++) {
            int tmp = combi[j];
            if (tmp != check_table[i][j]) {
                cout << "check fail combi[" << j << "] = " << tmp
                     << " check_talbe[" << i << "][" << j << "] = "
                     << check_table[i][j] << endl;
                return -1;
            }
        }
        bool final = combi.next();
        if (final) {
            break;
        }
    }
    return 0;
}

static int output_combination(int argc, char * argv[])
{
    if (argc <= 2) {
        cout << argv[0] << " size init" << endl;
        return -1;
    }
    errno = 0;
    int size = strtol(argv[1], NULL, 10);
    int init = strtol(argv[2], NULL, 10);
    if (errno) {
        cout << "size and init must be numbers" << endl;
        return -1;
    }
    Combination comb(size);
    comb.reset(init);
    do {
        for (int i = 0; i < size; i++) {
            cout << comb[i] << " ";
        }
        cout << endl;
    } while (comb.next());
    return 0;
}
