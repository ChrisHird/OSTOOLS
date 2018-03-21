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
#include <qmhrtvm.h>                        // retrieve message api header

// program is passed 2 parameters, argv_1 = message file, argv_2 = search string

int main(int argc, char **argv) {
int i = 0;                                  // counter
int found = 0;                              // string found
int Rep_Dta_Len = 0;                        // length of replacement data passed
short int len;                              // length of search string
char UsrSpc[20] = "RTVMSG    QTEMP     ";   // Userspace returned data
char Rep_Dta[16] = {'0'};                   // replacement data
char Rep_Sub_Vals[10] = "*NO       ";       // replace substitution values
char Ret_Fmt_Ctl[10] = "*NO       ";        // return format control chars
char Rtv_Opt[10] = "*FIRST    ";            // retrieval option
char Msg_Id[7] = {' '};                     // returned message ID
char search[51] = {'\0'};                   // search string
char cmd[8192];                             // command str
char str1[8192];                            // str buffer
char str2[8192];                            // str buffer
char *space;                                // pointer to userspace
char *search_str;                           // search string
char *tmp1;                                 // temp ptr
char *tmp2;                                 // temp ptr
Qmh_Rtvm_RTVM0300_t *MsgInfo;               // pointer to returned structure
Os_EC_t Error_Code;                         // error Code

// set up the error code structure used by the API
Error_Code.EC.Bytes_Provided = sizeof(Error_Code);
// get a pointer to the user space for the returned data
if(Get_Spc_Ptr(UsrSpc,&space,_4MB) != 1)
   return -1;
// get the length of the string passed
len = *(short int *)argv[2];
//printf("Len = %d\n",len);
// null terminate the string we are looking for
search_str = (char *)argv[2];
search_str += sizeof(short int);
memcpy(search,search_str,len);
//printf("Search string = %s\n",search);
// retrieve all messages
QMHRTVM(space,
        _4MB,
        "RTVM0300",
        Msg_Id,
        argv[1],
        Rep_Dta,
        Rep_Dta_Len,
        Rep_Sub_Vals,
        Ret_Fmt_Ctl,
        &Error_Code,
        Rtv_Opt,
        0,
        0);
if(Error_Code.EC.Bytes_Available > 0) {
   snd_error_msg(Error_Code);
   exit(-1);
   }
MsgInfo = (Qmh_Rtvm_RTVM0300_t *)space;
// read through all of the messages in the file
if(MsgInfo->Bytes_Available > 0) {
   memcpy(Rtv_Opt,"*NEXT     ",10);
   do {
      // flag if message contains string, set to 0 (NO) each loop
      found = 0;
      // message text search
      //printf("Length = %d Offset = %d\n",MsgInfo->Length_Message_Returned,MsgInfo->Offset_Message_Returned);
      if(MsgInfo->Length_Message_Returned > 0) {
         tmp1 = space;
         tmp1 += MsgInfo->Offset_Message_Returned;
         memcpy(str1,tmp1,MsgInfo->Length_Message_Returned);
         memset(&str1[MsgInfo->Length_Message_Returned],'\0',1);
         //look for the text string
         if(strstr(str1,search))
            found++;
         }
      // help text search
      //printf("Help Length = %d Offset = %d\n",MsgInfo->Length_Help_Returned,MsgInfo->Offset_Help_Returned);
      if(MsgInfo->Length_Help_Returned > 0) {
         tmp2 = space;
         tmp2 += MsgInfo->Offset_Help_Returned;
         memcpy(str2,tmp2,MsgInfo->Length_Help_Returned);
         memset(&str2[MsgInfo->Length_Help_Returned],'\0',1);
         if(strstr(str2,search))
            found++;
         }
      //printf("found = %d Msg = %.7s\n",found,MsgInfo->Message_ID);
      if(found > 0) {
         sprintf(cmd,"?CHGMSGD MSGID(%.7s) MSGF(HAMN10SRC/HAMNMSGF11) MSG('%s')",MsgInfo->Message_ID,str1);
         if(memcmp(str2,"*NONE",5) != 0) {
            strcat(cmd," SECLVL('");
            strcat(cmd,str2);
            strcat(cmd,"')");
            }
         //printf("%s\n",cmd);
         system(cmd);
         }
      // get the next message
      memcpy(Msg_Id,MsgInfo->Message_ID,7);
      //printf("%.7s\n",Msg_Id);
      QMHRTVM(space,
              _4MB,
              "RTVM0300",
              Msg_Id,
              argv[1],
              Rep_Dta,
              Rep_Dta_Len,
              Rep_Sub_Vals,
              Ret_Fmt_Ctl,
              &Error_Code,
              Rtv_Opt,
              0,
              0);
      if(Error_Code.EC.Bytes_Available > 0) {
         snd_error_msg(Error_Code);
         exit(-1);
         }
      }while(MsgInfo->Bytes_Available > 0);
   }
return 1;
}
