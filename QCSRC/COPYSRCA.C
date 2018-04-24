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

#include <recio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// function cpy_src()
// Purpose: To copy the source to a single member for better reads
// @parms
//      NONE
// returns 1 success

int cpy_src(char *NewSrc,
            char *OldSrc,
            char *filter,
            int rec_len) {
_RFILE *fp;                                 // file ptr
_RFILE *fp1;                                // file ptr
_RIOFB_T *fdbk;                             // feed back
_RIOFB_T *fdbk1;                            // feed back
char cmd[256];                              // command string
char SrcPf[32];                             // source file
char buf[151] = {'\0'};                     // read buffer
char *code;                                 // code ptr

// create the new srource file
sprintf(cmd,"CHKOBJ OBJ(%s) OBJTYPE(*FILE)",NewSrc);
if(system(cmd) != 0) {
   sprintf(cmd,"CRTSRCPF FILE(%s) MBR(SRCMBR) RCDLEN(%d)",NewSrc,rec_len);
   system(cmd);
   }
// open the old source file
sprintf(SrcPf,"%s(*ALL)",OldSrc);
if((fp = _Ropen(SrcPf, "rr" )) == NULL )  {
   printf ( "Open failed %s\n",SrcPf);
   exit(1);
   }
// target file is a single member
if((fp1 = _Ropen(NewSrc,"rr+")) == NULL )  {
   printf ( "Open failed %s\n",NewSrc);
   _Rclose(fp);
   exit(1);
   }
code = buf + 12;
do {
   fdbk = _Rreadn(fp,buf,rec_len,__NO_LOCK);
   // include the line if it has the filter string
   if(strstr(buf,filter)) {
      fdbk1 = _Rwrite(fp1,buf,rec_len);
      if(fdbk1->num_bytes != rec_len) {
         printf("Failed to add record\n");
         _Rclose(fp);
         _Rclose(fp1);
         return(-1);
         }
      }
   }while(fdbk->num_bytes != EOF);
_Rclose(fp);
_Rclose(fp1);
return 1;
}

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

int main(int argc, char **argv) {
int rec_len = 150;                          // record length
short int filter_len = 0;                   // filter length
char NewSrc[22];                            // new srource file
char OldSrc[22];                            // old source file
char filter[51] = {'\0'};                   // filter string
char *tmp;                                  // temp ptr

Crt_Q_Name(argv[1],OldSrc);
Crt_Q_Name(argv[2],NewSrc);
// set up the filter string
filter_len = *(short int *)argv[3];
tmp = argv[3];
tmp += sizeof(short int);
memcpy(filter,tmp,filter_len);
cpy_src(NewSrc,OldSrc,filter,rec_len);
return 1;
}
