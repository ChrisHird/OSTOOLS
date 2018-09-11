<!DOCTYPE html>
<!--
To change this license header, choose License Headers in Project Properties.
To change this template file, choose Tools | Templates
and open the template in the editor.
-->
<html>
    <head>
        <meta charset="UTF-8">
        <title>Test Sign On</title>
        <?php
        // start the session processing for storing values
        session_start();
        ?>
    </head>
    <body>
        <?php
        if (isset($_SESSION['valid_user'])) {
            echo("You are signed in as :" . $_SESSION['valid_user']);
        } else {
            ?>
            <table>                    
                <form action="scripts/getmsg.php" method="POST">
                    <tr><td><label>Name : </label><input name="userid" type="text" value="" size="10" maxlength="10" /></td></tr>
                    <tr><td><label>Password : </label><input name="pwd" type="password" value="" size="10" /></td></tr>
                    <tr><td><input type="submit" value="Get Messages" /></td></tr>
                </form>
            </table>
            <?php
            if (isset($_SESSION['ErrMsg'])) {
                echo($_SESSION['ErrMsg'] . "<br />");
                $_SESSION['ErrMsg'] = NULL;
            }
        }
        ?>
    </body>
</html>
