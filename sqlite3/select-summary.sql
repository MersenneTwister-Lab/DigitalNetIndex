.separator ,
select netname, min(dimr), max(dimr), count(*), dimf2 tvalue from digitalnet
group by netname, dimf2
order by netname, dimf2;
