/*
	Tyler Evans
*/
drop procedure if exists numParts;
delimiter //
create procedure numParts()
begin
	SELECT count(*)
	FROM suppliersparts.parts;
	
end;//
delimiter ;

