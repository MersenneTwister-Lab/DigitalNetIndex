drop table digitalnet_id;
create table digitalnet_id (
        id int,
	name text,
        longname text,
        primary key (id)
        );
insert into digitalnet_id values(0, 'nx', 'Niederreiter-Xing');
insert into digitalnet_id values(1, 'sobol', 'Sobol');
insert into digitalnet_id values(2, 'oldso', 'Old Sobol');
insert into digitalnet_id values(3, 'nxlw', 'Niederreiter-Xing + Low WAFOM');
insert into digitalnet_id values(4, 'solw', 'Sobol + Low WAFOM');
