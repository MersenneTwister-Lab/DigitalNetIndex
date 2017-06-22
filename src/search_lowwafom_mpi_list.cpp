#include "DigitalNet.hpp"
#include "linear_scramble.hpp"
#include "calc_wafom_lookup.hpp"
#include "calc_tvalue.hpp"
#include "cvmean.h"
#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cerrno>
#include <getopt.h>
#include <cmath>
#if !defined(NO_MPI)
#include <mpi.h>
#endif
#include <fcntl.h>
#include <unistd.h>

using namespace std;
using namespace DigitalNetNS;

// n は 1 から始まる数
static int gyukou(int n, int block)
{
    int b2 = block * 2;
    if (n % b2 == 0) {
        return block * (n / block - 1) + 1;
    }
    int x = n % b2;
    if (x <= block) {
        return n;
    }
    int k = n % block - 1;
    return b2 * (n / b2 + 1) - k;
}

struct calc_list_t {
    int s;
    int m;
};

static calc_list_t list[] = {
    {35, 18},
    {36, 18},
    {37, 18},
    {38, 18},
    {39, 18},
    {40, 18},
    {41, 18},
    {42, 18},
    {43, 17},
    {43, 18},
    {44, 17},
    {44, 18},
    {45, 17},
    {45, 18},
    {46, 17},
    {46, 18},
    {47, 17},
    {47, 18},
    {48, 17},
    {48, 18},
    {49, 17},
    {49, 18},
    {50, 17},
    {50, 18},
    {51, 17},
    {51, 18},
    {53, 17},
    {53, 18},
    {53, 17},
    {53, 18},
    {43, 17},
    {43, 18},
    {44, 17},
    {44, 18},
    {45, 17},
    {45, 18},
    {46, 17},
    {46, 18},
    {47, 17},
    {47, 18},
    {48, 17},
    {48, 18},
    {49, 17},
    {49, 18},
    {50, 17},
    {50, 18},
    {51, 17},
    {51, 18},
    {52, 17},
    {52, 18},
    {53, 17},
    {53, 18},
    {54, 17},
    {54, 18},
    {55, 17},
    {55, 18},
    {56, 17},
    {56, 18},
    {57, 17},
    {57, 18},
    {58, 17},
    {58, 18},
    {59, 17},
    {59, 18},
    {60, 17},
    {60, 18},
    {61, 17},
    {61, 18},
    {62, 17},
    {62, 18},
    {63, 17},
    {63, 18},
    {64, 17},
    {64, 18},
    {65, 17},
    {65, 18},
    {66, 17},
    {66, 18},
    {67, 17},
    {67, 18},
    {68, 17},
    {68, 18},
    {69, 17},
    {69, 18},
    {70, 17},
    {70, 18},
    {71, 17},
    {71, 18},
    {72, 17},
    {72, 18},
    {73, 17},
    {73, 18},
    {74, 17},
    {74, 18},
    {75, 15},
    {75, 16},
    {75, 17},
    {75, 18},
    {76, 15},
    {76, 16},
    {76, 17},
    {76, 18},
    {77, 15},
    {77, 16},
    {77, 17},
    {77, 18},
    {78, 15},
    {78, 16},
    {78, 17},
    {78, 18},
    {79, 15},
    {79, 16},
    {79, 17},
    {79, 18},
    {80, 15},
    {80, 16},
    {80, 17},
    {80, 18},
    {81, 15},
    {81, 16},
    {81, 17},
    {81, 18},
    {82, 15},
    {82, 16},
    {82, 17},
    {82, 18},
    {83, 15},
    {83, 16},
    {83, 17},
    {83, 18},
    {84, 15},
    {84, 16},
    {84, 17},
    {84, 18},
    {85, 15},
    {85, 16},
    {85, 17},
    {85, 18},
    {86, 15},
    {86, 16},
    {86, 17},
    {86, 18},
    {87, 15},
    {87, 16},
    {87, 17},
    {87, 18},
    {88, 15},
    {88, 16},
    {88, 17},
    {88, 18},
    {89, 15},
    {89, 16},
    {89, 17},
    {89, 18},
    {90, 15},
    {90, 16},
    {90, 17},
    {90, 18},
    {91, 15},
    {91, 16},
    {91, 17},
    {91, 18},
    {92, 15},
    {92, 16},
    {92, 17},
    {92, 18},
    {93, 16},
    {93, 17},
    {93, 18},
    {94, 16},
    {94, 17},
    {94, 18},
    {95, 16},
    {95, 17},
    {95, 18},
    {96, 16},
    {96, 17},
    {96, 18},
    {97, 16},
    {97, 17},
    {97, 18},
    {98, 16},
    {98, 17},
    {98, 18},
    {99, 16},
    {99, 17},
    {99, 18},
    {100, 16},
    {100, 17},
    {100, 18}
};

int main(int argc, char * argv[])
{
    int rank;
    int num_process;
#if !defined(NO_MPI)
    // MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_process);
#else
#define MPI_Finalize()
    rank = 0;
    num_process = 1;
#endif
    digital_net_id dnid = SOBOL;
    int n = 64;
    static lookup_t table;
    if (argc <= 1) {
        cerr << argv[0] << "seed" << endl;
        MPI_Finalize();
        return -1;
    }
    errno = 0;
    uint32_t seed = strtoul(argv[1], NULL, 10);
    if (errno) {
        cerr << argv[0] << "seed" << endl;
        MPI_Finalize();
        return 1;
    }
    char * pgm = argv[0];
    char fname[500];
    int idx = rank;
    seed = seed + rank * 127;
    sprintf(fname, "%s-%d-%04d.txt", pgm, seed, rank);
    int fd;
    fd = open(fname, O_WRONLY | O_CREAT, 0660);
    if (fd < 0) {
        cerr << pgm << ": open file error\n" << endl;
        MPI_Finalize();
        return 1;
    }
    dup2(fd, 1);
    if (errno) {
        cerr << pgm << ": dup error.\n" << endl;
        close(fd);
        MPI_Finalize();
        return 1;
    }
    int max_idx = sizeof(list) / sizeof(calc_list_t);
    while (idx < max_idx) {
        int s = list[idx].s;
        int m = list[idx].m;
        double c = calc_c_for_cvmean(s, n);
        make_table(64, table, c);
        linear_scramble<uint64_t>(dnid, s, m, seed, 200, table);
        idx += num_process;
    }
    close(fd);
    MPI_Finalize();
    return 0;
}
