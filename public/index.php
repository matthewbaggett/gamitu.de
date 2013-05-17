<?php
ini_set('display_errors', true);
ini_set('display_startup_errors', true);
ini_set('memory_limit', '2G');
error_reporting(E_ALL);

// Include t-core
require_once("../lib/tcore/bootstrap.inc");

// Include Google PHP library
require_once('../lib/google-api-php-client/src/Google_Client.php');
require_once('../lib/google-api-php-client/src/contrib/Google_PlusService.php');
require_once('../lib/google-api-php-client/src/contrib/Google_Oauth2Service.php');
require_once('../lib/google-api-php-client/src/contrib/Google_LatitudeService.php');

// Include EpiFoursquare library
require_once("../lib/foursquare-async/EpiCurl.php");
require_once("../lib/foursquare-async/EpiSequence.php");
require_once("../lib/foursquare-async/EpiFoursquare.php");

\tcore\tcore::configure();
\tcore\tcore::run();