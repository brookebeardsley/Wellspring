<?php require_once("config.inc"); ?>
<?php

  $action=get_passed_data('action');

  switch($action){
    case "upload":
      $ts=new train_schedule(get_passed_data('body'));
      $ts->view_to_jsonp();
      break;
    case "delete":
      $ts=new train_schedule();
      $ts->remove_from_sview(get_passed_data('scheduleid'));
      $ts->view_to_jsonp();
      break;
    case "update":
      $ts=new train_schedule();
      if ($ts->update_sview(get_passed_data('scheduleid'),get_passed_data('trainline'),get_passed_data('routename'),get_passed_data('runnumber'),get_passed_data('operatorid')))
        $ts->view_to_jsonp();
      break;
    default:
      b_send_response(array('status' => 'error', 'message' => 'Invalid action: "'.$action.'".'),get_passed_data('callback'));
      break;
  } // end switch($action)
?>