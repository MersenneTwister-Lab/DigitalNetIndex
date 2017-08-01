-- alter table digitalnet rename to digital_tb;
alter table digitalnet add column id int;
update digitalnet set id = 0 where netname = 'nx';
update digitalnet set id = 3 where netname = 'nxlw';
update digitalnet set id = 4 where netname = 'solw';
create table digital_tb (
	id int,
        bitsize int,
        dimr int,
        dimf2 int,
        wafom real,
        tvalue int,
        data text,
        primary key (id, bitsize, dimr, dimf2)
        );
insert into digital_tb select id, bitsize, dimr, dimf2, wafom, tvalue, data
from digitalnet;
drop table digitalnet;
create view digitalnet as select
name as netname, i.id, bitsize, dimr, dimf2, wafom, tvalue, data
from digital_tb t join digitalnet_id i
where t.id = i.id;
