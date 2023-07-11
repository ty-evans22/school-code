-- author: Tyler Evans

DELIMITER $$

CREATE PROCEDURE deleteUser(
	IN screen_name VARCHAR(80), OUT output INTEGER)
proc_label:BEGIN
	SELECT COUNT(practice.users.screen_name)
    FROM practice.users
    WHERE practice.users.screen_name = screen_name
    INTO output;
    
    IF output = 0 THEN
		 SET output = -1;
         LEAVE proc_label;
	END IF;
    
    -- delete urls from user tweets
    DELETE R
    FROM practice.users U
    INNER JOIN practice.tweets T ON U.screen_name = T.posting_user
    INNER JOIN practice.urls R ON T.tid = R.tid
    WHERE U.screen_name = screen_name;
    
    -- delete urls from mentioned tweets
    DELETE R
    FROM practice.mentions M
    INNER JOIN practice.urls R ON M.tid = R.tid
    WHERE M.mentioned_user_scr = screen_name;
    
    -- delete hashtags from user tweets
    DELETE H
    FROM practice.hashtags H
    INNER JOIN practice.tweets T ON H.tid = T.tid
    WHERE T.posting_user = screen_name;
    
    -- delete hashtags from mentioned tweets
    DELETE H
    FROM practice.hashtags H
    INNER JOIN practice.mentions M ON H.tid = M.tid
    WHERE M.mentioned_user_scr = screen_name;
    
    -- delete mentions
    DELETE M
    from practice.mentions M
    WHERE M.mentioned_user_scr = screen_name;
    
    -- delete user mentions
    DELETE M
    from practice.mentions M
    INNER JOIN practice.tweets T ON M.tid = T.tid
    WHERE T.posting_user = screen_name;
    
    -- delete user tweets
    DELETE FROM practice.tweets
    WHERE posting_user = screen_name;
    
    -- delete user
    DELETE FROM practice.users
    WHERE practice.users.screen_name = screen_name;
    
    IF @@error_count > 0 THEN
		SET output = 1;
        LEAVE proc_label;
	ELSE SET output = 0;
    END IF;
END $$