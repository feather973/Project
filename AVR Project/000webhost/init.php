<?php
include("database_connect.php");

if (mysqli_connect_errno()) {
    echo "Failed to connect to MySQL: " . mysqli_connect_error();
}

mysqli_query($con, "UPDATE ESPtable SET ENEMY1X = 900, ENEMY1Y = 900, ENEMY2X = 900, ENEMY2Y = 900");

?>