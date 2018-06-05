#ifndef IPFUNC_h
   #define IPFUNC_h
   #include <sys/socket.h>                     // sockets header
   #include <netinet/in.h>                     // host address
   #include <netdb.h>                          // network Db func
   #include <arpa/inet.h>                      // inet_addr header
   #include <resolv.h>                         // hstrerror header
   #include <sys/types.h>                      // types header
   #include <netinet/tcp.h>                    // TCP Options

   typedef struct url_x {
                  char *scheme;
                  char *host;
                  char *port;
                  char *path;
                  char *query;
                  char *fragment;
                  } url_t;
   int Get_Host_Addr(char *,struct sockaddr_in *,int);
   int rmt_connect(char *,int,int *);
   int parse_url(const char *, url_t *);
   void free_mem(url_t *);
   #endif
