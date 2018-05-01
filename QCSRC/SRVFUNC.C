#include <H/SRVFUNC>                        // Server Functions Header

// Function Handle_SO()
// purpose: handle a sign on request
// @parms
//      socket
//      Prf Handle
// returns 1 on success

int Handle_SO(int accept_sd,
              char *Usr_hdl,
              iconv_t a_e_ccsid,
              iconv_t e_a_ccsid) {
int rc = 0;                                 // return code
int len = 0;                                // counter
int ret = 0;                                // return code
size_t insz;                                // input len
size_t outsz;                               // output size
char recv_buf[_32K];                        // recv buffer
char msg_dta[_MAX_MSG];                     // message buffer
char *out_ptr;                              // buffer ptr
char *in_ptr;                               // buffer ptr
char Profile[10] = {' '};                   // Profile
char Pwd[128] = {' '};                      // password
char convBuf[132];                          // conversion buffer
Os_EC_t Error_Code = {0};                   // error struct

Error_Code.EC.Bytes_Provided = _ERR_REC;
// first we need to retieve the profile
sprintf(msg_dta,"Please enter your Profile name : ");
len = strlen(msg_dta);
in_ptr = msg_dta;
out_ptr = convBuf;
insz = len;
outsz = 132;
ret = (iconv(e_a_ccsid,(char **)&(in_ptr),&insz,(char **)&(out_ptr),&outsz));
rc = send(accept_sd,convBuf,len,0);
// make sure the data was sent
if(rc != len) {
   return -1;
   }
// make sure not too long. Our test server sends a '0A' at the end of the input so
// it needs to be dropped. In a future implementation we should consider alternatives
rc = recv(accept_sd,recv_buf,_32K,0);
// copy to the Profile holder which is initialized with all blanks after conversion
in_ptr = recv_buf;
out_ptr = convBuf;
insz = rc;
outsz = 132;
ret = (iconv(a_e_ccsid,(char **)&(in_ptr),&insz,(char **)&(out_ptr),&outsz));
// strip off the 'A0' byte.
len = rc -1;
if(len > 10) {
   sprintf(msg_dta,"Profile too long Sign On aborted");
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   return -1;
   }
memcpy(Profile,convBuf,len);
// now get the password
sprintf(msg_dta,"Please enter your Password : ");
len = strlen(msg_dta);
in_ptr = msg_dta;
out_ptr = convBuf;
insz = len;
outsz = 132;
ret = (iconv(e_a_ccsid,(char **)&(in_ptr),&insz,(char **)&(out_ptr),&outsz));
rc = send(accept_sd,convBuf,len,0);
if(rc != len) {
   return -1;
   }
// recv the password and convert
rc = recv(accept_sd,recv_buf,_32K,0);
in_ptr = recv_buf;
out_ptr = convBuf;
insz = rc;
outsz = 132;
ret = (iconv(a_e_ccsid,(char **)&(in_ptr),&insz,(char **)&(out_ptr),&outsz));
// strip off the 'A0' byte.
len = rc -1;
if(len > 128) {
   sprintf(msg_dta,"Password too long");
   snd_msg("GEN0001",msg_dta,strlen(msg_dta));
   return -1;
   }
memcpy(Pwd,convBuf,len);
// now check the password against the profile, CCSID of 0 means use the job ccsid
QsyGetProfileHandle(Usr_hdl,
                    Profile,
                    Pwd,
                    rc,
                    0,
                    &Error_Code);
if(Error_Code.EC.Bytes_Available) {
   snd_error_msg(Error_Code);
   return -1;
   }
return 1;
}
