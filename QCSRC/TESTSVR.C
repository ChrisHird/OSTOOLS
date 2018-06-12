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

#include <H/COMMON>                         // common header
#include <H/MSGFUNC>                        // message functions
#include <qrcvdtaq.h>                       // Receive Data Q Msg
#include <qclrdtaq.h>                       // Clear Data Q Msgs
#include <sys/socket.h>                     // socket
#include <netinet/in.h>                     // net addr
#include <spawn.h>                          // spawn job
#include <errno.h>                          // error number
#include <signal.h>                         // Exception signals

typedef _Packed struct pid_list_x{
         int pid_num[50];
         }pid_list_t;

#define _QSIZE 256
#define _CPYRGHT "Copyright @ Shield Advanced Solutions Ltd 1997-2018"
#pragma comment(copyright,_CPYRGHT)

main(int argc, char *argv[]) {
int on = 1;                                 // on flag
int result = 0;                             // result flag
int stop = 0;                               // stop flag
int i, pid,rc,j;                            // counters
int listen_sd, accept_sd = 0;               // socket descriptors
int num_srv = 1;                            // number of servers
int Server_Port = 12345;                    // server port
int type = 0;                               // Switch key
decimal(5,0)  DataLength = 0.0d;            // Number of bytes returned
decimal(5,0)  WaitTime = -1.0d;             // wait for data <0 = forever
decimal(3,0)  SInfLength = 9.0d;            // length of Sender inf
decimal(3,0)  KeyLength = 4.0d;             // length of key
char *spawn_argv[3];                        // Spawn arg
char *spawn_envp[1];                        // Spawn Environment
char *recptr;                               // pointer to data
char buffer[80];                            // Buffer
char DQKey[4] = "0000";                     // key data used for retvl
char QueueData[_QSIZE];                     // Data from Data queue
char DQueue1[10] = "SVRCTLQ   ";            // Master Q
char DQLib[10] = "OSPGM     ";              // Master Q Lib
char msg_dta[100] = {'\0'};                 // msg buffer
char SpawnStr[50];                          // spawn string
char Key[5];                                // Switch Key
pid_list_t pid_list;                        // Process List struct
pid_list_t ss_pid_list;                     // Process List struct
Qmhq_Sender_Information_t SInfo;            // Sender Inf struct
struct inheritance inherit;                 // inheritance Struct
struct sockaddr_in addr;                    // socket struct
Os_EC_t Error_Code = {0};                   // Error code data

Error_Code.EC.Bytes_Provided = _ERR_REC;
// set up the worker program spawn string
strcpy(SpawnStr,"/QSYS.LIB/OSPGM.LIB/WORKER.PGM");
// Clear out any existing stop messages in control queue
QCLRDTAQ(DQueue1,
         DQLib,
         "EQ",
         KeyLength,
         DQKey,
         &Error_Code);
if(Error_Code.EC.Bytes_Available) {
   sprintf(msg_dta,"Unable to clear the data queue %.7s",Error_Code.EC.Exception_Id);
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   }
// Set up the listening sockets Non Secure
listen_sd = socket(AF_INET, SOCK_STREAM, 0);
if(listen_sd < 0) {
   sprintf(msg_dta," socket() failed %s",strerror(errno));
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   close(accept_sd);
   exit(-1);
   }
setsockopt(listen_sd,SOL_SOCKET,SO_REUSEADDR,(char *)&on,sizeof(on));
setsockopt(listen_sd,SOL_SOCKET,SO_RCVBUF,CHAR_32K,sizeof(CHAR_32K));
memset(&addr, 0, sizeof(addr));
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = htonl(INADDR_ANY);
addr.sin_port = htons(Server_Port);
rc = bind(listen_sd,(struct sockaddr *) &addr, sizeof(addr));
if(rc < 0)  {
   sprintf(msg_dta," bind() failed %s",strerror(errno));
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   close(listen_sd);
   exit(-1);
   }
rc = listen(listen_sd, 5);
if(rc < 0) {
   sprintf(msg_dta," bind() failed %s",strerror(errno));
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   close(listen_sd);
   exit(-1);
   }
memset(&inherit, 0, sizeof(inherit));
sprintf(buffer, "%d", listen_sd);
spawn_argv[0] = "";
spawn_argv[1] = buffer;
spawn_argv[2] = NULL;
spawn_envp[0] = NULL;
// load the listening jobs Non Secure
for(i = 0; i < num_srv; i++)  {
   pid = spawn(SpawnStr,listen_sd + 1, NULL, &inherit,spawn_argv, spawn_envp);
   if(pid < 0)  {
      sprintf(msg_dta," spawn() failed %s",strerror(errno));
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      close(listen_sd);
      exit(-1);
      }
   pid_list.pid_num[i] = pid;
   }
// Check for signal to end
do {
   recptr = QueueData;
   QRCVDTAQ(DQueue1,
            DQLib,
            &DataLength,
            &QueueData,
            WaitTime,
            "LE",
            KeyLength,
            &DQKey,
            SInfLength,
            &SInfo);
   memset(Key,'\0',5);
   memcpy(Key,DQKey,4);
   type = atoi(Key);
   switch(type)  {
      case  0     :   {  // End the process */
         for(i = 0; i < num_srv; i++) {
            kill(pid_list.pid_num[i], SIGTERM);
            }
         stop = 1;
         break;
         }
      Default :  { // do nothing
         break;
         }
      }
   }while(!stop);
close(listen_sd);
exit(0);
}

