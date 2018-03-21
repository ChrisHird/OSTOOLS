#ifndef MSGFUNC_h
   #define MSGFUNC_h
   #include <H/COMMON>                      // common header
   #include <qmhsndm.h>                     // snd msg

   //the default message queue
   #define _DFT_MSGQ "OSMSGQ    *LIBL     "

   // function declarations
   void snd_error_msg(Os_EC_t);
   #endif
