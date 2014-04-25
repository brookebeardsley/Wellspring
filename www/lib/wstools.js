$(document).ready(function(){
  $(".row_options").hide();

  $.fn.extend({
    filedrop: function (options) {
        var defaults = {
            callback : null
        }
        options =  $.extend(defaults, options)
        return this.each(function() {
            var files = []
            var $this = $(this)

            // Stop default browser actions
            $this.bind('dragover dragleave', function(event) {
                event.stopPropagation()
                event.preventDefault()
            })

            // Catch drop event
            $this.bind('drop', function(event) {
                // Stop default browser actions
                event.stopPropagation()
                event.preventDefault()

                // Get all files that are dropped
                files = event.originalEvent.target.files || event.originalEvent.dataTransfer.files

                // Convert uploaded file to data URL and pass trought callback
                if(options.callback) {
                    var reader = new FileReader()
                    reader.onload = function(event) {
                        options.callback(event.target.result)
                    }
                    reader.readAsText(files[0])
                }
                return false
            })
        })
    }
  })

  $("#header").filedrop({
     callback:function(e){
       $("#working").show();
       var ajaxrq=$.ajax({
         url:'service.php',
         dataType: 'jsonp',
         type: 'GET',
         data: {
                action:'upload',
                body:e
              },
         jsonpCallback: "jresp",
         success: function (data) {
         },
         error: function(handle,status,error) {
         }
       }); // end ajax

       ajaxrq.done(function(data){
         var ajax_resp=data;
           $("#working").hide();
           $("#content").html(ajax_resp.message);
           $(".row_options").hide();
       }); // end ajaxrq.done
     }
  }); // end header.filedrop

  $("#content").on("click", ".delete_row", function(e){
     var id=$(this).attr('id').substring(4);
     $("#working").show();
     var ajaxrq=$.ajax({
         url:'service.php',
         dataType: 'jsonp',
         type: 'GET',
         data: {
                action:'delete',
                scheduleid:id
              },
         jsonpCallback: "jresp",
         success: function (data) {
         },
         error: function(handle,status,error) {
         }
     }); // end ajax

     ajaxrq.done(function(data){
         var ajax_resp=data;
           $("#working").hide();
           $("#content").html(ajax_resp.message);
           $(".row_options").hide();
     }); // end ajaxrq.done
  }); // end delete_row click

  $("#content").on("click", ".edit_row", function(e){var id=$(this).attr('id').substring(5);editRow(id);});

/*
  $("#content").on("click", ".trainline", function(e){var id=$(this).attr('id').substring(3);editRow(id);});
  $("#content").on("click", ".routename", function(e){var id=$(this).attr('id').substring(3);editRow(id);});
  $("#content").on("click", ".runnumber", function(e){var id=$(this).attr('id').substring(3);editRow(id);});
  $("#content").on("click", ".operatorid", function(e){var id=$(this).attr('id').substring(3);editRow(id);});
*/

  $("#content").on("click", ".trainline", function(e){var id=$(this).attr('id').substring(3);$("#rwo_"+id).show();});
  $("#content").on("click", ".routename", function(e){var id=$(this).attr('id').substring(3);$("#rwo_"+id).show();});
  $("#content").on("click", ".runnumber", function(e){var id=$(this).attr('id').substring(3);$("#rwo_"+id).show();});
  $("#content").on("click", ".operatorid", function(e){var id=$(this).attr('id').substring(3);$("#rwo_"+id).show();});

  $("#content").on("click", ".hide_rwo", function(e){var id=$(this).attr('id').substring(4);$("#rwo_"+id).hide();});

  $("#content").on("click", ".cancel", function(e){var id=$(this).attr('id').substring(4);editCancel(id);});

  $("#content").on("click", ".save", function(e){var id=$(this).attr('id').substring(5);editSave(id);});

  $("#content").on("click", ".add_row", function(e){addRow();});

  function addRow()
  { // create div to display input fields.. then tack on other automation.  set "id" to new.
    if ($("#opt_new").length==0)
    {
      var optdiv=document.createElement('div');
      var new_html='';
      new_html+="<input id=\"itl_new\"/>&nbsp;&nbsp;";
      new_html+="<input id=\"irn_new\"/>&nbsp;&nbsp;";
      new_html+="<input id=\"irr_new\"/>&nbsp;&nbsp;";
      new_html+="<input id=\"ioi_new\"/>&nbsp;&nbsp;";
      new_html+="<button class=\"save\" id=\"save_new\">Save Changes</button>&nbsp<button class=\"cancel\" id=\"cxl_new\">Cancel</button>";

      $(optdiv).html(new_html)
             .appendTo(".add_div")
             .attr("id","opt_new")
             .attr("class","opt")
             .css("position","relative")
             .css("top",0)
             .css("left",0);
    } // end if does not exist yet
  } // end addRow

  function editRow(id)
  {
    if ($("#opt_"+id).length==0)
    {
    // for each td element (except "del_"), create an input box for it. and place in values.
    // append at the end of the row (floating div) an element with "Save" and "Cancel"
    makeEditable("tl_"+id);
    makeEditable("rn_"+id);
    makeEditable("rr_"+id);
    makeEditable("oi_"+id);

    displayEditOptions(id);

    } // end if I'm not currently in an edit
  } // end edit row

  function editCancel(id)
  {
    makeNormal("tl_"+id,"h");
    makeNormal("rn_"+id,"h");
    makeNormal("rr_"+id,"h");
    makeNormal("oi_"+id,"h");
    $("#opt_"+id).remove();
  }

  function makeEditable(tag)
  {
    var val=$("#"+tag).html();
    $("#"+tag).html("<input type=\"text\" id=\"i"+tag+"\" value=\""+val+"\" /><input type=\"hidden\" id=\"h"+tag+"\" value=\""+val+"\" />");
  }

  // iselect is "i"nput (for save) or "h"idden (for cancel)
  function makeNormal(tag,iselect)
  {
    $("#"+tag).html($("#"+iselect+tag).val());
  }

  function displayEditOptions(id)
  {
    var optdiv=document.createElement('div');
    $(optdiv).html("<button class=\"save\" id=\"save_"+id+"\">Save</button>&nbsp<button class=\"cancel\" id=\"cxl_"+id+"\">Cancel</button>")
             .appendTo("#rwo_"+id)
             .attr("id","opt_"+id)
             .css("position","relative")
             .css("top",0)
             .css("left",0);
  }

  function editSave(id)
  {
     $("#working").show();
     var ajaxrq=$.ajax({
         url:'service.php',
         dataType: 'jsonp',
         type: 'GET',
         data: {
                action:'update',
                trainline:$("#itl_"+id).val(),
                routename:$("#irn_"+id).val(),
                runnumber:$("#irr_"+id).val(),
                operatorid:$("#ioi_"+id).val(),
                scheduleid:id
              },
         jsonpCallback: "jresp",
         success: function (data) {
         },
         error: function(handle,status,error) {
         }
     }); // end ajax

     ajaxrq.done(function(data){
         var ajax_resp=data;
           $("#working").hide();
           $("#content").html(ajax_resp.message);
           $(".row_options").hide();
     }); // end ajaxrq.done
  } // end edit row

}); // end .ready
