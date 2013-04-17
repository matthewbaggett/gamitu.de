<?php 
ini_set('display_errors', TRUE);
ini_set('display_startup_errors', TRUE);
ini_set('memory_limit', '2G');

require_once('../lib/google-api-php-client/src/Google_Client.php');
require_once('../lib/google-api-php-client/src/contrib/Google_PlusService.php');
require_once("../lib/tcore/bootstrap.inc");

\tcore\tcore::run();