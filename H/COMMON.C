#ifndef COMMON_h
   #define COMMON_h
   #include <stdlib.h>                      // standard library
   #include <string.h>                      // string functions
   #include <stdio.h>                       // standard IO
   #include <qusec.h>                       // Error codes

   // error code structure with 1KB message data
   typedef _Packed struct  Os_EC_x {
                           Qus_EC_t EC;
                           char Exception_Data[1024];
                           } Os_EC_t;

   typedef _Packed struct  date_fmt_x {
                           char C;
                           char Y[2];
                           char M[2];
                           char D[2];
                           } date_fmt_t;

   typedef _Packed struct  time_fmt_x {
                           char H[2];
                           char M[2];
                           char S[2];
                           } time_fmt_t;

   // define size
   #define _ERR_REC sizeof(struct Os_EC_x);

   // so we don't have to remember sizes
   #define _1KB 1024
   #define _8K _1KB * 8
   #define _32K _1KB * 32
   #define _64K _1KB * 64
   #define _1MB _1KB * _1KB
   #define _1GB ((long)_1MB * _1KB)
   #define _1TB ((double)_1GB * _1KB)
   #define _4MB _1MB * 4
   #define _8MB _1MB * 8
   #define _16MB 16773120
   #define CHAR_32K "32768"


   #endif
