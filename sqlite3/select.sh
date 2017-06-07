#!/bin/bash
dbname=$1
netname=$2
dimr=$3
dimf2=$4
sqlite3 $dbname <<EOF
select netname, bitsize, dimr, dimf2, wafom, tvalue from digitalnet
where netname = "$netname"
and bitsize = 64
and dimr = "$dimr"
and dimf2 = (select min(dimf2)
from digitalnet where netname = "$netname" and bitsize = 64 and dimr = "$dimr" and dimf2 >= "$dimf2");
EOF
