000101180313//
000102180313// Copyright (c) 2018 Chris Hird
000103180313// All rights reserved.
000104180313//
000105180313// Redistribution and use in source and binary forms, with or without
000106180313// modification, are permitted provided that the following conditions
000107180313// are met:
000108180313// 1. Redistributions of source code must retain the above copyright
000109180313//    notice, this list of conditions and the following disclaimer.
000110180313// 2. Redistributions in binary form must reproduce the above copyright
000111180313//    notice, this list of conditions and the following disclaimer in the
000112180313//    documentation and/or other materials provided with the distribution.
000113180313//
000114180313// Disclaimer :
000115180313// This code is distributed in the hope that it will be useful,
000116180313// but WITHOUT ANY WARRANTY; without even the implied warranty of
000117180313// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
000118180313
000119180313#include <H/MSGFUNC>                        // MsgFunc header
000120180313
000124180313// function snd_error_msg()
000125180313// Purpose: Forward an Error Message to the message queue.
000126180313// @parms
000127180313//      Error Code Structure
000128180313// returns void
000129180313
000130180313void snd_error_msg(Os_EC_t Error_Code) {
000131180313int data_len = 0;
000132180313char Msg_Type[10] = "*INFO     ";           // msg type
000133180313char Msg_File[20] = "QCPFMSG   *LIBL     "; // Message file to use
000134180313char Msg_Key[4] = {' '};                    // msg key
000135180313char QRpy_Q[20] = {' '};                    // reply queue
000136180313Os_EC_t E_Code = {0};                       // error code struct
000137180313
000138180313Error_Code.EC.Bytes_Provided = _ERR_REC;
000139180313data_len = Error_Code.EC.Bytes_Available - 16;  // BA + BP +msgid +rsvd
000140180313QMHSNDM(Error_Code.EC.Exception_Id,
000141180313        Msg_File,
000142180313        Error_Code.Exception_Data,
000143180313        data_len,
000144180313        Msg_Type,
000145180313        _DFT_MSGQ,
000146180313        1,
000147180313        QRpy_Q,
000148180313        Msg_Key,
000149180313        &E_Code);
000150180313if(E_Code.EC.Bytes_Available > 0) {
000151180313   // if we get an error on sending the message send it
000152180313   snd_error_msg(E_Code);
000153180313   }
000154180313return;
000155180313}
000156180313
000157180313
