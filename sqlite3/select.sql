select netname, bitsize, dimr, dimf2, wafom, tvalue, data from digitalnet
where netname = ?
and bitsize = 64
and dimr = ?
and dimf2 = (select min(dimf2) from digitalnet
where netname = ?
and bitsize = ?
and dimr = ?
and dimf2 >= ?);
