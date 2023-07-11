-- Tyler Evans
drop procedure if exists suppliersWithoutParts;

delimiter //
create procedure suppliersWithoutParts()
begin
	select s.sid, s.sname
	from suppliersparts.suppliers s 
	left join suppliersparts.catalog c on s.sid=c.sid
	where c.sid is null;
end;//
delimiter ;