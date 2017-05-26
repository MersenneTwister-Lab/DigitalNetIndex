digital_header = DigitalNet.h digital.h bit_operator.h powtwo.h config.h \
combination.hpp

compute_header = compute.h gray.hpp matrix_rank.hpp kahan.hpp

bin_PROGRAMS = digitalnet_index

# build target files of make check
check_PROGRAMS = test_combination test_gray

#executable files of make check (sh file ok)
TEST = test_combination test_gray

test_combination_SOURCES = test_combination.cpp combination.hpp
test_gray_SOURCES = test_gray.cpp

digitalnet_index_SOURCES = digitalnet_index.cpp \
	DigitalNet.cpp $(digital_header) \
	compute.cpp $(compute_header)

AM_CXXFLAGS = -O3 -Wall -Wextra -D__STDC_CONSTANT_MACROS
