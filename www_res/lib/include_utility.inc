<?php

function public_db_connect()
{
  return new mysqli('localhost','ws_cilbup','cilbup','ws');
}

function store_message($message,$bucket='error')
{
  if (!(isset($_SESSION[$bucket])))
    $_SESSION[$bucket]=array();
  $_SESSION[$bucket][]=$message;
  return TRUE;
} // function store_error

function display_message($bucket='error')
{
  switch ($bucket)
  {
    case 'error': $color="\"red\"";  break;
    case 'success': $color="\"green\"";  break;
    default: $color="\"blue\"";  break;
  } // end switch bucket

  if (!(isset($_SESSION[$bucket])))
    return FALSE;

  foreach ($_SESSION[$bucket] as $message)
  {
    echo "<font color=".$color.">".$message."</font><br>";
  }
  unset($_SESSION[$bucket]);
  return TRUE;
}

function get_passed_data($variable)
{
//echo "<font color=\"red\">Start get_passed_data.  variable=".$variable."</font><br>";
  if (isset($_POST[$variable]))
    $$variable=$_POST[$variable];
  else if (isset($_GET[$variable]))
    $$variable=$_GET[$variable];
  else
    $$variable='';

  // dropping in 4 exclusions...
  if (($variable=='body') || ($variable=='subject') || ($variable=='message') || ($variable=='notes'))
    return $$variable;

  // separate check if it's a URL.
  if ($variable=='url')
  {
    if (preg_match('|^http(s)?://[a-z0-9-]+(.[a-z0-9-]+)*(:[0-9]+)?(/.*)?$|i', $$variable))
    {
      return $$variable;
    }
    else
    {
      return '';
    }
  }

  // validate passed data for bad characters (ignoring type)
  $bad_characters = "[^A-Za-z0-9#:.\'' '_@#%!-]"; // not sure about adding ? and $... worrid about injection attacks now
  if (preg_match("/$bad_characters/",$$variable))
  {
    $$variable=''; // if error, just blank the value.
  }

  return $$variable;
} // function get_passed_data

function unsafe_get_passed_data($variable)
{
  if (isset($_POST[$variable]))
    $$variable=$_POST[$variable];
  else if (isset($_GET[$variable]))
    $$variable=$_GET[$variable];
  else
    $$variable='';

  return $$variable;
} // function unsafe_get_passed_data

function valid_input($myinput, $good_input) {
  //$myinput is the string being checked.
  //$good_input is the regular expression used to check it.
  if (preg_match("/$good_input/", $myinput, $regs)) {
    return true;
  }
  else {
    return false;
  }
}

function get_primary_key($table)
{
  $query="show index from ".$table." where Key_name='PRIMARY';";
  $result=mysql_query($query);
  $data=mysql_fetch_object($result);
  return $data->Column_name;
}

function validate_id($id)
{
  if ((is_numeric($id)) && ($id>0))
    return TRUE;
  else
    return FALSE;
}

function format_phone($phone)
{
  if (Strlen($phone)==10)
    return "(".substr($phone,0,3).")".substr($phone,3,3)."-".substr($phone,6,4);
  else 
    return "";

} // end function format_phone

/* Tools to handle jsonp */
function b_send_response($jres,$callback)
{
  if ($callback!="")
  {
    header('Content-Type: text/javascript; charset=utf8');
    header('Access-Control-Allow-Origin: http://www.example.com/');
    header('Access-Control-Max-Age: 3628800');
    header('Access-Control-Allow-Methods: GET, POST, PUT, DELETE');
    echo $callback."(";
  }
  else
    header('Content-Type: application/json; charset=utf8');
  echo json_encode($jres);
  if ($callback!="")
    echo ");";
} // end b_send_response($jres,$callback)

?>