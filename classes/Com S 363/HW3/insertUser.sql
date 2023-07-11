-- author: Tyler Evans

DELIMITER $$

CREATE PROCEDURE insertUser(
	IN screen_name VARCHAR(80), IN user_name VARCHAR(80),
    IN category VARCHAR(80), IN subcategory VARCHAR(80),
    IN state VARCHAR(80), IN numFollowers INTEGER,
    IN numFollowing INTEGER, OUT success INTEGER)
proc_label:BEGIN
	-- check if insertion already exists
	SELECT COUNT(practice.users.screen_name)
    FROM practice.users
    WHERE practice.users.screen_name = screen_name
    INTO success;
    
    IF success = 1 THEN
		SET success = 0;
        LEAVE proc_label;
	END IF;

	-- otherwise insert new row
    INSERT INTO practice.users
    VALUES (screen_name, user_name, category, subcategory,
			state, numFollowers, numFollowing);
    
    SELECT COUNT(practice.users.screen_name)
    FROM practice.users
    WHERE practice.users.screen_name = screen_name
    INTO success;
END $$