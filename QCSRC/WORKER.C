/**
  *
  *  Revision log.:
  *  Date     Author    Revision
  *  2018     Chris H   1.0
  *
  *  Copyright (C) <2018>  <Chris Hird>
  *
  *  This program is free software; you can redistribute it and/or modify
  *  it under the terms of the GNU General Public License as published by
  *  the Free Software Foundation; either version 2 of the License, or
  *  (at your option) any later version.
  *
  *  This program is distributed in the hope that it will be useful,
  *  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *  GNU General Public License for more details.
  *
  *  You should have received a copy of the GNU General Public License
  *  with this program; if not, write to the Free Software Foundation,
  *  Inc.,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
  *  You can contact the author via Email chrish@shieldadvanced.com
  */
#include <H/MSGFUNC>                        // message functions
#include <H/COMMON>                         // common header
#include <netdb.h>                          // Socket DB
#include <sys/socket.h>                     // Sockets
#include <netinet/in.h>                     // Socket net
#include <arpa/inet.h>                      // ARPA structs
#include <errno.h>                          // Error return header
#include <netdb.h>                          // net DB header
#include <sys/errno.h>                      // sys error header
#include <arpa/inet.h>                      // inet header
#include <signal.h>                         // signal funcs
#include <except.h>                         // exception funcs
#include <iconv.h>                          // conversion header
#include <qtqiconv.h>                       // iconv header

#define _QSIZE 256
#define _CPYRGHT "Copyright @ Shield Advanced Solutions Ltd 1997-2018"
#pragma comment(copyright,_CPYRGHT)

// Signal catcher

void catcher(int sig) {
// you can do some messaging etc in this function, it is called
// when the kill signal is received from the TESTSVR program
exit(0);
}

int main(int argc, char **argv) {
int listen_sd, accept_sd = 0;               // file descriptors
int rc = 0;                                 // return codes
int ret = 0;                                // return vals
int Stop = 0;                               // stop flag
int i = 0;                                  // counter
int j = 0;                                  // counter
int offset = 0;                             // offset marker
int total_bytes = 0;                        // counter
int bytes_processed = 0;                    // counter
size_t insz;                                // input len
size_t outsz = _32K;                        // outbuf size
char recv_buf[_32K];                        // recv buffer
char convbuf[_32K];                         // conversion buffer
char msg_dta[_MAX_MSG];                     // message buffer
char *out_ptr;                              // buffer ptr
char *in_ptr;                               // buffer ptr
struct sockaddr_in addr;                    // socket struct
struct sigaction sigact;                    // Signal Action Struct
QtqCode_T jobCode = {0,0,0,0,0,0};          // (Job) CCSID to struct
QtqCode_T asciiCode = {819,0,0,0,0,0};      // (ASCII) CCSID from struct
iconv_t a_e_ccsid;                          // convert table struct
iconv_t e_a_ccsid;                          // convert table struct

// Set up the signal handler
sigemptyset(&sigact.sa_mask);
sigact.sa_flags = 0;
sigact.sa_handler = catcher;
sigaction(SIGTERM, &sigact, NULL);
// create the conversion tables
// ASCII to EBCDIC
a_e_ccsid = QtqIconvOpen(&jobCode,&asciiCode);
if(a_e_ccsid.return_value == -1) {
   sprintf(msg_dta,"QtqIconvOpen Failed %s",strerror(errno));
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   exit(-1);
   }
// EBCDIC to ASCII
e_a_ccsid = QtqIconvOpen(&asciiCode,&jobCode);
if(e_a_ccsid.return_value == -1) {
   iconv_close(a_e_ccsid);
   sprintf(msg_dta,"QtqIconvOpen Failed %s",strerror(errno));
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   exit(-1);
   }
// connect to the socket passed as argv[1]
listen_sd = atoi(argv[1]);
do {
   accept_sd = accept(listen_sd, NULL, NULL);
   setsockopt(accept_sd,SOL_SOCKET,SO_RCVBUF,CHAR_32K,sizeof(CHAR_32K));
   if(accept_sd < 0) {
      sprintf(msg_dta,"accept() failed",strerror(errno));
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      close(listen_sd);
      exit(-1);
      }
   memset(recv_buf,'\0',_32K);
   rc = recv(accept_sd, recv_buf, _32K, 0);
   sprintf(msg_dta,"Bytes received %d %s",rc,recv_buf);
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   if(rc < 0) {
      sprintf(msg_dta,"recv() failed",strerror(errno));
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      close(listen_sd);
      close(accept_sd);
      exit(-1);
      }
   // convert the input
   memset(convbuf,'\0',_32K);
   in_ptr = recv_buf;
   out_ptr = convbuf;
   insz = rc;
   outsz = _32K;
   ret = (iconv(a_e_ccsid,(char **)&(in_ptr),&insz,(char **)&(out_ptr),&outsz));
   // send the data we just received to a message queue
   snd_msg("GEN0001",convbuf,strlen(convbuf));
   // send a reply to the client
   sprintf(msg_dta,"Yep got that now closing");
   in_ptr = msg_dta;
   out_ptr = convbuf;
   insz = strlen(msg_dta);
   outsz = _32K;
   ret = (iconv(e_a_ccsid,(char **)&(in_ptr),&insz,(char **)&(out_ptr),&outsz));
   rc = send(accept_sd,convbuf,strlen(msg_dta),0);
   close(accept_sd);
   } while(Stop == 0);
}

