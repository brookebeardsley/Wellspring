<div id="content">
<?php
  // below line create the new class.. and imports a file at the same time.
  //$ts=new train_schedule('sample.csv');

  $ts=new train_schedule();
  $ts->view_to_html();
?>
</div>