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
#include <H/SRVFUNC>                        // Server functions
#include <qsyphandle.h>                     // profile handles

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
int len = 0;                                // msg_dta length
int offset = 0;                             // offset marker
int total_bytes = 0;                        // counter
int bytes_processed = 0;                    // counter
int valid_user = 0;                         // valid user flag
int req = 0;                                // request flag
char recv_buf[_32K];                        // recv buffer
char convBuf[_32K];                         // conversion buffer
char msg_dta[1024];                         // message buffer
char Usr_hdl[12];                           // Usr profile handle
char Cur_hdl[12];                           // Current profile handle
char key[5] = {'\0'};                       // request key
struct sockaddr_in addr;                    // socket struct
struct sigaction sigact;                    // Signal Action Struct
QtqCode_T jobCode = {0,0,0,0,0,0};          // (Job) CCSID to struct
QtqCode_T asciiCode = {819,0,0,0,0,0};      // (ASCII) CCSID from struct
iconv_t a_e_ccsid;                          // convert table struct
iconv_t e_a_ccsid;                          // convert table struct
Os_EC_t Error_Code = {0};                   // error struct

Error_Code.EC.Bytes_Provided = _ERR_REC;

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
// get the current profile handle for the job
QsyGetProfileHandleNoPwd(Cur_hdl,
                         "*CURRENT  ",
                         "*NOPWDCHK ",
                         &Error_Code);
if(Error_Code.EC.Bytes_Available) {
   snd_error_msg(Error_Code);
   exit(-1);
   }
// connect to the socket passed as argv[1]
listen_sd = atoi(argv[1]);
do {
   // only accept new connection if not already set
   if(valid_user == 0) {
      accept_sd = accept(listen_sd, NULL, NULL);
      setsockopt(accept_sd,SOL_SOCKET,SO_RCVBUF,CHAR_32K,sizeof(CHAR_32K));
      if(accept_sd < 0) {
         sprintf(msg_dta,"accept() failed",strerror(errno));
         snd_msg("GEN0001",msg_dta,strlen(msg_dta));
         close(listen_sd);
         exit(-1);
         }
      }
   memset(recv_buf,'\0',_32K);
   rc = recv(accept_sd, recv_buf, _32K, 0);
   // at this point we need to get the key information and switch to sort
   // the first 4 bytes will be the key and it must be sent separately the enter key is also passed
   if(rc == 5) {
      convert_buffer(recv_buf,convBuf,rc,_32K,a_e_ccsid);
      memcpy(key,convBuf,4);
      //sprintf(msg_dta,"Key : %s",key);
      //snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      req = atoi(key);
      switch(req) {
         case  0 :
            // signon request
            if(Handle_SO(accept_sd,Usr_hdl,a_e_ccsid,e_a_ccsid) != 1) {
               sprintf(msg_dta,"Sign On failed : ");
               snd_msg("GEN0001",msg_dta,strlen(msg_dta));
               len = strlen(msg_dta);
               convert_buffer(msg_dta,convBuf,len,_32K,e_a_ccsid);
               rc = send(accept_sd,convBuf,len,0);
               close(accept_sd);
               }
            valid_user = 1;
            break;
         case 1 :
            // sign off request
            if(valid_user == 1) {
               QsySetToProfileHandle(Cur_hdl,
                                     &Error_Code);
               if(Error_Code.EC.Bytes_Available > 0) {
                  snd_error_msg(Error_Code);
                  }
               // release the profile handle for the user
               QsyReleaseProfileHandle(Usr_hdl,
                                       &Error_Code);
               // send closing message
               sprintf(msg_dta,"OK signing off and closing socket : ");
               len = strlen(msg_dta);
               convert_buffer(msg_dta,convBuf,len,_32K,e_a_ccsid);
               rc = send(accept_sd,convBuf,len,0);
               // close the socket
               close(accept_sd);
               valid_user = 0;
               }
            break;
         case 2 :
            if(valid_user == 1) {
               if(handle_CM(accept_sd,a_e_ccsid,e_a_ccsid) == 1) {
                  sprintf(msg_dta,"Command succeeded");
                  len = strlen(msg_dta);
                  convert_buffer(msg_dta,convBuf,len,_32K,e_a_ccsid);
                  rc = send(accept_sd,convBuf,len,0);
                  }
               }
            else {
               sprintf(msg_dta,"Must be signed on to send request");
               len = strlen(msg_dta);
               convert_buffer(msg_dta,convBuf,len,_32K,e_a_ccsid);
               rc = send(accept_sd,convBuf,len,0);
               }
            break;
         case 3 :
            if(valid_user == 1) {
               if(handle_MR(accept_sd,a_e_ccsid,e_a_ccsid) != 1) {
                  sprintf(msg_dta,"Failed to retrieve messages");
                  len = strlen(msg_dta);
                  convert_buffer(msg_dta,convBuf,len,_32K,e_a_ccsid);
                  rc = send(accept_sd,convBuf,len,0);
                  }
               }
            else {
               sprintf(msg_dta,"Must be signed on to send request");
               len = strlen(msg_dta);
               convert_buffer(msg_dta,convBuf,len,_32K,e_a_ccsid);
               rc = send(accept_sd,convBuf,len,0);
               }
            break;
         default :
               sprintf(msg_dta,"Unknown Request : ");
               snd_msg("GEN0001",msg_dta,strlen(msg_dta));
               len = strlen(msg_dta);
               convert_buffer(msg_dta,convBuf,len,_32K,e_a_ccsid);
               rc = send(accept_sd,convBuf,strlen(msg_dta),0);
            break;
         } // switch
      } // key request
   if(valid_user == 0) {
      close(accept_sd);
      }
   } while(Stop == 0);
}

