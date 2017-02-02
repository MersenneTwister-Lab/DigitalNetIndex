digital_header = digital.h bit_operator.h powtwo.h config.h
compute_header = compute.h gray.hpp matrix_rank.hpp kahan.hpp

bin_PROGRAMS = digitalnet_index

digitalnet_index_SOURCES = digitalnet_index.cpp \
	DigitalNet.cpp $(digital_header) \
	compute.cpp $(compute_header)

#AM_LDFLAGS = -L./
AM_CXXFLAGS = -O3 -Wall -Wextra -D__STDC_CONSTANT_MACROS
