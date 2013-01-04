<?php 
ini_set('display_errors', TRUE);
ini_set('display_startup_errors', TRUE);
ini_set('memory_limit', '2G');
date_default_timezone_set("Europe/London");

// Define google api things
define("GOOGLE_CLIENT_ID",'692588353126.apps.googleusercontent.com');
define("GOOGLE_CLIENT_SECRET",'MuzN_JnyjmaVZuL5zEfhhcQ1');
define("GOOGLE_API_KEY","AIzaSyAOp7CgK7IF_w97D_BIcU7iSb_544FOZro");

// Default configuration options
\fb::setEnabled(FALSE);
\fb::setEnabled(TRUE);

\tcore\tcore::get_view()
	->set_author("Matthew Baggett <matthew@baggett.me>")
	->set_description("A fun lil' web application for tracking your location & movement - Gamification of Travel")
	->set_title("Gamitu.de")
	->set_sitename("Gamitude");
	
\tcore\db::config(
		"midas.cxf7buw7vivh.eu-west-1.rds.amazonaws.com", //host
		"gamitude",	//user
		"2sUN13984x89e46", //pass
		"gamitude" //db
);
	