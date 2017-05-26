#include "grayindex.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

static int check_table[20] = {
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2
};

static int test_grayindex()
{
    GrayIndex gray;
    for (int i = 0; i < 20; i++) {
        int idx = gray.index();
        gray.next();
        if (idx != check_table[i]) {
            cout << "gray.index() = " << idx << " check_table[" << i
                 << "] = " << check_table[i] << endl;
            return -1;
        }
    }
    return 0;
}

static int output_grayindex()
{
    GrayIndex gray;
    for (int i = 0; i < 20; i++) {
        cout << gray.index() << endl;
        gray.next();
    }
    return 0;
}

int main(int argc, char **)
{
    if (argc == 1) {
        return test_grayindex();
    } else {
        return output_grayindex();
    }
}
