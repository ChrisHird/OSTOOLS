<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
session_start();

function get_messages() {
    $socket = unserialize($_SESSION['sock']);
    $key = "0003 ";
    $output = "";
    socket_write($socket, $key,5);
    do {
        $output = socket_read($socket, 2048, PHP_BINARY_READ);
        echo($output);
    } while($output);
    echo("<br />");
}