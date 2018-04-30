/*********************************************************************/
/*                                                                   */
/*Program Name: SVRSTOP                                              */
/*                                                                   */
/*Program Language:  ILE C                                           */
/*                                                                   */
/*Description: This program illustrates how to use APIs to create    */
/*             and manipulate a data queue.                          */
/*                                                                   */
/*                                                                   */
/*Header Files Included: <stdio.h>                                   */
/*                       <string.h>                                  */
/*                       <stdlib.h>                                  */
/*                       <decimal.h>                                 */
/*                       <qsnddtaq.h>                                */
/*                                                                   */
/*                                                                   */
/*APIs Used:                                                         */
/*               QSNDDTAQ - Send Data to dataqueue                   */
/*                                                                   */
/*********************************************************************/
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
