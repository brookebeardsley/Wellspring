<?php
class train_schedule {
  private $mysqli;
  private $sviewname='';
  private $sviewid;
  private $display_row_colors=array("#FBFEFA","#CBCEFA");
  private $display_row_colors_count;

  public function __construct($schedule_source='',$sviewname='') {
    $this->mysqli=public_db_connect();
//    $this->mysqli=$_SESSION['mysqli'];  // haven't decided which statement is better yet.

    if ($sviewname=='')
      $this->sviewname=$this->new_sview(session_id());
    else
      $this->sviewname=$this->load_sview($sviewname);

    if ($this->sviewid===FALSE)
      return FALSE;

    if ($schedule_source!='')
      $this->load_data($schedule_source);

    $this->display_row_colors_count=count($this->display_row_colors);
  } // end construct

  function start_sql_transaction()
  {
    if ($this->mysqli->query("SET AUTOCOMMIT=0")===TRUE)
      if ($this->mysqli->query("BEGIN")===TRUE)
        return TRUE;
    store_message($this->mysqli->error);
    return FALSE;
  }

  function end_sql_transaction($commit=FALSE)
  {
    if ($commit===TRUE)
      $command="COMMIT";
    else
      $command="ROLLBACK";

    if ($this->mysqli->query($command)===TRUE)
      if ($this->mysqli->query("SET AUTOCOMMIT=1")===TRUE)
        return TRUE;
    store_message($this->mysqli->error);
    return FALSE;
  }
 
  private function new_sview($sviewname)
  {
    if ($result=$this->mysqli->query("select sviewid from sview where sviewname='".$sviewname."' limit 1;"))
    {
      if ($result->num_rows==0)
      {
        $result->close();
        if ($this->mysqli->query("insert into sview (sviewname) VALUES ('".$sviewname."');")===TRUE)
          $this->sviewid=$this->mysqli->insert_id;
        else
        {
          store_message($this->mysqli->error);
          $this->sviewid=FALSE;
        } // end if-else insert sview
      }
      else
      {
        $this->sviewid=$result->fetch_object()->sviewid;
        //store_message("Did not create view: '".$sviewname."': It already exists database.  Loaded it instead.");
      } // end if-else # rows=0;
    }
    else
    {
      store_message($this->mysqli->error);
      $this->sviewid=FALSE;
    }
    //$result->close();
    return $this->sviewid; 
  } // end private function new_sview($sviewname)

  private function load_sview($sviewname)
  {
    if ($result=$this->mysqli->query("select sviewid from sview where 'sviewname'='".$sviewname."' limit 1;"))
    {
      if ($result->num_rows>0)
      {
        $this->sviewid=$result->fetch_object()->sviewid;
      }
      else
      {
        store_message("Unable to load view: '".$sviewname."': It does not exist in database.");
        $this->sviewid=FALSE;
      }
    }
    else
    {
      store_message($this->mysqli->error);
      $this->sviewid=FALSE;
    }
    $result->close();
    return $this->sviewid; 
  } // end private function load_sview($sviewname)

  public function load_data($source)
  {
    // determine input type...

    // is it a ".csv"?
    if (strtolower(substr($source,-4,4))==".csv")
    {
       // yes, load from file.
       $this->load_data_from_file($source);
    }
    else
    {
      switch (strtolower(substr($source,0,4)))
      {
        case 'trai':
        case 'rout':
        case 'run_':
        case 'oper':
          $this->load_data_from_stream($source);
          break;
      } // end switch if it is raw CSV data stream
    } // end if-else "is it a .csv?"
  } // end public function load_data

  private function load_data_from_stream($source)
  {
    $csv_lines=explode(chr(10),$source);
    $header=explode(',',trim($csv_lines[0]));
    foreach ($header as $index=>$value)
    {
      switch(strtolower(trim($value)))
      {
        case 'train_line':
          $TRAINLINE=$index;
          break;
        case 'route_name':
          $ROUTENAME=$index;
          break;
        case 'run_number':
          $RUNNUMBER=$index;
          break;
        case 'operator_id':
          $OPERATORID=$index;
          break;
      } // end switch
    } // end foreach

    $csv_length=count($csv_lines);
    $commit=TRUE; // needed for the SQL transaction rollback action.
    $this->start_sql_transaction(); 
    for ($i=1; $i<$csv_length; $i++)
    {
      $row=explode(',',$csv_lines[$i]);
      $commit=$this->add_line(trim($row[$TRAINLINE]), trim($row[$ROUTENAME]), trim($row[$RUNNUMBER]), trim($row[$OPERATORID]));
    } // end for each data line

    $this->end_sql_transaction($commit);
  } // end private function load_data_from_stream($source)

  private function load_data_from_file($source)
  {
    $fsh=fopen($source,'r');
    if ($fsh)
    {
      $header=explode(',',fgets($fsh));
      foreach ($header as $index=>$value)
      {
        switch(strtolower(trim($value)))
        {
          case 'train_line':
            $TRAINLINE=$index;
            break;
          case 'route_name':
            $ROUTENAME=$index;
            break;
          case 'run_number':
            $RUNNUMBER=$index;
            break;
          case 'operator_id':
            $OPERATORID=$index;
            break;
        } // end switch
      } // end foreach

      $commit=TRUE; // needed for the SQL transaction rollback action.
      $this->start_sql_transaction();

      while ((($line=fgets($fsh))!==false) && ($commit===TRUE))
      {
        $row=explode(',',$line);
        $commit=$this->add_line(trim($row[$TRAINLINE]), trim($row[$ROUTENAME]), trim($row[$RUNNUMBER]), trim($row[$OPERATORID]));
      } // end while (fgets($fsh))

      $this->end_sql_transaction($commit);

      fclose($fsh);
    } // end if ($fsh)
  } // end function load_data_from_file($source)

  private function add_line($d_trainline,$d_routename,$d_runnumber,$d_operatorid)
  {
    $commit=TRUE;

    $result=$this->mysqli->query("SELECT scheduleid from schedule where trainline='".$d_trainline."' and routename='".$d_routename."' and runnumber='".$d_runnumber."' and operatorid='".$d_operatorid."';");
    if ($result===FALSE)
    {
      store_message($this->mysqli->error);
      $commit=FALSE;
    }
    else if ($result->num_rows>0)
      $scheduleid=$result->fetch_object()->scheduleid;
    else
    {
      $result->close();
      if ($this->mysqli->query("INSERT into schedule (trainline,routename,runnumber,operatorid) VALUES ('".$d_trainline."','".$d_routename."','".$d_runnumber."','".$d_operatorid."');"))
        $scheduleid=$this->mysqli->insert_id;
      else
      {
        store_message("Failed to insert row: ('".$row[$TRAINLINE]."','".$row[$ROUTENAME]."','".$row[$RUNNUMBER]."','".$row[$OPERATORID]."')");
        $commit=FALSE;
      }
    } // end if-else result chain to determine uniqueness of row data.
    //$result->close();

    if ($commit===TRUE)
    {
      $result=$this->mysqli->query("SELECT scheduleid from sview_schedule where sviewid='".$this->sviewid."' and scheduleid='".$scheduleid."';");
      if ($result===FALSE)
      {
        store_message($this->mysqli->error);
        $commit=FALSE;
      }
      else if ($result->num_rows==0)
      {
        $result->close();
        if ($this->mysqli->query("INSERT into sview_schedule (sviewid,scheduleid) VALUES ('".$this->sviewid."','".$scheduleid."');")===FALSE)
        {
          store_message("Failed to insert row in sview_schedule");
          store_message($this->mysqli->error);
          $commit=FALSE;
        }
      } // end else if record already exists
    } // end add a link between schedule and sview
    return $commit;
  } // end function add_line

  public function view_to_html()
  {
    // headers:
    echo $this->header_string();

    if ($result=$this->mysqli->query("SELECT scheduleid, trainline, routename, runnumber, operatorid from schedule where scheduleid in (select scheduleid from sview_schedule where sviewid='".$this->sviewid."') order by runnumber ASC;"))
    {
      $r=1;
      while ($obj = $result->fetch_object())
      {
        echo $this->row_to_string($obj,$r++);
      }
    } // end if select query worked.
    
    echo "</table>";
    echo "<div class=\"add_div\"><button class=\"add_row\" id=\"rwo_new\">+ Add More Information</button></div>";
  } // end public function view_to_html()

  public function view_to_jsonp()
  {
    $h=$this->header_string();

    if ($result=$this->mysqli->query("SELECT scheduleid, trainline, routename, runnumber, operatorid from schedule where scheduleid in (select scheduleid from sview_schedule where sviewid='".$this->sviewid."') order by runnumber ASC;"))
    {
      $r=1;
      while ($obj = $result->fetch_object())
      {
        $h.=$this->row_to_string($obj,$r++);
      }
    } // end if select query worked.
    
    $h.= "</table>";
    $h.= "<div class=\"add_div\"><button class=\"add_row\" id=\"rwo_new\">+ Add More Information</button></div>";
    b_send_response(array('status' => 'success', 'message' => $h),get_passed_data('callback'));
  } // end public function view_to_jsonp()

  private function header_string()
  {
    $h="";
    $h.="<table class=\"schedule\">";
    $h.=   "<tr>";
    $h.=     "<th>Train Line</th>";
    $h.=     "<th>Route Name</th>";
    $h.=     "<th>Run Number</th>";
    $h.=     "<th>Operator Id</th>";
    $h.=     "<th class=\"delete_row\">&nbsp;</th>";
    $h.=   "</tr>";
    return $h;
  } // end private function header_string()

  // each table row needs to include the following:
  // train line | route name | run number | operator id | [option to delete (dis-associate) row]
  // jQuery to handle element creation/ modification for edit-in-place.
  private function row_to_string($row_obj,$row_number)
  {
    $j="";
    $color=$this->display_row_colors[($row_number % $this->display_row_colors_count)];
    $id=$row_obj->scheduleid;
    $j.= "<tr>";
      $j.= "<td class=\"trainline\" style=\"background-color:".$color.";\" id=\"tl_".$id."\">".$row_obj->trainline."</td>";
      $j.= "<td class=\"routename\" style=\"background-color:".$color.";\" id=\"rn_".$id."\">".$row_obj->routename."</td>";
      $j.= "<td class=\"runnumber\" style=\"background-color:".$color.";\" id=\"rr_".$id."\">".$row_obj->runnumber."</td>";
      $j.= "<td class=\"operatorid\" style=\"background-color:".$color.";\" id=\"oi_".$id."\">".$row_obj->operatorid."</td>";
      $j.= "<td class=\"row_options\" style=\"background-color:".$color.";\" id=\"rwo_".$id."\">";
      $j.=   "<button class=\"edit_row\" id=\"edit_".$id."\">Edit</button>&nbsp;";
      $j.=   "<button class=\"delete_row\" id=\"del_".$id."\">Delete</button>&nbsp;";
      $j.=   "<button class=\"hide_rwo\" id=\"hid_".$id."\">Hide</button>&nbsp;";
      $j.= "</td>";
    $j.= "</tr>";
    return $j;
  } // end function row_to_string($row_obj,$row_number)

  public function remove_from_sview($scheduleid)
  {
    $this->mysqli->query("DELETE from sview_schedule where sviewid='".$this->sviewid."' and scheduleid='".trim($scheduleid)."';");
      if ($this->mysqli->error)
        store_message($this->mysqli->error);
  } // end public function remove_from_sview($scheduleid)

  // See if row already exists with given data (except scheduleid)
  // else create new schedule row.
  // locate old sview_schedule record and update it with the new scheduleid. (that's right, we're not deleting anything)
  public function update_sview($scheduleid,$trainline,$routename,$runnumber,$operatorid)
  {
    $new_scheduleid="";
    $result=$this->mysqli->query("SELECT scheduleid from schedule where trainline='".$trainline."' and routename='".$routename."' and runnumber='".$runnumber."' and operatorid='".$operatorid."';");
    if ($result===FALSE)
    {
      store_message($this->mysqli->error);
      $commit=FALSE;
    }
    else if ($result->num_rows>0)
      $new_scheduleid=$result->fetch_object()->scheduleid;
    else
    { // need to add the row..
      $result->close();
      if ($this->mysqli->query("INSERT into schedule (trainline,routename,runnumber,operatorid) VALUES ('".$trainline."','".$routename."','".$runnumber."','".$operatorid."');"))
        $new_scheduleid=$this->mysqli->insert_id;
      else
      {
        b_send_response(array('status' => 'error', 'message' => 'Unable to edit row: '.$this->mysqli->error),get_passed_data('callback'));
        return FALSE;
      }
    } // end if-else row with new data already exists

    if ($new_scheduleid=="")
    {
      b_send_response(array('status' => 'error', 'message' => 'Unable to edit row.'),get_passed_data('callback'));
      return FALSE;
    }

    // update link.
    if ($scheduleid!="new")
    {
      if ($this->mysqli->query("UPDATE sview_schedule SET scheduleid='".$new_scheduleid."' where sviewid='".$this->sviewid."' and scheduleid='".$scheduleid."';")===FALSE)
      {
        b_send_response(array('status' => 'error', 'message' => 'Unable to link row to view: '.$this->mysqli->error),get_passed_data('callback'));
        return FALSE;
      }
    }
    else
    { // it's new.. just insert it
      if ($this->mysqli->query("INSERT into sview_schedule (sviewid,scheduleid) VALUES ('".$this->sviewid."','".$new_scheduleid."');")===FALSE)
      {
        b_send_response(array('status' => 'error', 'message' => 'Unable to link row to view: '.$this->mysqli->error),get_passed_data('callback'));
        return FALSE;
      }

    }
    return TRUE;
  } // end public function update_sview

} // end class train_schedule
?>