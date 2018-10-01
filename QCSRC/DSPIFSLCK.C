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
#include <qp0lror.h>                        // list IFS references

int main(int argc, char **argv) {
int pathLen = 0;                            // path length
int i = 0;                                  // counter
char Spc_Name[20] = "IFSINFO   QTEMP     "; // User space name
char *path_ptr;                             // path ptr
char *space;                                // ptr to usrspc
char *tmp;                                  // temp ptr
Qp0l_RORO0200_Output_T *o2P;                // output ptr
Qp0l_Job_Using_Object_T *jobP;              // job structure ptr
IFS_Info_t pathInfo;                        // nls path struct
Os_EC_t Error_Code;                         // error code struct

Error_Code.EC.Bytes_Provided = _ERR_REC;
// create user space for output _4MB should be plenty
if(Get_Spc_Ptr(Spc_Name,&space,_4MB) != 1)
   exit(-1);
// set up path passed in
tmp = argv[1];
pathLen = *(short int *)argv[1];
tmp += sizeof(short int);
path_ptr = tmp;
o2P = (Qp0l_RORO0200_Output_T *)space;
// set up the path to use
memset(&pathInfo, 0, sizeof(pathInfo));
pathInfo.header.CCSID = 37;
memcpy(pathInfo.header.Country_ID,"US",2);
memcpy(pathInfo.header.Language_ID,"ENU",3);
pathInfo.header.Path_Type = QLG_CHAR_SINGLE;
pathInfo.header.Path_Length = pathLen;
pathInfo.header.Path_Name_Delimiter[0] = '/';
memcpy(pathInfo.path,path_ptr, pathLen);
QP0LROR(space,
        _4MB,
        QP0LROR_RORO0200_FORMAT,
        (Qlg_Path_Name_T *)&pathInfo,
        &Error_Code);
if(Error_Code.EC.Bytes_Available != 0) {
   snd_error_msg(Error_Code);
   return;
   }
// Print some output.
printf("Reference count:  %d\n",o2P->Count);
printf("Jobs returned:    %d\n",o2P->JobsReturned);
//printf("Jobs Offset = %d\n",o2P->JobsOffset);
if(o2P->JobsReturned > 0) {
   tmp = space;
   tmp += o2P->JobsOffset;
   jobP = (Qp0l_Job_Using_Object_T *)tmp;
   for(i = 0; i < o2P->JobsReturned; i++) {
      printf("job: %.10s %.10s %.6s holding lock on path\n",i,jobP->Name,jobP->User,jobP->Number);
      }
   jobP++;
   }
exit(0);
}
