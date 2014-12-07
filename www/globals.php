<?php
  define('TTF_DIR', '/usr/share/fonts/truetype/msttcorefonts/');
  $param_date_format='Y-m-d';
  $user_date_format='l, F jS Y H:i';
  $db_name = 'dht22';
  
  if(isset($_GET["id"])) {
    $conn=mysqli_connect("", "", "", $db_name);
    if (mysqli_connect_errno()) {
      exit('Failed to connect to MySQL: ' . mysqli_connect_error());
    } 
    $id = htmlspecialchars($_GET["id"]);  
  
    $result=mysqli_query($conn,"SELECT * from cores WHERE id=$id");
    $row = mysqli_fetch_array($result);
    $user_timezone=$row['tz'];
    
    $result=mysqli_query($conn,"SELECT name from cores WHERE id=$id");
    $sensor_name_row = mysqli_fetch_array($result);
    $sensor_name=$sensor_name_row['name'];    
  }
?>
