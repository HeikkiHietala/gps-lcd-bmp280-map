<?php
    $_POST = file_get_contents('php://input');
    $myYear = date("d.m.Y");
    $myHour = date("H:i");
    $myFile = "iotwrite.txt";
    $fh = fopen($myFile, 'a') or die("can't open file");
    fwrite($fh, $myYear . " " .  $myHour);
    fwrite($fh, "\t");
    fwrite($fh, $_POST);
    fwrite($fh, "\n");    
    fclose($fh);
?>