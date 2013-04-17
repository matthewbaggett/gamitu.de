<?php 
ini_set('display_errors', TRUE);
ini_set('display_startup_errors', TRUE);
ini_set('memory_limit', '2G');

// Include Google PHP library
require_once('../lib/google-api-php-client/src/Google_Client.php');
require_once('../lib/google-api-php-client/src/contrib/Google_PlusService.php');
require_once('../lib/google-api-php-client/src/contrib/Google_Oauth2Service.php');
require_once('../lib/google-api-php-client/src/contrib/Google_LatitudeService.php');

// Include t-core
require_once("../lib/tcore/bootstrap.inc");

\tcore\tcore::run();