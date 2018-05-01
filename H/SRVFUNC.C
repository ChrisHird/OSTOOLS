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

   int Handle_SO(int,char *,iconv_t,iconv_t);
   #endif
