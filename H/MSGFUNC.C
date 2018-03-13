#ifndef MSGFUNC_h
   #define MSGFUNC_h
   #include <qusec.h>                          // Error Code Structs
   #include <qmhsndm.h>                        // snd msg

   //the default message queue
   #define _DFT_MSGQ "OSMSGQ    *LIBL     "
   // error code structure with 1KB message data
   typedef _Packed struct  Os_EC_x {
                           Qus_EC_t EC;
                           char Exception_Data[1024];
                           } Os_EC_t;
   // define size
   #define _ERR_REC sizeof(struct Os_EC_x);

   // function declarations
   void snd_error_msg(Os_EC_t);
   #endif
