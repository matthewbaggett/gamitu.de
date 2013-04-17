<?php 
ini_set('display_errors', TRUE);
ini_set('display_startup_errors', TRUE);
ini_set('memory_limit', '2G');
date_default_timezone_set("Europe/London");

attempt_define("APPLICATION_NAME","Gamitude");

// Define google api things
attempt_define("GOOGLE_CLIENT_ID",'692588353126.apps.googleusercontent.com');
attempt_define("GOOGLE_CLIENT_SECRET",'MuzN_JnyjmaVZuL5zEfhhcQ1');
attempt_define("GOOGLE_API_KEY","AIzaSyAOp7CgK7IF_w97D_BIcU7iSb_544FOZro");
attempt_define("GOOGLE_LOGIN_REDIRECT",'http://gamitu.de/user/register-google');

// Define 4SQ api things
define("FOURSQUARE_ID", "A0E0UFLSVUV45SMMCD0O0J4OIO0I3OFJJB5U3SZ0IFSVNYJC");
define("FOURSQUARE_SECRET", "2UZWRFA2WL2UK4N1LCF3SBLPLNF0IBMI4BGD04S1QWC0VNTA");
define("FOURSQUARE_REDIRECT_URL", "https://gamitu.de/Account/Foursquare-Attach-Complete");

// Default configuration options
\fb::setEnabled(FALSE);
\fb::setEnabled(TRUE);

\tcore\tcore::get_view()
	->set_author("Matthew Baggett <matthew@baggett.me>")
	->set_description("A fun lil' web application for tracking your location & movement - Gamification of Travel")
	->set_title("Gamitu.de")
	->set_sitename("Gamitude");
	
attempt_define("SCHEDULE_FUDGE",FALSE);