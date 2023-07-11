// tevans22Q2
// Author: Tyler Evans

// a.
// MATCH (u:User)-[:POSTED]->(t:Tweet)
// WHERE t.post_month = 2 AND t.post_year = 2016
// RETURN u.screen_name AS screenname, u.sub_category AS party, t.retweet_count AS retweetCount
// ORDER BY t.retweet_count DESC
// LIMIT 5;

// b.
// MATCH (h:Hashtag)-[:TAGGED]->(t:Tweet)<-[:POSTED]-(u:User)
// WHERE u.location = 'Ohio' OR u.location = 'Alaska'
// RETURN distinct h.name AS hashtagName
// ORDER BY h.name ASC
// LIMIT 5;

// c.
// MATCH (t:Tweet)<-[:POSTED]-(u:User)
// WHERE u.sub_category = 'democrat'
// RETURN distinct u.screen_name AS screenname, u.sub_category AS party, u.followers AS numFollowers
// ORDER BY u.followers DESC
// LIMIT 5;

// d.
// MATCH (post:User {sub_category: 'GOP'})-[:POSTED]->(t:Tweet)
// MATCH (t)-[m:MENTIONED]->(u:User)
// WITH u, collect(DISTINCT post.screen_name) AS listMentioningUsers, COUNT(m) AS mcount ORDER BY mcount DESC LIMIT 5
// RETURN u.screen_name AS mentionedUser, listMentioningUsers;

// e.
MATCH (h:Hashtag)-[:TAGGED]->(t:Tweet)
MATCH (u:User)-[:POSTED]->(t)
WHERE u.location <> 'na'
WITH h, collect(DISTINCT u.location) AS statelist, COUNT(DISTINCT u.location) AS numstates ORDER BY numstates DESC
RETURN h.name AS hashtag, numstates, statelist
LIMIT 3;