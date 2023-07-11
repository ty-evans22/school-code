-- author: Tyler Evans

CREATE PROCEDURE mostFollowedUsers(
	IN k INTEGER, IN political_party VARCHAR(80))
    SELECT
		screen_name,
        subcategory,
        numFollowers
	FROM practice.users
    WHERE subcategory = political_party
    ORDER BY numFollowers DESC LIMIT k;