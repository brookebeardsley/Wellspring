Installation Instructions:

1) Copy the contents of "www" to the a location the http server can host it.  This is our "web directory".
2) Copy the contents of "www_res" to a non-webdirectory on the same machine.  This is the "resource directory".
3) Locate config.inc (in the webdirectory), and edit $wwwdir to location of where you placed files in step 1.
4) Locate config.inc (in the webdirectory), and edit $resdir to location of where you placed files in step 2.
5) In the resource directory, locate "Other/ws.sql":  Review and edit it if needed (in case you want to change the database / user names).
   Then run the sql script to create the database structure required for this application. I suggest (phpMyAdmin, but the command prompt works too.)
6) In the resource directory, edit "lib/include_utility.inc": locate the function "public_db_connect()" and update the parameters required for mysql access: <server>,<user>,<password>,<database>
7) (optional) In the resource directory, review "pages/body.inc":  You can choose to have it import a csv from here, if you would like, just enter in the file path and uncomment out some sections.
8) Open up the site using a browser (I primarially tested on Chrome, but it seems to do ok in Firefox)
9) Read more instructions on the bottom of the webpage (hopefully you could figure it out without reading..)


Other notes:
sample.csv is a sample file that you provided
brooke.csv is a just another test file (yes, it has a duplicate entry from sample.csv)

Known probable issues:
1) CSV data checking has not been implemented
  -> I expect it to error if a CSV is submitted that does not contain the 4 required column headers.
2) No ability to save / recall the schedule "view".  At present, the default name for your view is your php session_id value.
3) No validation checking on the front end js.
4) limited validation checking of the back end js:
    -> drag and drop import data stream is not validated: possible SQL injection attack.
    -> manual "add rows" is validated against attacks, but "trainline" is not validated against the list provided. (the sql server does validate this data though.  inserts "" if invalid.)
5) No garbage collection routine implemented to identify abandoned schedule / sview_schedule records and purge / mark them for deletion.
6) front-end user interface could be upgraded so that it scrolls as more entries are loaded.
7) Front-end options menu should be cleaned up.