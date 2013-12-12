/*----------------------------------------------------------------------
  File    : error.h
  Contents: 
  Author  : Ben Burnett
  History : 10.01.2007 file created
----------------------------------------------------------------------*/

#ifndef ERROR_H
#define ERROR_H

/*----------------------------------------------------------------------
  Error Codes
----------------------------------------------------------------------*/

enum error_code {
  E_NONE    =  0,               /* no error */
  E_NOMEM   = -1,               /* not enough memory */
  E_FOPEN   = -2,               /* cannot open file */
  E_FREAD   = -3,               /* read error on file */
  E_FWRITE  = -4,               /* write error on file */  
  E_NSRC    = -5,               /* no source file */
  E_OPTION  = -6,               /* unknown option */
  E_OPTARG  = -7,               /* missing option argument */
  E_ARGCNT  = -8,               /* too few/many arguments */
  E_SYNTAX  = -9,               /* syntax error */
  E_SYMBOL  = -10,              /* undefined symbol */
  E_CONST   = -11,              /* non-constant used in a constant expression */
  E_TYPE    = -12,              /* type error */
  E_DUP     = -13,              /* duplicate symbol */
  E_PROC    = -14,              /* not a procedure */
  E_BOOLEAN = -15,              /* boolean expression expected */
  E_UNKNOWN = -16               /* unknown error */
};

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

void error ( error_code code, ... );

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/



