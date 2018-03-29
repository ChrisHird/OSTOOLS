/*===================================================================*/
/*  Command name :  CHKSIGS                                          */
/*  Author name..:  Chris Hird                                       */
/*  Date created :  March 2018                                       */
/*                                                                   */
/*  Purpose......:  Check pgm and srvpgm sigs in lib                 */
/*  CPP..........:  chksigs                                          */
/*  Revision log.:                                                   */
/*  Date     Author    Revision                                      */
/*                                                                   */
/*  @Copyright Chris Hird 2018                                       */
/*===================================================================*/

             CMD        PROMPT('Check signatures')
             PARM       KWD(LIB) TYPE(*CHAR) LEN(10) MIN(1) CHOICE(*VALUES) PROMPT('Library to be Checked')
