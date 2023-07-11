-- author: Tyler Evans

CREATE PROCEDURE findPopularHashtags(
	IN k INTEGER, IN year_of_interest INTEGER)
    SELECT 
		COUNT(DISTINCT practice.users.state) AS 'statenum',
        group_concat(DISTINCT practice.users.state ORDER BY practice.users.state) AS 'states',
		practice.hashtags.name AS 'hashtagname'
    FROM practice.hashtags
    INNER JOIN practice.tweets
    ON practice.hashtags.tid = practice.tweets.tid
    INNER JOIN practice.users
    ON practice.tweets.posting_user = practice.users.screen_name
    WHERE practice.tweets.post_year = year_of_interest
		AND NOT practice.users.state = 'na'
    GROUP BY practice.hashtags.name
    ORDER BY statenum DESC LIMIT k;