# OSTOOLS
A collection of sample 'C' code *PGMs and *SRVPGMs which can be used as a basis for further development. The aim is to show how to develop modular 'C' programs for the IBM i 
by using the ILE language capabilities provided by the system. The concepts can be used to develop applications using any ILE capable language such as RPG by adding additional 
modules and service programs to the ones provided.
# Samples
Discussion about the processes we followed can be found at [Shield Advanced Blog](https://www.shieldadvanced.com/Blog/?s=Let%27s+%27C%27)
## CHKMSGID
A program which walks through source code looking for a particular string in each line. Can be used to identify message ID's contained in a message file that have not been utilized
in the source code. Concept can be used for any search string requirement in a SRC-PF file.
##CHKSIGS
When you are developing Service Programs it is important that you understand the requirements for linking the programs, [Link to ILE Concepts](https://www.ibm.com/support/knowledgecenter/ssw_ibm_i_73/ilec/sc415606.pdf)
is a good guide to the requirements but this program is built to wlak through all of the *PGM and *SRVPGM objects in a library and check the signatures for the *SRVPGM match those
contained in the *PGM objects.
##COPYSRCA
A program which will copy source lines from a SRC-PF file to a temporary file based on a string passed into the program. This can be used to filter the content of the file prior to 
running the CHKMSGID program which will significantly improve the time taken for the program to filter the source content.
##FNDMSGCNT
A program that will find message ID's in a message file which have a particular string in them and prompt the CHGMSGD command to allow changes. This is a good way to alter the message
content in a message file when things such as a product name changes and all messages have to reflect the new name.
##HTTPGET
A program which will contact a Web Server and retrieve the page returned for a specific GET request. Currently supports http url requests but will be updated to allow https requests to be made.
##LSTOBJBYTP
Program to list the content of a library by Object type. Sends a list of objects to STDOUT.
##SVRSTOP
Program which sends a stop request via a data queue to a server job (see TESTSVR)
##TESTSVR
A Server program which will take ASCII based requests and convert them to EBCDIC for internal processing. Main concept is how to convert ASCII based requests into EBCDIC so the IBM i can process
and then return an ASCII based response to the client.
##WORKER
A Worker job launched by the TESTSVR to service client requests. Simple test which currently allows secure requests (run under the requesting profile) to be run on the IBM i.
#Service Programs
A number of service programs have been built to provide functionality to other programs. This provides a way to minimize coding effort (code once use many times) plus benefit from code control
(only one place to update code for many requests). Also provides a level of memory reduction (lower footprint etc) and in many cases better performance.

A new program has been added CRTOBJS which can be used to build all of the contained code as well as the objects required to run the test environemnts. Simply clone the repository
to your IBM i and compile the CRTOBJS program in the source library (OSLIB) then call the program passing in the parameter for the Target release you want the objects to be created
for (V7R1M0 etc.)     
