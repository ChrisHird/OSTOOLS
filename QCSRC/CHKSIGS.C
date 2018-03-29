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

#include <H/MSGFUNC>                        // Message headers
#include <H/SPCFUNC>                        // User Space functions
#include <H/COMMON>                         // common header file
#include <qbnlpgmi.h>                       // ILE pgm info
#include <qbnlspgm.h>                       // Srv pgm info
#include <quslobj.h>                        // list objects

// function pre-declarations
int dsp_sig(const char *,size_t, char *);
void Dump_Hex_Output(const char*, size_t);

int main(int argc, char **argv) {
int i = 0;                                  // counter
int j = 0;                                  // counter
int k = 0;                                  // counter
short int found = 0;                        // found flag
char Obj_SPC[20] = "OBJLST    QTEMP     ";  // USRSPC
char Srv_SPC[20] = "QBNLSPGM  QTEMP     ";  // usrspc
char Pgm_SPC[20] = "QBNLPGMI  QTEMP     ";  // usrspc
char Srv_Format1[8] = "SPGL0200";           // request format
char Srv_Format[8] = "SPGL0800";            // request format
char Pgm_Format[8] = "PGML0200";            // request format
char SPC_Desc[50] = {' '};                  // USRSPC Description
char Replace[10] = "*YES      ";            // Replace USRSPC
char Ext_Atr[10] = "USRSPC    ";            // Ext attr USRSPC
char Initial_Value = ' ';                   // Init val USRSPC
char Auth[10] = "*CHANGE   ";               // Pub aut to USRSPC
char Signature[16];                         // SrvPgm Signature
char Obj_Sel[20] = "*ALL                ";  // Objects to select
char srv_pgm_name[20];                      // service program name
char sig1[33] = {0};                        // signature
char sig2[33] = {0};                        // signature
char msg_dta[1024];                         // msg buffer
char *List_Section;                         // usrspc list ptr
char *tmp;                                  // temp ptr
Qus_OBJL0100_t *Obj_Entry;                  // USRSPC Pointer
Qbn_LPGMI_PGML0200_t *Pgm_Entry;            // info
Qbn_LSPGM_SPGL0800_t *Srv_Entry;            // SrvPgm Info
Qbn_LSPGM_SPGL0200_t *SrvPgmEntry;          // service program info
Qus_Generic_Header_0100_t *obj_space;       // usrspc hdr ptr
Qus_Generic_Header_0100_t *srv_space;       // usrspc hdr ptr
Qus_Generic_Header_0100_t *pgm_space;       // usrspc hdr ptr
Os_EC_t Error_Code;                         // error struct

Error_Code.EC.Bytes_Provided = sizeof(Error_Code);
memcpy(&Obj_Sel[10],argv[1],10);
// build the user spaces
// holds all of the objects in the library
if(Get_Spc_Ptr(Obj_SPC,&obj_space,_16MB) != 1)
   exit(-1);
// holds the Service Program details
if(Get_Spc_Ptr(Srv_SPC,&srv_space,_1MB) != 1)
   exit(-1);
// holds the program details
if(Get_Spc_Ptr(Pgm_SPC,&pgm_space,_4MB) != 1)
   exit(-1);
// list all objects in the library
QUSLOBJ(Obj_SPC,
        "OBJL0100",
        Obj_Sel,
        "*ALL      ",
        &Error_Code);
if(Error_Code.EC.Bytes_Available > 0) {
   snd_error_msg(Error_Code);
   exit(-1);
   }
// work through the objects
tmp = (char *)obj_space;
// move to list data
tmp += obj_space->Offset_List_Data;
// set structure pointer to list data
Obj_Entry = (Qus_OBJL0100_t *)tmp;
//printf("Number of objects listed %d\n",obj_space->Number_List_Entries);
for(i = 0; i < obj_space->Number_List_Entries; i++) {
   // look only at the programs
   if(memcmp(Obj_Entry->Object_Type_Used,"*PGM",4) == 0) {
      // get the list of service programs
      //printf("Checking *PGM %.20s\n",Obj_Entry->Object_Name_Used);
      QBNLPGMI(Pgm_SPC,
               Pgm_Format,
               Obj_Entry->Object_Name_Used,
               &Error_Code);
      if(Error_Code.EC.Bytes_Available > 0) {
         // send error message but do not exit!
         snd_error_msg(Error_Code);
         }
      else {
         //printf("Number of Service Programs listed %d\n",pgm_space->Number_List_Entries);
         // get to the contents of the service programs
         if(pgm_space->Number_List_Entries > 0) {
            tmp = (char *)pgm_space;
            tmp += pgm_space->Offset_List_Data;
            Pgm_Entry = (Qbn_LPGMI_PGML0200_t *)tmp;
            // loop through the listed service programs
            for(j = 0; j < pgm_space->Number_List_Entries; j++) {
               // check the signature for the service program
               memcpy(srv_pgm_name,Pgm_Entry->Bound_Service_Program,10);
               // check for library list as the library definition.
               if(*Pgm_Entry->Bound_Service_Library_Name == 0x00)
                  memcpy(&srv_pgm_name[10],"*LIBL     ",10);
               else
                  memcpy(&srv_pgm_name[10],Pgm_Entry->Bound_Service_Library_Name,10);
               QBNLSPGM(Srv_SPC,
                        Srv_Format,
                        srv_pgm_name,
                        &Error_Code);
               if(Error_Code.EC.Bytes_Available > 0) {
                  // send error message but do not exit!
                  snd_error_msg(Error_Code);
                  }
               else {
                  // check the signature
                  tmp = (char *)srv_space;
                  tmp += srv_space->Offset_List_Data;
                  Srv_Entry = (Qbn_LSPGM_SPGL0800_t *)tmp;
                  found = 0;
                  //printf("Number of signatures %d\n",srv_space->Number_List_Entries);
                  for(k = 0; k < srv_space->Number_List_Entries; k++) {
                     //printf("*PGM %.10s *SRVPGM %.20s\n",Obj_Entry->Object_Name_Used,srv_pgm_name);
                     //dsp_sig(Srv_Entry->Signature,16,sig1);
                     //dsp_sig(Pgm_Entry->Bound_Service_Signature,16,sig2);
                     //printf(msg_dta,"SRVPGM = %.32s\nPGM = %.32s\n",sig1,sig2);
                     if(memcmp(Srv_Entry->Signature,Pgm_Entry->Bound_Service_Signature,16) == 0) {
                        found = 1;
                        break;
                        }
                     Srv_Entry++;
                     }
                  if(found == 0) {
                     // signatures do not match so print to stdout
                     printf("No valid signature in %.10s for Pgm %.10s\n",srv_pgm_name,Obj_Entry->Object_Name_Used);
                     printf("*PGM    signature - ");
                     Dump_Hex_Output(Pgm_Entry->Bound_Service_Signature,16);
                     printf("\n");
                     }
                  }
               Pgm_Entry++;
               }
            }
         }
      }
   // if its a service program
   else if(memcmp(Obj_Entry->Object_Type_Used,"*SRVPGM",7) == 0) {
      //printf("Checking *SRVPGM %.20s\n",Obj_Entry->Object_Name_Used);
      QBNLSPGM(Pgm_SPC,
               Srv_Format1,
               Obj_Entry->Object_Name_Used,
               &Error_Code);
      if(Error_Code.EC.Bytes_Available > 0) {
         // send error message but do not exit!
         snd_error_msg(Error_Code);
         }
      else {
         //printf("Number of Service Programs listed %d\n",pgm_space->Number_List_Entries);
         if(pgm_space->Number_List_Entries > 0) {
            tmp = (char *)pgm_space;
            tmp += pgm_space->Offset_List_Data;
            SrvPgmEntry = (Qbn_LSPGM_SPGL0200_t *)tmp;
            for(j = 0; j < pgm_space->Number_List_Entries; j++) {
               // check the signature for the service program
               memcpy(srv_pgm_name,SrvPgmEntry->Bound_Srvpgm_Name,10);
               if(*SrvPgmEntry->Bound_Srvpgm_Library_Name == 0x00)
                  memcpy(&srv_pgm_name[10],"*LIBL     ",10);
               else
                  memcpy(&srv_pgm_name[10],SrvPgmEntry->Bound_Srvpgm_Library_Name,10);
               QBNLSPGM(Srv_SPC,
                        Srv_Format,
                        srv_pgm_name,
                        &Error_Code);
               if(Error_Code.EC.Bytes_Available > 0) {
                  // send error message but do not exit!
                  snd_error_msg(Error_Code);
                  }
               else {
                  // check the signature
                  tmp = (char *)srv_space;
                  tmp += srv_space->Offset_List_Data;
                  Srv_Entry = (Qbn_LSPGM_SPGL0800_t *)tmp;
                  found = 0;
                  //printf("Number of signatures %d\n",srv_space->Number_List_Entries);
                  for(k = 0; k < srv_space->Number_List_Entries; k++) {
                     //printf("*PGM %.10s *SRVPGM %.20s\n",Obj_Entry->Object_Name_Used,srv_pgm_name);
                     //dsp_sig(Srv_Entry->Signature,16,sig1);
                     //dsp_sig(Pgm_Entry->Bound_Service_Signature,16,sig2);
                     //printf("SRVPGM = %.32s\nPGM = %.32s\n",sig1,sig2);
                     if(memcmp(Srv_Entry->Signature,SrvPgmEntry->Bound_Srvpgm_Signature,16) == 0) {
                        found = 1;
                        break;
                        }
                     Srv_Entry++;
                     }
                  if(found == 0) {
                     printf("No valid signature in %.10s for Pgm %.10s\n",srv_pgm_name,Obj_Entry->Object_Name_Used);
                     printf("\n*PGM    signature - ");
                     Dump_Hex_Output(SrvPgmEntry->Bound_Srvpgm_Signature,16);
                     printf("\n");
                     }
                  }
               SrvPgmEntry++;
               }
            }
         }
      }
   Obj_Entry++;
   }
exit(0);
}


// functions dsp_sig()
// Purpose: Create Hex signature for display
// @parms
//      Input buffer (signature)
//      length of Buffer
//      sig buffer (written from input content)
// returns int

int dsp_sig(const char *buf,size_t buf_len, char *sig) {
int i = 0;                                  // counter

while(buf_len > 0) {
   sprintf(&sig[i],"%02x",*buf);
   i+= 2;
   buf_len--;
   buf++;
   }
return 1;
}

// functions Dump_Hex_Output()
// Purpose: prints the Hex content to stdout
// @parms
//      Input Buffer
//      Length of input buffer
// returns void

void Dump_Hex_Output(const char* buf, size_t buf_len) {
while(buf_len > 0) {
   printf("%02x",*buf);
   buf_len--;
   buf++;
   }
}


