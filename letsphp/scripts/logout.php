<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
session_start();
// send the sigoff request
$key = "0001 ";
$socket = unserialize($_SESSION['sock']);
socket_write($socket, $key,5);
socket_close($socket);
session_destroy();
header("Location: /index.php");
exit(0);
