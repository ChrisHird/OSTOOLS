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
#include <recio.h>                          // record IO
#include <errno.h>                          // error number

#pragma linkage (COPYSRCA,OS,nowiden)
void COPYSRCA(char *,char *,char *);

// function Crt_Q_Name()
// Purpose: To create a qualified object name. LIB/OBJ
// @parms
//      string object
//      string q name
// returns 1 success

int Crt_Q_Name(char *Object,
               char *Q_Name) {
int i,j = 0;                                // counters

for(i = 10,j = 0; i < 20; i++,j++) {
   Q_Name[j] = (Object[i] == ' ') ? '\0' : Object[i];
   }
Q_Name[j] = '\0';
strcat(Q_Name,"/");
j = strlen(Q_Name);
for(i = 0;i < 10;i++,j++) {
   Q_Name[j] = (Object[i] == ' ') ? '\0' : Object[i];
   }
Q_Name[j] = '\0';
return 1;
}

// function check_recs()
// Purpose: Read through the records looking for string
// @parms
//      string to find
// returns 1 success

int check_recs(_RFILE *fp,
               char *MsgId,
               char *MsgF) {
_RIOFB_T *fdbk;                             // feed back
int found = 0;                              // counter
int rec_len = 150;                          // record length
char buf[151] = {'\0'};                     // read buffer
char cmd[256];                              // command string

do {
   fdbk = _Rreadn(fp,buf,rec_len,__NO_LOCK);
   if(strstr(buf,MsgId)) {
      found = 1;
      break;
      }
   }while(fdbk->num_bytes != EOF);
// if we did not find the message ID in the source we could delete it
// we prompt the command so we could cancel if required
if(found == 0) {
   // remove the messaid from the file
   sprintf(cmd,"?RMVMSGD MSGID(%s) MSGF(%s)",MsgId,MsgF);
   system(cmd);
   }
return 1;
}

int main(int argc, char** argv) {
_RFILE *fp;                                 // file ptr
_RIOFB_T *fdbk;                             // feed back
int rec_len = 150;                          // record length
int Rep_Dta_Len = 0;                        // length of replacement data passed
char UsrSpc[20] = "RTVMSG    QTEMP     ";   // Userspace returned data
char Rep_Dta[16] = {'0'};                   // replacement data
char Rep_Sub_Vals[10] = "*NO       ";       // replace substitution values
char Ret_Fmt_Ctl[10] = "*NO       ";        // return format control chars
char Rtv_Opt[10] = "*FIRST    ";            // retrieval option
char Msg_Id[7] = {' '};                     // returned message ID
char msg_id[8] = {'\0'};                    // message id to look for
char msg_dta[_MAX_MSG];                     // message buffer
char NewSrc[22];                            // new srource file
char MsgF[22];                              // message file
char *space;                                // pointer to userspace
Qmh_Rtvm_RTVM0300_t *MsgInfo;               // pointer to returned structure
Os_EC_t Error_Code;                         // error Code

Error_Code.EC.Bytes_Provided = sizeof(Error_Code);
// create the source file names etc and copy the source
Crt_Q_Name(argv[1],MsgF);
Crt_Q_Name(argv[3],NewSrc);
// create the temporary source file and populate
COPYSRCA(argv[2],argv[3],argv[4]);
// open the file
if((fp = _Ropen(NewSrc,"rr")) == NULL)  {
   sprintf(msg_dta,"Open failed %s: %s\n",NewSrc,strerror(errno));
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   exit(1);
   }
// get ptr to usrspc for messages
if(Get_Spc_Ptr(UsrSpc,&space,_4MB) != 1) {
   _Rclose(fp);
   exit(-1);
   }
// get the first message from message file
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
   _Rclose(fp);
   exit(-1);
   }
MsgInfo = (Qmh_Rtvm_RTVM0300_t *)space;
if(MsgInfo->Bytes_Available <= 0) {
   sprintf(msg_dta,"no message found\n");
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   _Rclose(fp);
   exit(0);
   }
memcpy(Rtv_Opt,"*NEXT     ",10);
do {
   memcpy(msg_id,MsgInfo->Message_ID,7);
   //printf("Checking %s\n",msg_id);
   // set to start of file each time
   _Rlocate(fp,NULL,0,__START);
   check_recs(fp,msg_id,MsgF);
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
      _Rclose(fp);
      exit(-1);
      }
   }while(MsgInfo->Bytes_Available > 0);
_Rclose(fp);
exit(0);
}
