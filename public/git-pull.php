<?php
$command = "git pull .";
echo $command."\n"; 
exec($command);

die("\n\ndone");