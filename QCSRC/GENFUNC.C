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

#include <H/GENFUNC>                        // General functions Hdr
#include <H/MSGFUNC>                        // Message functions
#include <H/COMMON>                         // common header


// function get_lpp_status()
// Purpose to retrieve the status of a LPP
// @parms
//      LPP
// returns status

int get_lpp_status(char *lpp,
                   char *ver,
                   char *opt) {
char msg_dta[_MAX_MSG];                     // message buffer
Qsz_Product_Info_Rec_t Prd_Inf;             // Prod Info Struct
Qsz_PRDR0100_t Prod_Dets;                   // returned data
Os_EC_t Error_Code;                         // error struct

Error_Code.EC.Bytes_Provided = sizeof(Error_Code);

memcpy(Prd_Inf.Product_Id,lpp,7);
memcpy(Prd_Inf.Release_Level,ver,6);
memcpy(Prd_Inf.Product_Option,opt,4);
memcpy(Prd_Inf.Load_Id,"*CODE     ",10);
QSZRTVPR(&Prod_Dets,
         sizeof(Prod_Dets),
         "PRDR0100",
         &Prd_Inf,
         &Error_Code);
if(Error_Code.EC.Bytes_Available > 0) {
   if(memcmp(Error_Code.EC.Exception_Id,"CPF0C1F",7) == 0) {
      sprintf(msg_dta,"Product %.7s - %.6s - %.4s not installed",lpp,ver,opt);
      snd_msg("GEN0001",msg_dta,strlen(msg_dta));
      return 0;
      }
   else {
      snd_error_msg(Error_Code);
      return -1;
      }
   }
if(memcmp(Prod_Dets.Symbolic_Load_State,"*INSTALLED",10) == 0) {
   return 1;
   }
return 0;
}

// function reg_appid()
// purpose: register application with DCM
// @parms
//      Application ID
//      Application description
// returns 1

int reg_appid(char *appID,
              char *appDesc) {
int appIDLen = 0;                           // app ID Length
int appDescLen = 0;                         // app Description Length
char msg_dta[_MAX_MSG];                     // message buffer
Ctl_Rec_t AppCtls;                          // control record
Os_EC_t Error_Code;                         // error struct

Error_Code.EC.Bytes_Provided = sizeof(Error_Code);
appIDLen = strlen(appID);
appDescLen = strlen(appDesc);
// number of keys non default
AppCtls.numRecs = 4;
// application type
AppCtls.appType.size = sizeof(_Packed struct App_Type_x);
AppCtls.appType.key = 8;
AppCtls.appType.dtaLen = 1;
AppCtls.appType.dta = '2';
// application description
AppCtls.appDesc.size = sizeof(_Packed struct App_Desc_x);
AppCtls.appDesc.key = 2;
AppCtls.appDesc.dtaLen = 50;
memset(AppCtls.appDesc.dta,' ',50);
memcpy(AppCtls.appDesc.dta,appDesc,appDescLen);
// certificate trust
AppCtls.caTrust.size = sizeof(_Packed struct CA_Trust_x);
AppCtls.caTrust.key = 4;
AppCtls.caTrust.dtaLen = 1;
AppCtls.caTrust.dta = '0';
// replace existing cert
AppCtls.certRpl.size = sizeof(_Packed struct Cert_Rpl_x);
AppCtls.certRpl.key = 5;
AppCtls.certRpl.dtaLen = 1;
AppCtls.certRpl.dta = '1';
// register
QsyRegisterAppForCertUse(appID,
                         &appIDLen,
                         (Qsy_App_Controls_T *)&AppCtls,
                         &Error_Code);
if(Error_Code.EC.Bytes_Available > 0) {
   snd_error_msg(Error_Code);
   return -1;
   }
return 1;
}

