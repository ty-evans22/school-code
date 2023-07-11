-- Author: Tyler Evans

-- Problem 1a
SELECT COUNT(*) AS num_events
FROM events;

-- Problem 1b
SELECT vid, sched_date
FROM events
WHERE NOT location='ISU Campus';

-- Problem 1c
SELECT DISTINCT events.vid, events.sched_date, events.location
FROM persons
JOIN participates ON persons.pid=participates.pid
JOIN events ON participates.vid=events.vid
WHERE pname='Pak';

-- Problem 1d
SELECT DISTINCT events.vid, events.sched_date, events.location
FROM persons
JOIN participates ON persons.pid=participates.pid
JOIN events ON participates.vid=events.vid
WHERE pname='Pak' OR pname='John'
ORDER BY events.sched_date DESC;

-- Problem 1e
SELECT participates.vid
FROM persons
JOIN participates ON persons.pid=participates.pid
WHERE pname='John' OR pname='Mary' OR pname='Jane'
GROUP BY participates.vid
HAVING COUNT(participates.vid)=3;

-- Problem 1f
SELECT persons.pname
FROM participates
RIGHT JOIN persons
ON participates.pid=persons.pid
GROUP BY participates.pid
HAVING COUNT(participates.pid)=0
ORDER BY persons.pid DESC;

-- Problem 1g
SELECT persons.pid, persons.pname
FROM participates
RIGHT JOIN persons
ON participates.pid=persons.pid
GROUP BY participates.pid
HAVING COUNT(participates.pid)>1
ORDER BY participates.pid;

-- Problem 1h
SELECT venue_id, name
FROM venues
WHERE NOT EXISTS 
(SELECT * FROM events WHERE events.vid = venues.venue_id);

-- Problem 1i
SELECT venue_id, name
FROM venues
WHERE venue_id NOT IN (SELECT vid FROM events);