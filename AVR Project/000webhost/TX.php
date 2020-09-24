<?php
/*This file should receive a link somethong like this: http://noobix.000webhostapp.com/TX.php?unit=1&b1=1
If you paste that link to your browser, it should update b1 value with this TX.php file. Read more details below.
The ESP will send a link like the one above but with more than just b1. It will have b1, b2, etc...
*/

//We loop through and grab variables from the received the URL
foreach($_REQUEST as $key => $value)  //Save the received value to the hey variable. Save each cahracter after the "&"
{
	//Now we detect if we recheive the id, the password, unit, or a value to update
if($key =="id"){
$id = $value;
}	

if($key == "en1x"){
$en1x = $value;
}

if($key == "en1y"){
$en1y = $value;
}

if($key == "en2x"){
$en2x = $value;
}

if($key == "en2y"){
$en2y = $value;
}
}//End of foreach

//echo "_id$id##_angle$angle##_en1x$en1x##_en1y$en1y##_en2x$en2x##_en2y$en2y##";

include("database_connect.php"); 	//We include the database_connect.php which has the data for the connection to the database


// Check  the connection
if (mysqli_connect_errno()) {
  echo "Failed to connect to MySQL: " . mysqli_connect_error();
}

mysqli_query($con, "LOCK TABLES ESPtable WRITE");

mysqli_query(
$con, 
"UPDATE ESPtable 
SET ENEMY1X = $en1x, ENEMY1Y = $en1y, 
ENEMY2X = $en2x, ENEMY2Y = $en2y
WHERE id=$id"
);

mysqli_query($con, "UNLOCK TABLES ESPtable");

mysqli_query($con, "LOCK TABLES ESPtable2 WRITE");

mysqli_query(
$con, 
"UPDATE ESPtable2 
SET ANGLE = $id
WHERE id=99999"
);

mysqli_query($con, "UNLOCK TABLES ESPtable2");
?>








