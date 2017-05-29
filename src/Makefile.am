digital_header = DigitalNet.hpp digital.h bit_operator.h config.h \
combination.hpp

calc_header = grayindex.hpp matrix_rank.hpp kahan.hpp calc_wafom.hpp \
calc_wafom_lookup.hpp calc_tvalue.hpp cvmean.h

bin_PROGRAMS = digitalnet_index

# build target files of make check
check_PROGRAMS = test_combination test_gray test_calc_cvmean test_calc_wafom \
 test_calc_tvalue test_digitalnet

#executable files of make check (sh file ok)
TESTS = test_combination test_gray test_calc_cvmean test_calc_wafom \
 test_calc_tvalue test_digitalnet

test_combination_SOURCES = test_combination.cpp $(digital_header)
test_gray_SOURCES = test_gray.cpp $(calc_header)
test_calc_cvmean_SOURCES = test_calc_cvmean.cpp cvmean.cpp $(calc_header)
test_calc_wafom_SOURCES = test_calc_wafom.cpp calc_wafom_lookup.cpp \
DigitalNet.cpp $(calc_header)
test_calc_tvalue_SOURCES = test_calc_tvalue.cpp DigitalNet.cpp $(calc_header)
test_digitalnet_SOURCES = test_digitalnet.cpp DigitalNet.cpp $(digital_header)

digitalnet_index_SOURCES = digitalnet_index.cpp \
	DigitalNet.cpp $(digital_header) \
 cvmean.cpp $(calc_header)

AM_CXXFLAGS = -O3 -Wall -Wextra -D__STDC_CONSTANT_MACROS
