-- 1.
-- a)
select count(*) as numFoods from food;

-- b)
select f.*
from food f, ingredient i, recipe r
where f.fid=r.fid and r.iid=i.iid and 
i.iname='Chicken'
order by r.amount desc;

-- c)
select f.fid, f.fname, count(*) as numMeatIngredients
from recipe r, food f, ingredient i 
where r.fid = f.fid and r.iid = i.iid 
and i.category='Meat'
group by f.fid, f.fname;

-- d)
select i.iname
from ingredient i, recipe r 
where i.iid = r.iid 
group by i.iname
having count(r.iid) >=2; 

-- e)
select i.iname 
from ingredient i left join recipe r on i.iid=r.iid
where r.iid is null
order by i.iname desc;

select i.iname
from ingredient i
where not exists (
   select * 
   from recipe r
   where i.iid = r.iid
)
order by i.iname desc;

-- g)
select f.* 
from food f, recipe r, ingredient i 
where f.fid = r.fid and r.iid = i.iid and i.iname='Green Onion' 
and exists
(select * 
 from recipe r2, ingredient i2 
 where r2.iid = i2.iid and i2.iname='Chicken' and f.fid=r2.fid); 
 
 -- h)
 select f.* 
from food f, recipe r, ingredient i 
where f.fid = r.fid and r.iid = i.iid and i.iname='Green Onion' 
and f.fid in
(select r2.fid
 from recipe r2, ingredient i2 
 where r2.iid = i2.iid and i2.iname='Chicken'); 
