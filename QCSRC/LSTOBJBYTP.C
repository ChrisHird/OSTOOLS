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

#include <H/MSGFUNC>                        // Message headers
#include <H/SPCFUNC>                        // User Space functions
#include <H/COMMON>                         // common header file
#include <quslobj.h>                        // list Objects
#include <qusdltus.h>                       // delete user psace


int main(int argc, char **argv) {
int i = 0;                                  // counter
char Obj_SPC[20] = "QUSLOBJ   QTEMP     ";  // user space
char Lib_Info[20] = "*ALL      ";           // library info
char *List_Section;
Qus_OBJL0100_t *Entry_List;                 // USRSPC Pointer
Qus_Generic_Header_0100_t *space;           // USRSPC Pointer
Os_EC_t Error_Code = {0};                   // Error Code Struct

Error_Code.EC.Bytes_Provided = _ERR_REC;
// copy the library name into place
memcpy(&Lib_Info[10],argv[1],10);
// get a pointer to a user space
if(Get_Spc_Ptr(Obj_SPC,&space,_4MB) != 1) {
   exit(-1);
   }
// send out message about the API we are going to call
snd_msg("MSG0001","QUSLOBJ   ",10);
// call the API
QUSLOBJ(Obj_SPC,
        "OBJL0100",
        Lib_Info,
        argv[2],
        &Error_Code);
if(Error_Code.EC.Bytes_Available > 0) {
   snd_error_msg(Error_Code);
   exit(-1);
   }
// set up the pointers to the information returned.
List_Section = (char *)space;
List_Section = List_Section + space->Offset_List_Data;
Entry_List = (Qus_OBJL0100_t *)List_Section;
/* Check that some objects were returned if yes create audit date   */
if(space->Number_List_Entries < 1) {
   printf("Nothing returned\n");
   exit(0);
   }
for(i = 0; i < space->Number_List_Entries; i++) {
   printf("Object %.10s type %.10s\n",Entry_List->Object_Name_Used,Entry_List->Object_Type_Used);
   }
snd_msg("MSG0001","QUSDLTUS  ",10);
QUSDLTUS(Obj_SPC,
         &Error_Code);
if(Error_Code.EC.Bytes_Available > 0) {
   snd_error_msg(Error_Code);
   exit(-1);
   }
exit(0);
}
