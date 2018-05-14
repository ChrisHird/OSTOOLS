#ifndef MSGFUNC_h
   #define MSGFUNC_h
   #include <H/COMMON>                      // common header
   #include <qmhsndm.h>                     // snd msg
   #include <qmhsndpm.h>                    // snd program msg
   #include <qmhrcvpm.h>                    // receive program message
   #include <qmhchgem.h>                    // change error message
   #include <qgyolmsg.h>                    // open list messages

   //the default message queue
   #define _DFT_MSGQ "OSMSGQ    *LIBL     "
   #define _DFT_MSGF "OSMSGF    OSPGM     "

   // message retrieval buffer
   typedef _Packed struct  Rcv_Msg_x {
                           Qmh_Rcvpm_RCVM0200_t msg_struct;
                           char msg_data[2048];
                           } Rcv_Msg_t;
   // message selection
   typedef _Packed struct  SelInf_x {
                           Qgy_Olmsg_MsgSelInfo_t osData;
                           char  Sel_Cri[1][10];
                           char  Msg_Key[1][4];
                           int   FieldID[2];
                           }SelInf_t;
   // returned from API
   typedef _Packed struct  Msg_Ret_x {
                           int Num;
                           char Q_Inf[2][20];
                           } Msg_Ret_t;
   // message returned to user
   typedef _Packed struct  Msg_Dets_x {
                           char MsgID[7];
                           int MsgSev;
                           char MsgKey[8];
                           char MsgDta[132];
                           char Date_Time[20];
                           } Msg_Dets_t;

   // function declarations
   void snd_error_msg(Os_EC_t);
   void snd_msg(char *,char *,int);
   #endif
