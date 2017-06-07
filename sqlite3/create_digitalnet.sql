drop table digitalnet;
create table digitalnet (
	netname text,
        bitsize int,
        dimr int,
        dimf2 int,
        wafom real,
        tvalue int,
        data text,
        primary key (netname, bitsize, dimr, dimf2)
        );
