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
    digital_net_id dnid = NX;
    int n = 64;
    static lookup_t table;
    if (argc <= 3) {
        cerr << argv[0] << "start_s end_s seed" << endl;
        MPI_Finalize();
        return -1;
    }
    errno = 0;
    int start_s = strtoul(argv[1], NULL, 10);
    int end_s = strtoul(argv[2], NULL, 10);
    uint32_t seed = strtoul(argv[3], NULL, 10);
    if (errno) {
        cerr << argv[0] << "start_s end_s seed" << endl;
        MPI_Finalize();
        return 1;
    }
    int start_m = 10;
    if (argc >= 5) {
        start_m = strtoul(argv[4], NULL, 10);
        if (errno) {
            cerr << argv[0] << "start_s end_s seed" << endl;
            MPI_Finalize();
            return 1;
        }
    }
    char * pgm = argv[0];
    char fname[500];
    int s = start_s - 1 + gyukou(rank + 1, num_process);
    cout << "s = " << s << endl;
    sprintf(fname, "%s-nx-%03d-%04d.txt", pgm, s, rank);
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
    seed = seed + rank * 127;

    while (s <= end_s) {
        double c = calc_c_for_cvmean(s, n);
        make_table(64, table, c);
        for (int m = start_m; m <= 16; m++) {
            linear_scramble<uint64_t>(dnid, s, m, seed, 200, table);
            //cout << "linear_scramble " << dnid << " " << s << " " << m
            //     << " " << seed << " " << 200 << endl;
        }
        rank += num_process;
        s = start_s - 1 + gyukou(rank + 1, num_process);
    }
    close(fd);
    MPI_Finalize();
    return 0;
}
