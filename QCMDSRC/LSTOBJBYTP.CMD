/*===================================================================*/
/*  COMMAND name :  LSTOBJBYTP                                       */
/*  Author name..:  Chris Hird                                       */
/*  Date created :  March 2018                                       */
/*                                                                   */
/*                                                                   */
/*  Purpose......:  List objects of certain type in library          */
/*  CPP..........:  LSTOBJBYTP                                       */
/*  Revision log.:                                                   */
/*  Date     Author    Revision                                      */
/*                                                                   */
/*  @Copyright Chris Hird 2018                                       */
/*===================================================================*/
             CMD        PROMPT('List Objects of Type')
             PARM       KWD(LIB) TYPE(*char) LEN(10) MIN(1) PROMPT('Library Name')
             PARM       KWD(TYPE) TYPE(*CHAR) LEN(10) MIN(1) PROMPT('Object Type')
