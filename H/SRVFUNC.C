#ifndef SRVFUNC_h
   #define SRVFUNC_h
   #include <H/COMMON>                      // common header
   #include <H/MSGFUNC>                     // message functions
   #include <qsyphandle.h>                  // profile handles
   #include <netdb.h>                       // Socket DB
   #include <sys/socket.h>                  // Sockets
   #include <netinet/in.h>                  // Socket net
   #include <arpa/inet.h>                   // ARPA structs
   #include <errno.h>                       // Error return header
   #include <netdb.h>                       // net DB header
   #include <sys/errno.h>                   // sys error header
   #include <arpa/inet.h>                   // inet header
   #include <signal.h>                      // signal funcs
   #include <except.h>                      // exception funcs
   #include <iconv.h>                       // conversion header
   #include <qtqiconv.h>                    // iconv header
   #include <qcmdchk.h>                     // cmd check
   #include <qcmdexc.h>                     // execute cmd
   #include <except.h>                      // exception funcs

   int convert_buffer(char *,char *,int,int,iconv_t);
   static void cmd_errhdl(_INTRPT_Hndlr_Parms_T *);
   int Handle_SO(int,char *,iconv_t,iconv_t);
   int handle_CM(int,iconv_t,iconv_t);
   #endif
