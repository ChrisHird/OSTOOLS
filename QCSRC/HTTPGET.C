#include <H/IPFUNC>                         // IP functions
#include <H/COMMON>                         // common header
#include <H/MSGFUNC>                        // message functions
#include <H/SRVFUNC>                        // Server functions
#include <iconv.h>                          // conversion header
#include <qtqiconv.h>                       // iconv header
#include <errno.h>                          // error number

int main(int argc, char **argv) {
int sockfd = 0;                             // socket
int server_port = 80;                       // server port defaults to http
int rc = 0;                                 // return counter
char msg_dta[_MAX_MSG];                     // message data
char req[2048];                             // maximum allowed request 2048 bytes
char recv_buf[_32K];                        // receive buffer
char convBuf[_32K];                         // conversion buffer
char _LF[2] = {0x0d,0x25};                  // LF string
QtqCode_T jobCode = {0,0,0,0,0,0};          // (Job) CCSID to struct
QtqCode_T asciiCode = {819,0,0,0,0,0};      // (ASCII) CCSID from struct
iconv_t a_e_ccsid;                          // convert table struct
iconv_t e_a_ccsid;                          // convert table struct
url_t parsed_url = {NULL};                  // parsed url structure

// we need the conversion tables for talking to the web server
a_e_ccsid = QtqIconvOpen(&jobCode,&asciiCode);
if(a_e_ccsid.return_value == -1) {
   sprintf(msg_dta,"QtqIconvOpen Failed %s",strerror(errno));
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   return -1;
   }
// EBCDIC to ASCII
e_a_ccsid = QtqIconvOpen(&asciiCode,&jobCode);
if(e_a_ccsid.return_value == -1) {
   iconv_close(a_e_ccsid);
   sprintf(msg_dta,"QtqIconvOpen Failed %s",strerror(errno));
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   return -1;
   }
// check the url passed in
if(parse_url(argv[1],&parsed_url) != 1) {
   sprintf(msg_dta,"URL %s is incorrectly formatted",argv[1]);
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   return -1;
   }
printf("scheme %s\nhost %s\nport %s\npath %s\nquery %s\nsegment %s\n",
       parsed_url.scheme, parsed_url.host,parsed_url.port,parsed_url.path,parsed_url.query,parsed_url.fragment);
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
if(rmt_connect(parsed_url.host,server_port,&sockfd) != 1) {
   printf("Failed to connect\n");
   free_mem(&parsed_url);
   return -1;
   }
// build the request
sprintf(req,"GET /%s?%s HTTP/1.1%.2sHost: %s%.2sConnection: close%.2s%.2s",
                              parsed_url.path,parsed_url.query,_LF,parsed_url.host,_LF,_LF,_LF);
//printf("Request : %s\n",req);
// convert to ascii
convert_buffer(req,convBuf,strlen(req),_32K,e_a_ccsid);
// send off the request
rc = send(sockfd,convBuf,strlen(req),0);
if(rc != strlen(req)) {
   sprintf(msg_dta,"Failed to send request %s",req);
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   close(sockfd);
   free_mem(&parsed_url);
   return -1;
   }
// receive the response
rc = recv(sockfd,recv_buf,_32K,0);
// convert back to ebcdic
memset(convBuf,'\0',_32K);
convert_buffer(recv_buf,convBuf,rc,_32K,a_e_ccsid);
printf("returned data %s\n",convBuf);
// close the socket
close(sockfd);
// free any allocated memory
free_mem(&parsed_url);
return 1;
}

