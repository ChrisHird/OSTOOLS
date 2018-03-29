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

#include <H/COMMON>                         // Common header
#include <H/MSGFUNC>                        // MsgFunc header
#include <H/SPCFUNC>                        // User Space header

// function Crt_Usr_Spc()
// Purpose: To create a Userspace object.
// @parms
//      string Name
//      int size
// returns 1 on sucess

int Crt_Usr_Spc(char *SPC_Name,
                int Initial_Size) {
char Ext_Atr[10] = "USRSPC    ";            // Ext attr USRSPC
char Initial_Value = '0';                   // Init val USRSPC
char Auth[10] = "*CHANGE   ";               // Pub aut to USRSPC
char SPC_Desc[50] = {' '};                  // USRSPC Description
char Replace[10] = "*YES      ";            // Replace USRSPC
Os_EC_t Error_Code = {0};                   // Error Code struct

Error_Code.EC.Bytes_Provided = _ERR_REC;
QUSCRTUS(SPC_Name,
         Ext_Atr,
         Initial_Size,
         &Initial_Value,
         Auth,
         SPC_Desc,
         Replace,
         &Error_Code,
         "*USER     ");
if(Error_Code.EC.Bytes_Available > 0) {
   snd_error_msg(Error_Code);
   return -1;
   }
return 1;
}

// function Get_Spc_Ptr()
// Purpose: Returns a pointer to the user space, creates the userspace if
//          it does not exist.
// @parms
//      User Space name
//      holder for Pointer to return
// returns 1 on sucess

int Get_Spc_Ptr(char *Name,
                void *ptr,
                int size) {
Os_EC_t Error_Code = {0};                  // Error Code

Error_Code.EC.Bytes_Provided = _ERR_REC;
// set the pointer to the *USRSPC
QUSPTRUS(Name,
         ptr,
         &Error_Code);
if(Error_Code.EC.Bytes_Available > 0) {
   if(memcmp(Error_Code.EC.Exception_Id,"CPF9801",7) == 0) {
      // It does not exist so create it
      if(!Crt_Usr_Spc(Name,size)) {
         return -1;
         }
      // now get the pointer
      QUSPTRUS(Name,
               ptr,
               &Error_Code);
      if(Error_Code.EC.Bytes_Available > 0) {
         snd_error_msg(Error_Code);
         return -1;
         }
      }
   else {
      snd_error_msg(Error_Code);
      return -1;
      }
   }
return 1;
}

