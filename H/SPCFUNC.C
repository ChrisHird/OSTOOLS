#ifndef SPCFUNC_h
   #define SPCFUNC_h
   #include <quscrtus.h>                    // Create UsrSpc
   #include <qusdltus.h>                    // Delete UsrSpc
   #include <qusptrus.h>                    // Pointer to UsrSpc
   #include <qusgen.h>                      // usrspc gen hdr

   // function declarations
   int Crt_Usr_Spc(char *,int);
   int Get_Spc_Ptr(char *,void *,int);
   #endif
