<?php
  /* basic paths .. top for "Demo" build, bottom for "Local sandbox build" */
  //$wwwdir="";
  //$resdir="PATH TO RESOURCE DIRECTORY";

  $wwwdir="C:/wamp/www/Wellspring/";
  $resdir="C:/wamp/www_res/Wellspring/";

  include($resdir."lib/include_utility.inc");
  include($resdir."lib/train_schedule.inc");

  // start my phpsession
  session_start();
?>
