/*===================================================================*/
/*  Command name :  DSPIFSLCK                                        */
/*  CPP..........:  DSPIFSLCK                                        */
/*                                                                   */
/*  Author.......:  Chris Hird                                       */
/*  Date created :  September 2018                                   */
/*                                                                   */
/*  Purpose......:  Retrieve IFS locks for path                      */
/*                                                                   */
/*  Revision log.:                                                   */
/*  Date     Author    Revision                                      */
/*                                                                   */
/*  @Copyright Shield Advanced Solutions Ltd. Canada 1997 - 2018     */
/*===================================================================*/

             CMD        PROMPT('Display IFS Locks')
             PARM       KWD(PATH) TYPE(*CHAR) LEN(5002) RSTD(*NO) ALWUNPRT(*NO) ALWVAR(*NO) VARY(*YES *INT2) PROMPT('Path Name')
