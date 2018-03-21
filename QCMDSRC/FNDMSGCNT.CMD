/*===================================================================*/
/*  COMMAND name :  FNDMSGCNT                                        */
/*  Author name..:  Chris Hird                                       */
/*  Date created :  March 2018                                       */
/*                                                                   */
/*                                                                   */
/*  Purpose......:  Find message file content                        */
/*  CPP..........:  FNDMSGFCNT                                       */
/*  Revision log.:                                                   */
/*  Date     Author    Revision                                      */
/*                                                                   */
/*  @Copyright Chris Hird 2018                                       */
/*===================================================================*/

             CMD        PROMPT('Find MSGF Content')

             PARM       KWD(MSGF) TYPE(QUAL1) MIN(1) PROMPT('Message File')
             PARM       KWD(SSTR) TYPE(*CHAR) LEN(50) MIN(1) VARY(*YES *INT2) PROMPT('Search String')

 QUAL1:      QUAL       TYPE(*NAME) LEN(10)
             QUAL       TYPE(*NAME) LEN(10) DFT(*LIBL) SPCVAL((*LIBL)) PROMPT('Library name:')
