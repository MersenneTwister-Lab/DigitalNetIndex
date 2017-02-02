DigitalNetIndex
===============
Calculate WAFOM and t-value of Digital Net.

REQUIREMENT
===========
C++11, Boost 1.48 or later

INSTALL
=======
  ./configure --prefix=YOUR_INSTALL_DIR
  make
  make install

PREPARE
=======
before use, unzip data files
  cd YOUR_INSTALL_DIR/share/DigitalNetIndex/data
  gunzip bigtyl.dat.gz
  gunzip meantyl.dat.gz
set environment variable DIGITAL_NET_PATH
  export DIGITAL_NET_PATH=YOUR_INSTALL_DIR/share/DigitalNetIndex/data

EXAMPLE
=======


CAUTION
=======
Data file `bigtyl.dat' and `meantyl.dat' are binary files which were
made on Little Endian System and will not work correctly on
Big Endian Systems.

LICENSE
=======
    Copyright (C) 2017 Shinsuke Mori, Makoto Matsutmoto, Mutsuo Saito
    and Hiroshima University.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
