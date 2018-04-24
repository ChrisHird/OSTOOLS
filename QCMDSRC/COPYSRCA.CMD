/*===================================================================*/
/*  COMMAND name :  COPYSRCA                                         */
/*  Author name..:  Chris Hird                                       */
/*  Date created :  April 2018                                       */
/*                                                                   */
/*                                                                   */
/*  Purpose......:  Copy Source by inclusion                         */
/*  CPP..........:  COPYSRCA                                         */
/*  Revision log.:                                                   */
/*  Date     Author    Revision                                      */
/*                                                                   */
/*  @Copyright Chris Hird 2018                                       */
/*===================================================================*/

             CMD        PROMPT('Copy Source include')

             PARM       KWD(SRCF) TYPE(QUAL1) MIN(1) PROMPT('Source File')
             PARM       KWD(TGTF) TYPE(QUAL1) MIN(1) PROMPT('Target File')
             PARM       KWD(INCSTR) TYPE(*CHAR) LEN(50) MIN(1) VARY(*YES *INT2) PROMPT('Include String')

 QUAL1:      QUAL       TYPE(*NAME) LEN(10)
             QUAL       TYPE(*NAME) LEN(10) DFT(*LIBL) SPCVAL((*LIBL)) PROMPT('Library name:')
