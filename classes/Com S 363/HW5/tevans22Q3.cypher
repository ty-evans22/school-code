// Create SPJ Database
// Author: Tyler Evans

MATCH (n) OPTIONAL MATCH (n)-[r]-()
DELETE n, r;

MATCH (n:Suppliers)
REMOVE n:Suppliers;
MATCH (n:Parts)
REMOVE n:Parts;
MATCH (n:Projects)
REMOVE n:Projects;
MATCH (n:SPJ)
REMOVE n:SPJ;

drop constraint suppliersSID if exists;
drop constraint SIDexists if exists;
drop constraint partsPID if exists;
drop constraint PIDexists if exists;
drop constraint projectsPRID if exists;
drop constraint PRIDexists if exists;

create constraint suppliersSID FOR (s:Suppliers) REQUIRE s.id IS UNIQUE;
create constraint SIDexists FOR (s:Suppliers) REQUIRE s.id IS NOT NULL;
create constraint partsPID FOR (p:Parts) REQUIRE p.id IS UNIQUE;
create constraint PIDexists FOR (p:Parts) REQUIRE p.id IS NOT NULL;
create constraint projectsPRID FOR (pr:Projects) REQUIRE pr.id IS UNIQUE;
create constraint PRIDexists FOR (pr:Projects) REQUIRE pr.id IS NOT NULL;

CREATE (:Suppliers {id:line.sid, Sname:line.Sname});
CREATE (:Parts {id:line.pid, Pname:line.Pname});
CREATE (:Projects {id:line.prid, Jname:line.Jname});
CREATE (:SPJ {Qty:line.Qty});