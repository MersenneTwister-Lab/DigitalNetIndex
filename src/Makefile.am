digital_files = DigitalNet.hpp digital.h bit_operator.h config.h \
combination.hpp sobolpoint.h DigitalNet.cpp \
MersenneTwister64.hpp interlaced_sobolpoint.cpp sobolpoint.cpp

calc_files = grayindex.hpp matrix_rank.hpp kahan.hpp calc_wafom.hpp \
calc_wafom_lookup.hpp calc_tvalue.hpp cvmean.h cvmean.cpp \
calc_wafom_lookup.cpp diff_wafom.hpp

bin_PROGRAMS = digitalnet_index search_lowwafom add_index

# build target files of make check
check_PROGRAMS = test_combination test_gray test_calc_cvmean test_calc_wafom \
 test_calc_tvalue test_digitalnet test_maxmin test_double

#executable files of make check (sh file ok)
TESTS = test_combination test_gray test_calc_cvmean test_calc_wafom \
 test_calc_tvalue test_digitalnet test_maxmin test_double

test_combination_SOURCES = test_combination.cpp $(digital_files)
test_gray_SOURCES = test_gray.cpp $(calc_files)
test_calc_cvmean_SOURCES = test_calc_cvmean.cpp $(calc_files)
test_calc_wafom_SOURCES = test_calc_wafom.cpp $(digital_files) $(calc_files)
test_calc_tvalue_SOURCES = test_calc_tvalue.cpp $(calc_files) $(digital_files)
test_digitalnet_SOURCES = test_digitalnet.cpp $(digital_files)
test_maxmin_SOURCES = test_maxmin.cpp $(digital_files)
test_double_SOURCES = test_double.cpp

digitalnet_index_SOURCES = digitalnet_index.cpp $(digital_files) $(calc_files)
add_index_SOURCES = add_index.cpp $(digital_files) $(calc_files)

search_lowwafom_SOURCES = search_lowwafom.cpp $(digital_files) \
	linear_scramble.hpp $(calc_files)

AM_CXXFLAGS = -O3 -Wall -Wextra -D__STDC_CONSTANT_MACROS

EXTRA_DIST = Makefile.mpi search_lownx_mpi.cpp
