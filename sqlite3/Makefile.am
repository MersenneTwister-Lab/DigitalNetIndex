noinst_bindir =
noinst_bin_PROGRAMS = insert_digitalnet select_digitalnet nx2dnform \
insert_sobolbase select_sobolbase insert_digitalnet_binary \
select_digitalnet_binary

insert_digitalnet_SOURCES = insert_digitalnet.cpp
insert_digitalnet_binary_SOURCES = insert_digitalnet_binary.cpp nb64.cpp
select_digitalnet_SOURCES = select_digitalnet.cpp
select_digitalnet_binary_SOURCES = select_digitalnet_binary.cpp nb64.cpp
nx2dnform_SOURCES = nx2dnform.cpp
insert_sobolbase_SOURCES = insert_sobolbase.cpp
select_sobolbase_SOURCES = select_sobolbase.cpp

AM_CXXFLAGS = -O3 -Wall -Wextra -D__STDC_CONSTANT_MACROS

EXTRA_DIST = create_digitalnet.sql
