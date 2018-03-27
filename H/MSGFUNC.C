#ifndef MSGFUNC_h
   #define MSGFUNC_h
   #include <H/COMMON>                      // common header
   #include <qmhsndm.h>                     // snd msg
   #include <qmhsndpm.h>                    // snd program msg

   //the default message queue
   #define _DFT_MSGQ "OSMSGQ    *LIBL     "
   #define _DFT_MSGF "OSMSGF    OSPGM     "

   // function declarations
   void snd_error_msg(Os_EC_t);
   void snd_msg(char *,char *,int);
   #endif
