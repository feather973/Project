<?php
include("database_connect.php"); 	//We include the database_connect.php which has the data for the connection to the database

// Check  the connection
if (mysqli_connect_errno()) {
  echo "Failed to connect to MySQL: " . mysqli_connect_error();
}

//Get all the values form the table on the database

$result = mysqli_query($con, "SELECT * FROM ESPtable2");
$row = mysqli_fetch_array($result);
echo $row['ANGLE'].',';

$result = mysqli_query($con, "SELECT * FROM ESPtable");

$i=0;

while($i++<=180){
    $row = mysqli_fetch_array($result);
    echo $row['ID'].',';	
    echo $row['ENEMY1X'].',';	
    echo $row['ENEMY1Y'].',';
    echo $row['ENEMY2X'].',';
    if($i==180) echo $row['ENEMY2Y'];
    else echo $row['ENEMY2Y'].',';
}


?>