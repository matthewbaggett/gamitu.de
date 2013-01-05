<?php
$command = "git pull origin";
echo $command."\n"; 
exec($command);

die("\n\ndone");