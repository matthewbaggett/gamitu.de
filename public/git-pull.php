<?php
$command = "/usr/bin/git pull origin /home/gamitude/www";
echo $command."\n"; 
passthru($command);

die("\n\ndone");