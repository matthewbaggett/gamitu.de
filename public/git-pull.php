<?php
$command = "git pull origin";
echo $command."\n"; 
passthru($command);

die("\n\ndone");