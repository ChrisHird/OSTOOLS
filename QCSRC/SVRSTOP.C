//
// Copyright (c) 2018 Chris Hird
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// Disclaimer :
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#include <stdio.h>                          // stdio header
#include <string.h>                         // string header
#include <stdlib.h>                         // stdlib header
#include <decimal.h>                        // decimal header
#include <qsnddtaq.h>                       // send dataq header

void main(int argc, char **argv) {
decimal(3,0)  KeyLength = 4.0d;             // length of key
char Key[4] = "0000";                       // key used retrieval
char QueueData[7] = "STOP   ";              // Message to quit
char DQName[10] = "SVRCTLQ   ";             // data queue name
char DQLib[10] = "OSPGM     ";              // data queue lib
decimal(5,0)  Data_Len = 7.0d;              // data length

QSNDDTAQ(DQName,
         DQLib,
         Data_Len,
         QueueData,
         KeyLength,
         &Key);
exit(0);
}
