<?php
ob_start();
	/**
	 * GIT DEPLOYMENT SCRIPT
	 *
	 * Used for automatically deploying websites via github or bitbucket, more deets here:
	 *
	 *		https://gist.github.com/1809044
	 */
	chdir("../");
	// The commands
	$commands = array(
		'echo $PWD',
		'whoami',
		'hostname',
		'git pull',
		'git status',
		'git submodule init',
		'git submodule sync',
		'git submodule update',
		'git submodule status',
		'git submodule foreach git pull origin master'
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

	// Make it pretty for manual user access (and why not?)
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
mail(
"matthew@baggett.me",
"Deployment on ".gethostname()." at " . date("d/m/Y H:i:s"),
$output
);
?>