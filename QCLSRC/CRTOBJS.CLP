             PGM        PARM(&TGTRLS)
             DCL        VAR(&TGTRLS) TYPE(*CHAR) LEN(6)
             DCL        VAR(&PGMLIB) TYPE(*CHAR) LEN(10) value('OSPGM')
             DCL        VAR(&SRCLIB) TYPE(*CHAR) LEN(10) value('OSLIB')

             /* create the library, if already exists then clear it */
             CRTLIB     LIB(&PGMLIB)
             MONMSG     MSGID(CPF2111) EXEC(CLRLIB LIB(&PGMLIB))
             /* create the ancilliary objects */
             CRTMSGQ    MSGQ(&PGMLIB/OSMSGQ)
             CRTSBSD    SBSD(&PGMLIB/OSSBS) POOLS((1 *BASE))
             CRTJOBQ    JOBQ(&PGMLIB/OSJOBQ) TEXT('Job Queue for OSSBS jobs')
             CRTCLS     CLS(&PGMLIB/OSCLS) TEXT('OS Class')
             ADDJOBQE   SBSD(&PGMLIB/OSSBS) JOBQ(&PGMLIB/OSJOBQ)
             ADDRTGE    SBSD(&PGMLIB/OSSBS) SEQNBR(1) CMPVAL(*ANY) PGM(QCMD) CLS(&PGMLIB/OSCLS)
             CRTJOBD    JOBD(&PGMLIB/OSSVR) JOBQ(&PGMLIB/OSJOBQ) TEXT('OS Server Job Description') INLLIBL(&PGMLIB)
             CRTDTAQ    DTAQ(&PGMLIB/SVRCTLQ) MAXLEN(256) SEQ(*KEYED) KEYLEN(4) AUTORCL(*YES) TEXT('Server Control data queue')
             /* create the modules */
             CRTCMOD    MODULE(&SRCLIB/CHKMSGID) SRCFILE(&SRCLIB/QCSRC) SRCMBR(CHKMSGID) OUTPUT(*PRINT) DBGVIEW(*SOURCE) +
                          REPLACE(*YES) TGTRLS(&TGTRLS)
             CRTCMOD    MODULE(&SRCLIB/CHKSIGS) SRCFILE(&SRCLIB/QCSRC) SRCMBR(CHKSIGS) OUTPUT(*PRINT) DBGVIEW(*SOURCE) REPLACE(*YES) +
                          TGTRLS(&TGTRLS)
             CRTCMOD    MODULE(&SRCLIB/COPYSRCA) SRCFILE(&SRCLIB/QCSRC) SRCMBR(COPYSRCA) OUTPUT(*PRINT) DBGVIEW(*SOURCE) +
                          REPLACE(*YES) TGTRLS(&TGTRLS)
             CRTCMOD    MODULE(&SRCLIB/FNDMSGCNT) SRCFILE(&SRCLIB/QCSRC) SRCMBR(FNDMSGCNT) OUTPUT(*PRINT) DBGVIEW(*SOURCE) REPLACE(*YES) +
                          TGTRLS(&TGTRLS)
             CRTCMOD    MODULE(&SRCLIB/GENFUNC) SRCFILE(&SRCLIB/QCSRC) SRCMBR(GENFUNC) OUTPUT(*PRINT) DBGVIEW(*SOURCE) REPLACE(*YES) +
                          TGTRLS(&TGTRLS)
             CRTCMOD    MODULE(&SRCLIB/HTTPGET) SRCFILE(&SRCLIB/QCSRC) SRCMBR(HTTPGET) OUTPUT(*PRINT) DBGVIEW(*SOURCE) REPLACE(*YES) +
                          TGTRLS(&TGTRLS)
             CRTCMOD    MODULE(&SRCLIB/IPFUNC) SRCFILE(&SRCLIB/QCSRC) SRCMBR(IPFUNC) OUTPUT(*PRINT) DBGVIEW(*SOURCE) REPLACE(*YES) +
                          TGTRLS(&TGTRLS)
             CRTCMOD    MODULE(&SRCLIB/LSTOBJBYTP) SRCFILE(&SRCLIB/QCSRC) SRCMBR(LSTOBJBYTP) OUTPUT(*PRINT) DBGVIEW(*SOURCE) +
                          REPLACE(*YES) TGTRLS(&TGTRLS)
             CRTCMOD    MODULE(&SRCLIB/MSGFUNC) SRCFILE(&SRCLIB/QCSRC) SRCMBR(MSGFUNC) OUTPUT(*PRINT) DBGVIEW(*SOURCE) REPLACE(*YES) +
                          TGTRLS(&TGTRLS)
             CRTCMOD    MODULE(&SRCLIB/SPCFUNC) SRCFILE(&SRCLIB/QCSRC) SRCMBR(SPCFUNC) OUTPUT(*PRINT) DBGVIEW(*SOURCE) REPLACE(*YES) +
                          TGTRLS(&TGTRLS)
             CRTCMOD    MODULE(&SRCLIB/SRVFUNC) SRCFILE(&SRCLIB/QCSRC) SRCMBR(SRVFUNC) OUTPUT(*PRINT) DBGVIEW(*SOURCE) REPLACE(*YES) +
                          TGTRLS(&TGTRLS)
             CRTCMOD    MODULE(&SRCLIB/SVRSTOP) SRCFILE(&SRCLIB/QCSRC) SRCMBR(SVRSTOP) OUTPUT(*PRINT) DBGVIEW(*SOURCE) REPLACE(*YES) +
                          TGTRLS(&TGTRLS)
             CRTCMOD    MODULE(&SRCLIB/TESTSVR) SRCFILE(&SRCLIB/QCSRC) SRCMBR(TESTSVR) OUTPUT(*PRINT) DBGVIEW(*SOURCE) REPLACE(*YES) +
                          TGTRLS(&TGTRLS)
             CRTCMOD    MODULE(&SRCLIB/WORKER) SRCFILE(&SRCLIB/QCSRC) SRCMBR(WORKER) OUTPUT(*PRINT) DBGVIEW(*SOURCE) REPLACE(*YES) +
                          TGTRLS(&TGTRLS)
             /* build the binding directory  */
             DLTBNDDIR  BNDDIR(&SRCLIB/OS)
             MONMSG     MSGID(CPF2105)
             CRTBNDDIR  BNDDIR(&SRCLIB/OS)
             ADDBNDDIRE BNDDIR(&SRCLIB/OS) OBJ((GENFUNC *SRVPGM *DEFER) (IPFUNC *SRVPGM *DEFER) (MSGFUNC *SRVPGM *DEFER) (SPCFUNC +
                          *SRVPGM *DEFER) (SRVFUNC *SRVPGM *DEFER))
             /* build the binder source */
             DLTF       FILE(&SRCLIB/QSRVSRC)
             MONMSG     MSGID(CPF2105)
             CRTSRCPF   FILE(&SRCLIB/QSRVSRC) RCDLEN(150)
             /* if modules exist retrive binder source */
             CHKOBJ     OBJ(&SRCLIB/IPFUNC) OBJTYPE(*MODULE)
             MONMSG     MSGID(CPF9801) EXEC(GOTO A1)
             RTVBNDSRC  MODULE(&SRCLIB/IPFUNC) SRCFILE(&SRCLIB/QSRVSRC)
 A1:         CHKOBJ     OBJ(&SRCLIB/MSGFUNC) OBJTYPE(*MODULE)
             MONMSG     MSGID(CPF9801) EXEC(GOTO A2)
             RTVBNDSRC  MODULE(&SRCLIB/MSGFUNC) SRCFILE(&SRCLIB/QSRVSRC)
 A2:         CHKOBJ     OBJ(&SRCLIB/SPCFUNC) OBJTYPE(*MODULE)
             MONMSG     MSGID(CPF9801) EXEC(GOTO A3)
             RTVBNDSRC  MODULE(&SRCLIB/SPCFUNC) SRCFILE(&SRCLIB/QSRVSRC)
 A3:         CHKOBJ     OBJ(&SRCLIB/SRVFUNC) OBJTYPE(*MODULE)
             MONMSG     MSGID(CPF9801) EXEC(GOTO A4)
             RTVBNDSRC  MODULE(&SRCLIB/SRVFUNC) SRCFILE(&SRCLIB/QSRVSRC)
 A4:         CHKOBJ     OBJ(&SRCLIB/GENFUNC) OBJTYPE(*MODULE)
             MONMSG     MSGID(CPF9801) EXEC(GOTO B1)
             RTVBNDSRC  MODULE(&SRCLIB/GENFUNC) SRCFILE(&SRCLIB/QSRVSRC)

 B1:         /* add the OSPGM to the library list for picking up SRVPGM */
             ADDLIBLE   LIB(&PGMLIB) POSITION(*FIRST)
             CHKOBJ     OBJ(&SRCLIB/MSGFUNC) OBJTYPE(*MODULE)
             MONMSG     MSGID(CPF9801) EXEC(GOTO B2)
             CRTSRVPGM  SRVPGM(&PGMLIB/MSGFUNC) MODULE(&SRCLIB/MSGFUNC) SRCFILE(&SRCLIB/QSRVSRC) BNDDIR(&SRCLIB/OS) TGTRLS(&TGTRLS)
 B2:         CHKOBJ     OBJ(&SRCLIB/IPFUNC) OBJTYPE(*MODULE)
             MONMSG     MSGID(CPF9801) EXEC(GOTO B3)
             CRTSRVPGM  SRVPGM(&PGMLIB/IPFUNC) MODULE(&SRCLIB/IPFUNC) SRCFILE(&SRCLIB/QSRVSRC) BNDDIR(&SRCLIB/OS) TGTRLS(&TGTRLS)
 B3:         CHKOBJ     OBJ(&SRCLIB/SPCFUNC) OBJTYPE(*MODULE)
             MONMSG     MSGID(CPF9801) EXEC(GOTO B4)
             CRTSRVPGM  SRVPGM(&PGMLIB/SPCFUNC) MODULE(&SRCLIB/SPCFUNC) SRCFILE(&SRCLIB/QSRVSRC) BNDDIR(&SRCLIB/OS) TGTRLS(&TGTRLS)
 B4:         CHKOBJ     OBJ(&SRCLIB/SRVFUNC) OBJTYPE(*MODULE)
             MONMSG     MSGID(CPF9801) EXEC(GOTO B5)
             CRTSRVPGM  SRVPGM(&PGMLIB/SRVFUNC) MODULE(&SRCLIB/SRVFUNC) SRCFILE(&SRCLIB/QSRVSRC) BNDDIR(&SRCLIB/OS) TGTRLS(&TGTRLS)
 B5:         CHKOBJ     OBJ(&SRCLIB/GENFUNC) OBJTYPE(*MODULE)
             MONMSG     MSGID(CPF9801) EXEC(GOTO C1)
             CRTSRVPGM  SRVPGM(&PGMLIB/GENFUNC) MODULE(&SRCLIB/GENFUNC) SRCFILE(&SRCLIB/QSRVSRC) BNDDIR(&SRCLIB/OS) TGTRLS(&TGTRLS)
             /* create the programs */
 C1:         CRTPGM     PGM(&PGMLIB/CHKMSGID) MODULE(&SRCLIB/CHKMSGID) TEXT('') BNDDIR(&SRCLIB/OS) TGTRLS(&TGTRLS)
             CRTPGM     PGM(&PGMLIB/CHKSIGS) MODULE(&SRCLIB/CHKSIGS) TEXT('') BNDDIR(&SRCLIB/OS) TGTRLS(&TGTRLS)
             CRTPGM     PGM(&PGMLIB/COPYSRCA) MODULE(&SRCLIB/COPYSRCA) TEXT('') BNDDIR(&SRCLIB/OS) TGTRLS(&TGTRLS)
             CRTPGM     PGM(&PGMLIB/FNDMSGCNT) MODULE(&SRCLIB/FNDMSGCNT) TEXT('') BNDDIR(&SRCLIB/OS) TGTRLS(&TGTRLS)
             CRTPGM     PGM(&PGMLIB/HTTPGET) MODULE(&SRCLIB/HTTPGET) TEXT('') BNDDIR(&SRCLIB/OS) TGTRLS(&TGTRLS)
             CRTPGM     PGM(&PGMLIB/LSTOBJBYTP) MODULE(&SRCLIB/LSTOBJBYTP) TEXT('') BNDDIR(&SRCLIB/OS) TGTRLS(&TGTRLS)
             CRTPGM     PGM(&PGMLIB/SVRSTOP) MODULE(&SRCLIB/SVRSTOP) TEXT('') BNDDIR(&SRCLIB/OS) TGTRLS(&TGTRLS)
             CRTPGM     PGM(&PGMLIB/TESTSVR) MODULE(&SRCLIB/TESTSVR) TEXT('') BNDDIR(&SRCLIB/OS) TGTRLS(&TGTRLS)
             CRTPGM     PGM(&PGMLIB/WORKER) MODULE(&SRCLIB/WORKER) TEXT('') BNDDIR(&SRCLIB/OS) TGTRLS(&TGTRLS)
             /* create the commands */
             CRTCMD     CMD(&SRCLIB/CHKMSGID) PGM(CHKMSGID)
             CRTCMD     CMD(&SRCLIB/CHKSIGS) PGM(CHKSIGS)
             CRTCMD     CMD(&SRCLIB/COPYSRCA) PGM(COPYSRCA)
             CRTCMD     CMD(&SRCLIB/FNDMSGCNT) PGM(FNDMSGCNT)
             CRTCMD     CMD(&SRCLIB/LSTOBJBYTP) PGM(LSTOBJBYTP)
             /* create the menu for the server */
             CRTMNU     MENU(&PGMLIB/SVRMAIN) TYPE(*UIM) SRCFILE(&SRCLIB/QUIMSRC)
             /* create the msgfile */
             CRTMSGF    MSGF(&PGMLIB/OSMSGF)
             ADDMSGD    MSGID(GEN0001) MSGF(&PGMLIB/OSMSGF) MSG('&1') FMT((*CHAR 1024))
             ADDMSGD    MSGID(MSG0001) MSGF(&PGMLIB/OSMSGF) MSG('We have just called API &1') SECLVL('Just a simple message stating +
                          we have called an API') FMT((*CHAR 10))
             RMVLIBLE   LIB(&PGMLIB)
endpgm

