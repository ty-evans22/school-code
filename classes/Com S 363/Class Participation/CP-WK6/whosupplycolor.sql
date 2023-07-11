 /*
	Tyler Evans
*/
drop procedure if exists  whosupplycolor;
delimiter //
create procedure whosupplycolor(IN partcolor VARCHAR(20), IN partcolor2 VARCHAR(20))
begin
	SELECT c.sid, min(p.color) color1 , max(p.color) color2
	from suppliersparts.catalog c inner join suppliersparts.parts p 
		on c.pid = p.pid
	where p.color in (partcolor, partcolor2)
	group by c.sid;
	
END;//
delimiter ;