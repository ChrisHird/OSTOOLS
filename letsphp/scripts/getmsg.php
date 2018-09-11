<?php
// set up some variables to be used
$key = "0000 ";
$name = $_POST['userid'];
$pass = $_POST['pwd'];
$server = 'fill in your server address here';
$port = 12345;
$bytes_read = 0;
$bytes_written = 0;
// get the host address
$address = gethostbyname($Server);
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if ($socket === false) {
    $_SESSION['ErrMsg'] = "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n";
    header("Location: /index.php");
    exit(0);
}
// connect to the remote host
$result = socket_connect($socket, $address, $port);
if ($result === false) {
    $_SESSION['ErrMsg'] = "socket_connect() failed.\nReason: ($result) " . socket_strerror(socket_last_error($socket)) . "\n";
    //header("Location: /index.php");
    exit(0);
}
// send the request to server
$bytes_written = socket_write($socket, $key, 5);
// the server now sends request for name 
$output = socket_read($socket, 2048, PHP_BINARY_READ);
// make sure we get what we expect
if ($output != "Please enter your Profile name : ") {
    $_SESSION['ErrMsg'] = "Incorrect data returned : " . $output;
    socket_close($socket);
    header("Location: /index.php");
    exit(0);
}
// we could check to make sure its the right info 
// now send the user name from above
$bytes_written = socket_write($socket, $name, strlen($name));
if ($bytes_written <= 0) {
    $_SESSION['ErrMsg'] = "Failed to write to socket : " . $bytes_written . " : " . $name;
    socket_close($socket);
    //header("Location: /index.php");
    exit(0);
}
// request for password
$output = socket_read($socket, 2048, PHP_BINARY_READ);
if ($output != "Please enter your Password : ") {
    $_SESSION['ErrMsg'] = "Incorrect data returned : " . $output;
    socket_close($socket);
    header("Location: /index.php");
    exit(0);
}
// send in the password
$bytes_written = socket_write($socket, $pass, strlen($pass));
if ($bytes_written <= 0) {
    $_SESSION['ErrMsg'] = "Failed to write to socket : " . $bytes_written . " : " . $pass;
    socket_close($socket);
    //header("Location: /index.php");
    exit(0);
}
// get the messages
$key = "0003 ";
$output = "";
socket_write($socket, $key, 5);
// get the message requesting the message queue
$output = socket_read($socket, 2048, PHP_BINARY_READ);
if ($output != "Please enter the message Queue Name : ") {
    $_SESSION['ErrMsg'] = "Incorrect data returned : " . $output;
    socket_close($socket);
    header("Location: /index.php");
    exit(0);
}
// send in the message queue
$mq = "QSYSOPR   *LIBL     ";
$bytes_written = socket_write($socket, $mq, 20);
// get the messages, they are sent one at a time and require OK to proceed
do {
    $output = socket_read($socket, 2048, PHP_BINARY_READ);
    echo($output ."<br />");
    // if end of messages then no need to confirm as remote loop will break
    if($output != "End of Messages : ") {
        $bytes_written = socket_write($socket,"OK",2);
    }    
} while ($output != "End of Messages : ");
echo("<br />");
// send the sign off key
socket_write($socket,"0001 ");
// close the socket
socket_close($socket);
exit(0);
