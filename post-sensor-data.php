<!DOCTYPE html>
<html><body>
<?php

$servername = "localhost";

// REPLACE with your Database name
$dbname = "particlesensor";
// REPLACE with Database user
$username = "pmsuser";
// REPLACE with Database user password
$password = "enter_password_here";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection

$api_key_value = "api_key_must_match_ino_code";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $datetime = date ('Y-m-d H:i:s', $phptime);
        $pm10_standard = test_input($_POST["pm10_standard"]);
        $pm25_standard = test_input($_POST["pm25_standard"]);
        $pm100_standard = test_input($_POST["pm100_standard"]);
        $pm10_env = test_input($_POST["pm10_env"]);
        $pm25_env = test_input($_POST["pm25_env"]);
        $pm100_env = test_input($_POST["pm100_env"]);
        $particles_03um = test_input($_POST["particles_03um"]);
        $particles_05um = test_input($_POST["particles_05um"]);
        $particles_10um = test_input($_POST["particles_10um"]);
        $particles_25um = test_input($_POST["particles_25um"]);
        $particles_50um = test_input($_POST["particles_50um"]);
        $particles_100um = test_input($_POST["particles_100um"]);
        $analogvalue = test_input($_POST["analogvalue"]);
        $analogvolts = test_input($_POST["analogvolts"]);
        $batteryvolts = test_input($_POST["batteryvolts"]);
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO pms5003 (datetime, pm10_standard, pm25_standard, pm100_standard, pm10_env, pm25_env, pm100_env, particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um, analogvalue, analogvolts, batteryvolts) VALUES ('" . $datetime . "','" . $pm10_standard . "','" . $pm25_standard . "','" . $pm100_standard . "','" . $pm10_env . "','" . $pm25_env . "','" . $pm100_env . "','" . $particles_03um . "','" . $particles_05um . "','" . $particles_10um . "','" . $particles_25um . "','" . $particles_50um . "','" . $particles_100um . "','" . $analogvalue . "','" . $analogvolts . "','" . $batteryvolts . "')";
        
        if ($result = $conn->query($sql) !== FALSE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();
    }
    else {
        echo "Wrong API Key provided.";
    }

}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
