drop table digitalnet;
create table digitalnet (
        dimr int,
        dimf2 int,
        wafom real,
        tvalue int,
        data blob,
        primary key (dimr, dimf2)
        );
