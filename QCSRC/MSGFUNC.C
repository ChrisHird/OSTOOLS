//
// Copyright (c) 2018 Chris Hird
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// Disclaimer :
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#include <H/MSGFUNC>                        // MsgFunc header

// function snd_error_msg()
// Purpose: Forward an Error Message to the message queue.
// @parms
//      Error Code Structure
// returns void

void snd_error_msg(Os_EC_t Error_Code) {
int data_len = 0;
char Msg_Type[10] = "*INFO     ";           // msg type
char Msg_File[20] = "QCPFMSG   *LIBL     "; // Message file to use
char Msg_Key[4] = {' '};                    // msg key
char QRpy_Q[20] = {' '};                    // reply queue
Os_EC_t E_Code = {0};                       // error code struct

Error_Code.EC.Bytes_Provided = _ERR_REC;
data_len = Error_Code.EC.Bytes_Available - 16;  // BA + BP +msgid +rsvd
QMHSNDM(Error_Code.EC.Exception_Id,
        Msg_File,
        Error_Code.Exception_Data,
        data_len,
        Msg_Type,
        _DFT_MSGQ,
        1,
        QRpy_Q,
        Msg_Key,
        &E_Code);
if(E_Code.EC.Bytes_Available > 0) {
   // if we get an error on sending the message send it
   snd_error_msg(E_Code);
   }
return;
}

// function snd_msg()
// Purpose: Place a message in the message queue.
// @parms
//      string MsgID
//      string Msg_Data
//      int Msg_Dta_Len
// returns void

void snd_msg(char * MsgID,
             char * Msg_Data,
             int Msg_Dta_Len) {
char Msg_Type[10] = "*INFO     ";           // msg type
char Call_Stack[10] = {"*EXT      "};       // call stack entry
char QRpy_Q[20] = {' '};                    // reply queue
char Msg_Key[4] = {' '};                    // msg key
Os_EC_t Error_Code = {0};                   // error code struct

Error_Code.EC.Bytes_Provided = _ERR_REC;
// send the message to the message queue
QMHSNDM(MsgID,
        _DFT_MSGF,
        Msg_Data,
        Msg_Dta_Len,
        Msg_Type,
        _DFT_MSGQ,
        1,
        QRpy_Q,
        Msg_Key,
        &Error_Code);
if(Error_Code.EC.Bytes_Available > 0) {
   snd_error_msg(Error_Code);
   }
// add a diag message to the program message queue
QMHSNDPM(MsgID,
         _DFT_MSGF,
         Msg_Data,
         Msg_Dta_Len,
         "*DIAG     ",
         "*         ",
         0,
         Msg_Key,
         &Error_Code);
if(Error_Code.EC.Bytes_Available > 0) {
   snd_error_msg(Error_Code);
   }
return;
}


