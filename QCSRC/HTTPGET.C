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

#include <H/IPFUNC>                         // IP functions
#include <H/COMMON>                         // common header
#include <H/MSGFUNC>                        // message functions
#include <H/SRVFUNC>                        // Server functions
#include <H/GENFUNC>                        // General functions
#include <iconv.h>                          // conversion header
#include <qtqiconv.h>                       // iconv header
#include <errno.h>                          // error number
#include <gskssl.h>                         // secure sockets toolkit

int gsk_clean(gsk_handle my_env_handle,
              gsk_handle my_session_handle,
              url_t *parsed_url) {
if(my_env_handle != NULL)
   gsk_environment_close(&my_env_handle);
if(my_session_handle != NULL)
   gsk_secure_soc_close(&my_session_handle);
free_mem(parsed_url);
return 1;
}

int main(int argc, char **argv) {
int sockfd = 0;                             // socket
int server_port = 80;                       // server port defaults to http
int rc = 0;                                 // return counter
int secure = 0;                             // secure session initiated
int amtRead = 0;                            // secure read
int amtSent = 0;                            // secure send
int out = 0;                                // init output val
int snd_len = 0;                            // send length
char msg_dta[_MAX_MSG];                     // message data
char req[2048];                             // maximum allowed request 2048 bytes
char recv_buf[_32K];                        // receive buffer
char convBuf[_32K];                         // conversion buffer
char _LF[2] = {0x0d,0x25};                  // LF string
char appID[50];                             // application ID
char appDesc[50];                           // application description
QtqCode_T jobCode = {0,0,0,0,0,0};          // (Job) CCSID to struct
QtqCode_T asciiCode = {819,0,0,0,0,0};      // (ASCII) CCSID from struct
iconv_t a_e_ccsid;                          // convert table struct
iconv_t e_a_ccsid;                          // convert table struct
url_t parsed_url = {NULL};                  // parsed url structure
gsk_handle my_env_handle=NULL;              // environment handle
gsk_handle my_session_handle=NULL;          // session handle

// we need the conversion tables for talking to the web server
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
// check the url passed in
if(parse_url(argv[1],&parsed_url) != 1) {
   sprintf(msg_dta,"URL %s is incorrectly formatted",argv[1]);
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   exit(-1);
   }
//printf("scheme %s\nhost %s\nport %s\npath %s\nquery %s\nsegment %s\n",
//       parsed_url.scheme, parsed_url.host,parsed_url.port,parsed_url.path,parsed_url.query,parsed_url.fragment);
// check if defined port is used
if(parsed_url.port != NULL) {
   server_port = atoi(parsed_url.port);
   }
else {
   // default is 80 so only change if https
   if(memcmp(parsed_url.scheme,"https",5) == 0) {
      server_port = 443;
      }
   }
// connect to the server
if(memcmp(parsed_url.scheme,"http",5) == 0) {
   if(rmt_connect(parsed_url.host,server_port,&sockfd) != 1) {
      sprintf(msg_dta,"Failed to connect");
      gsk_clean(my_env_handle,my_session_handle,&parsed_url);
      exit(-1);
      }
   }
else {
   // need DCM installed to use API's
   if(get_lpp_status("5770SS1","*CUR  ","0034") != 1) {
      sprintf(msg_dta,"Requires DCM to be installed for Secure Sockets connections");
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      gsk_clean(my_env_handle,my_session_handle,&parsed_url);
      exit(-1);
      }
   // register the application ID
   sprintf(appID,"SAS_TEST_CLIENT");
   sprintf(appDesc,"This is a test application for TLS");
   if(reg_appid(appID,appDesc) != 1) {
      sprintf(msg_dta,"Failed to register the application ID");
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      gsk_clean(my_env_handle,my_session_handle,&parsed_url);
      exit(-1);
      }
   // get secure environment handle
   rc = gsk_environment_open(&my_env_handle);
   if(rc != GSK_OK) {
      // cannot set env
      sprintf(msg_dta,"gsk_environment_open() failed with rc = %d : %s",rc,gsk_strerror(rc));
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      gsk_clean(my_env_handle,my_session_handle,&parsed_url);
      exit(-1);
      }
   // set the application ID to use
   rc = gsk_attribute_set_buffer(my_env_handle,GSK_OS400_APPLICATION_ID,appID,strlen(appID));
   if(rc != GSK_OK) {
      // cannot set application ID
      sprintf(msg_dta,"gsk_attribute_set_buffer() failed with rc = %d : %s",rc,gsk_strerror(rc));
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      gsk_clean(my_env_handle,my_session_handle,&parsed_url);
      exit(-1);
      }
   // set the server name (otherwise may get incorrrect certificate returned)
   rc = gsk_attribute_set_buffer(my_env_handle,GSK_SSL_EXTN_SERVERNAME_CRITICAL_REQUEST,parsed_url.host,strlen(parsed_url.host));
   if(rc != GSK_OK) {
      // cannot set application ID
      sprintf(msg_dta,"gsk_attribute_set_buffer() failed with rc = %d : %s",rc,gsk_strerror(rc));
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      gsk_clean(my_env_handle,my_session_handle,&parsed_url);
      exit(-1);
      }
   // set as client side
   rc = gsk_attribute_set_enum(my_env_handle,GSK_SESSION_TYPE,GSK_CLIENT_SESSION);
   if(rc != GSK_OK) {
      sprintf(msg_dta,"gsk_attribute_set_enum() failed with rc = %d : %s",rc,gsk_strerror(rc));
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      gsk_clean(my_env_handle,my_session_handle,&parsed_url);
      exit(-1);
      }
   // disable SSLV3 not secure
   rc = gsk_attribute_set_enum(my_env_handle,GSK_PROTOCOL_SSLV3,GSK_PROTOCOL_SSLV3_OFF);
   if (rc != GSK_OK) {
      sprintf(msg_dta,"gsk_attribute_set_enum() failed with rc = %d : %s",rc,gsk_strerror(rc));
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      gsk_clean(my_env_handle,my_session_handle,&parsed_url);
      exit(-1);
      }
   // initialize environment
   rc = gsk_environment_init(my_env_handle);
   if (rc != GSK_OK) {
      sprintf(msg_dta,"gsk_environment_init() failed with rc = %d : %s",rc,gsk_strerror(rc));
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      gsk_clean(my_env_handle,my_session_handle,&parsed_url);
      exit(-1);
      }
   // connect as per normal to get socket
   if(rmt_connect(parsed_url.host,server_port,&sockfd) != 1) {
      sprintf(msg_dta,"Failed to connect");
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      gsk_clean(my_env_handle,my_session_handle,&parsed_url);
      exit(-1);
      }
   // open secure session
   rc = gsk_secure_soc_open(my_env_handle, &my_session_handle);
   if (rc != GSK_OK) {
      sprintf(msg_dta,"gsk_secure_soc_open() failed with rc = %d : %s",rc,gsk_strerror(rc));
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      gsk_clean(my_env_handle,my_session_handle,&parsed_url);
      close(sockfd);
      exit(-1);
      }
   // associate the sock and secure session
   rc = gsk_attribute_set_numeric_value(my_session_handle,GSK_FD,sockfd);
   if (rc != GSK_OK) {
      sprintf(msg_dta,"gsk_attribute_set_numeric_value() failed with rc = %d : %s",rc,gsk_strerror(rc));
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      gsk_clean(my_env_handle,my_session_handle,&parsed_url);
      close(sockfd);
      exit(-1);
      }
   // start the handshake process
   rc = gsk_secure_soc_init(my_session_handle);
   if (rc != GSK_OK) {
      sprintf(msg_dta,"gsk_secure_soc_init() failed with rc = %d : %s",rc,gsk_strerror(rc));
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      gsk_clean(my_env_handle,my_session_handle,&parsed_url);
      close(sockfd);
      exit(-1);
      }
   secure = 1;
   }
// build the request
sprintf(req,"GET /");
if(parsed_url.path != NULL) {
   strcat(req,parsed_url.path);
   if(parsed_url.query != NULL) {
      strcat(req,"?");
      strcat(req,parsed_url.query);
      }
   }
sprintf(msg_dta," HTTP/1.1%.2sHost: %s%.2sConnection: close%.2s%.2s",_LF,parsed_url.host,_LF,_LF,_LF);
strcat(req,msg_dta);
//printf("Request : %s\n",req);
snd_len = strlen(req);
// convert to ascii
convert_buffer(req,convBuf,snd_len,_32K,e_a_ccsid);
// send off the request
if(secure == 1) {
   rc = gsk_secure_soc_write(my_session_handle,convBuf,snd_len,&amtSent);
   //printf("rc = %d amtSent = %d snd_len = %d\n",rc,amtSent,snd_len);
   if(amtSent != snd_len) {
      if(rc != GSK_OK) {
         sprintf(msg_dta,"gsk_secure_soc_write() failed with rc = %d : %s",rc,gsk_strerror(rc));
         snd_msg("GEN0001",msg_dta,strlen(msg_dta));
         }
      else {
         sprintf(msg_dta,"Failed to send request %s",req);
         snd_msg("GEN0001",msg_dta,strlen(msg_dta));
         }
      close(sockfd);
      gsk_clean(my_env_handle,my_session_handle,&parsed_url);
      exit(-1);
      }
   }
else {
   rc = send(sockfd,convBuf,strlen(req),0);
   if(rc != snd_len) {
      sprintf(msg_dta,"Failed to send request %s",req);
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      close(sockfd);
      // need to clean up parsed url memory
      gsk_clean(my_env_handle,my_session_handle,&parsed_url);
      exit(-1);
      }
   }
// receive the response
if(secure == 1) {
   do {
      rc = gsk_secure_soc_read(my_session_handle,recv_buf,_32K, &amtRead);
      if(amtRead > 0) {
         //printf("rc = %d amtRead = %d\n",rc,amtRead);
         memset(convBuf,'\0',_32K);
         convert_buffer(recv_buf,convBuf,amtRead,_32K,a_e_ccsid);
         printf("returned data %s\n",convBuf);
         }
      } while(amtRead > 0);
   // close out the secure environment
   gsk_clean(my_env_handle,my_session_handle,&parsed_url);
   }
else {
   do {
      rc = recv(sockfd,recv_buf,_32K,0);
      if(rc <= 0) {
         break;
         }
      // convert back to ebcdic
      memset(convBuf,'\0',_32K);
      convert_buffer(recv_buf,convBuf,rc,_32K,a_e_ccsid);
      printf("returned data %s\n",convBuf);
      } while(rc > 0);
   }
// close the socket
close(sockfd);
// still need to clean up parsed url even if not secure
gsk_clean(my_env_handle,my_session_handle,&parsed_url);
return 1;
}

