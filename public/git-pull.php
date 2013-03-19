<?php
if($_REQUEST['password'] !== 'kU4EdRaw'){
    die("Wrong password");
}
chdir("../");
ob_start();

// The commands
$commands = array(
	'echo $PWD',
	'whoami',
	'hostname',
    'git status',
    'git pull',
	'git status',
	
	'rm lib/tcore -Rfv',
	'mkdir lib/tcore -p',
    'cd lib/tcore',
    'git clone git@github.com:geusebio/tcore.git . --verbose',
    'cd ../../',
	
    'rm lib/eden -Rfv',
	'mkdir lib/eden -p',
    'cd lib/eden',
    'git clone git@github.com:geusebio/eden.git . --verbose',
    'cd ../../',
	
    'rm lib/firephp -Rfv',
	'mkdir lib/firephp -p',
    'cd lib/firephp',
    'git clone git://github.com/firephp/firephp-core.git . --verbose',
    'cd ../../',
);

// Run the commands for output
$output = '';
foreach($commands AS $command){
	// Run it
	$tmp = shell_exec($command);
	// Output
	$output .= "<span style=\"color: #6BE234;\">\$</span> <span style=\"color: #729FCF;\">{$command}\n</span>";
	$output .= htmlentities(trim($tmp)) . "\n";
}

?>
<!DOCTYPE HTML>
<html lang="en-US">
<head>
	<meta charset="UTF-8">
	<title>GIT DEPLOYMENT SCRIPT</title>
</head>
<body style="background-color: #000000; color: #FFFFFF; font-weight: bold; padding: 0 10px;">
<pre>
 .  ____  .    ____________________________
 |/      \|   |                            |
[| <span style="color: #FF0000;">&hearts;    &hearts;</span> |]  | Git Deployment Script v0.1 |
 |___==___|  /              &copy; oodavid 2012 |
              |____________________________|

<?php echo $output; ?>
</pre>
</body>
</html>
<?php
$output = ob_get_contents();
ob_end_clean();

echo $output; 

$to = "matthew@baggett.me";
// To send HTML mail, the Content-type header must be set
$headers  = 'MIME-Version: 1.0' . "\r\n";
$headers .= 'Content-type: text/html; charset=iso-8859-1' . "\r\n";
$headers .= 'To: ' . $to . "\r\n";
$headers .= 'From: GitHub AutoDeploy on ' . gethostname() . " <service@gamitu.de>\r\n";

mail(
    $to,
    "Deployment on ".gethostname()." at " . date("d/m/Y H:i:s"),
    $output, 
    $headers
);
?>