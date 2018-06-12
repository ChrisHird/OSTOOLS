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
#include <H/MSGFUNC>                        // message functions
#include <H/COMMON>                         // common header
#include <ctype.h>                          // C Types
#include <errno.h>                          // error number

//
// function Get_Host_Addr()
// Purpose: get the Host address.
// @parms
//      string server name
//      struct socket address
//      int server port
// returns 1 on sucess


int Get_Host_Addr(char *server,
                  struct sockaddr_in *addr,
                  int Server_Port)  {
struct hostent *hostp;                      // host struct pointer
char msg_dta[_MAX_MSG];                     // msg array

addr->sin_family = AF_INET;
addr->sin_port = htons(Server_Port);
if((addr->sin_addr.s_addr  = inet_addr(server)) == (unsigned long) INADDR_NONE) {
   hostp = gethostbyname(server);
   if(hostp == (struct hostent *)NULL) {
      sprintf(msg_dta,"%s",hstrerror(h_errno));
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      return -1;
      }
   memcpy(&addr->sin_addr,hostp->h_addr,sizeof(addr->sin_addr));
   }
return 1;
}


// (function) rmt_connect
// Connect to the remote system
// @parms
//     Configuration record
//                       socket decriptor
// returns 1 connected, socket set to connection

int rmt_connect(char *server,
                int server_port,
                int *sockfd) {
int rc = 0;                                 // return value
char msg_dta[_MAX_MSG] = {'\0'};            // msg data
struct sockaddr_in addr;                    // socket struct

memset(&addr, 0, sizeof(addr));
*sockfd = socket(AF_INET, SOCK_STREAM, 0);
if(*sockfd < 0) {
   sprintf(msg_dta,"%s",strerror(errno));
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   return -1;
   }
// get correct IP address
if(Get_Host_Addr(server,&addr,server_port) != 1) {
   //close the socket
   close(*sockfd);
   return -1;
   }
rc = connect(*sockfd, (struct sockaddr *) &addr, sizeof(addr));
if(rc < 0) {
   sprintf(msg_dta,"Failed to connect to socket : %s",strerror(errno));
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   close(*sockfd);
   return -1;
   }
return 1;
}

// (function) parse_url
// parse the URL into parts
// @parms
//     Url
//     url parts struct
// returns 1 on OK -1 on failure

int parse_url(const char *url,
              url_t *parsed_url) {
const char *tmpstr;
const char *curstr;
int len;
int i;
int bracket_flag;

// set up start ptr
curstr = url;
// get scheme
tmpstr = strchr(curstr, ':');
if(tmpstr == NULL) {
   return -1;
   }
// Get the scheme length
len = tmpstr - curstr;
// the port number is also a ':' so make sure its not too long http(s) is maximum of 5
if(len > 5) {
   return -1;
   }
// Copy the scheme to the storage */
parsed_url->scheme = malloc(sizeof(char) * (len + 1));
if(parsed_url->scheme == NULL) {
   return -1;
   }
memcpy(parsed_url->scheme, curstr, len);
parsed_url->scheme[len] = '\0';
// Make the character to lower if it is upper case. */
for(i = 0; i < len; i++) {
   parsed_url->scheme[i] = tolower(parsed_url->scheme[i]);
   }
// Skip ':'
tmpstr++;
curstr = tmpstr;
// skip the '//' if not there bad url
for(i = 0; i < 2; i++) {
   if (*curstr !=  '/') {
      free_mem(parsed_url);
      return -1;
      }
   curstr++;
   }
// get the host
tmpstr = curstr;
while(*tmpstr != '\0') {
   // could be a port number or a '/'
   if((*tmpstr == ':') || (*tmpstr == '/')) {
      break;
      }
   tmpstr++;
   }
len = tmpstr - curstr;
parsed_url->host = malloc(sizeof(char) * (len + 1));
if((parsed_url->host == NULL) || (len <= 0)) {
   free_mem(parsed_url);
   return -1;
   }
memcpy(parsed_url->host, curstr, len);
parsed_url->host[len] = '\0';
curstr = tmpstr;
// Is port number specified?
if(*curstr == ':') {
   curstr++;
   tmpstr = curstr;
   while((*tmpstr != '\0') && (*tmpstr != '/')) {
      tmpstr++;
      }
   len = tmpstr - curstr;
   parsed_url->port = malloc(sizeof(char) * (len + 1));
   if(parsed_url->port == NULL) {
      free_mem(parsed_url);
      return -1;
      }
   memcpy(parsed_url->port, curstr, len);
   parsed_url->port[len] = '\0';
   curstr = tmpstr;
   }
// End of the string
if(*curstr == '\0') {
   return 1;
   }
// Skip '/'
if(*curstr != '/') {
  free_mem(parsed_url);
  return -1;
  }
curstr++;
// Parse path
tmpstr = curstr;
while((*tmpstr != '\0') && (*tmpstr != '#')  && (*tmpstr != '?')) {
   tmpstr++;
   }
len = tmpstr - curstr;
parsed_url->path = malloc(sizeof(char) * (len + 1));
if(parsed_url->path == NULL) {
   free_mem(parsed_url);
   return -1;
   }
memcpy(parsed_url->path, curstr, len);
parsed_url->path[len] = '\0';
curstr = tmpstr;
// Is query specified?
if(*curstr == '?') {
   // Skip '?'
   curstr++;
   // Read query
   tmpstr = curstr;
   while((*tmpstr != '\0') && (*tmpstr != '#')) {
      tmpstr++;
      }
   len = tmpstr - curstr;
   parsed_url->query = malloc(sizeof(char) * (len + 1));
   if(parsed_url->query == NULL) {
      free_mem(parsed_url);
      return -1;
      }
   memcpy(parsed_url->query, curstr, len);
   parsed_url->query[len] = '\0';
   curstr = tmpstr;
   }
// Is fragment specified?
if(*curstr == '#') {
   // Skip '#'
   curstr++;
   // Read fragment
   tmpstr = curstr;
   while(*tmpstr != '\0') {
      tmpstr++;
      }
   len = tmpstr - curstr;
   parsed_url->fragment = malloc(sizeof(char) * (len + 1));
   if(parsed_url->fragment == NULL) {
      free_mem(parsed_url);
      return -1;
      }
   memcpy(parsed_url->fragment, curstr, len);
   parsed_url->fragment[len] = '\0';
   curstr = tmpstr;
   }
return 1;
}

/*
 * Free memory of parsed url
 */
void free_mem(url_t *parsed_url) {
if(parsed_url != NULL) {
   if(parsed_url->scheme != NULL) {
      free(parsed_url->scheme);
      }
   if(parsed_url->host != NULL) {
      free(parsed_url->host);
      }
   if(parsed_url->port != NULL) {
      free(parsed_url->port);
      }
   if(parsed_url->path != NULL) {
      free(parsed_url->path);
      }
   if(parsed_url->query != NULL) {
      free(parsed_url->query);
      }
   if(parsed_url->fragment != NULL) {
      free(parsed_url->fragment);
      }
   }
return;
}
