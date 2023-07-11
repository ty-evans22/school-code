 /*
	Tyler Evans
*/
drop procedure if exists  redgreenSuppliers;
delimiter //
create procedure redgreenSuppliers()
begin
	SELECT c.sid, min(p.color) color1 , max(p.color) color2
	from suppliersparts.catalog c inner join suppliersparts.parts p 
		on c.pid = p.pid
	where p.color in ('green', 'red')
	group by c.sid
	having count(distinct p.color) = 2;
	
END;//
delimiter ;