000100180313#ifndef MSGFUNC_h
000101180313   #define MSGFUNC_h
000102180313   #include <qusec.h>                          // Error Code Structs
000103180313   #include <qmhsndm.h>                        // snd msg
000104180313
000105180313   //the default message queue
000106180313   #define _DFT_MSGQ "OSMSGQ    *LIBL     "
000107180313   // error code structure with 1KB message data
000108180313   typedef _Packed struct  Os_EC_x {
000109180313                           Qus_EC_t EC;
000110180313                           char Exception_Data[1024];
000111180313                           } Os_EC_t;
000112180313   // define size
000113180313   #define _ERR_REC sizeof(struct Os_EC_x);
000114180313
000115180313   // function declarations
000116180313   void snd_error_msg(Os_EC_t);
000117180313   #endif
