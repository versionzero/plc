/*----------------------------------------------------------------------
  File    : error.cc
  Contents: 
  Author  : Ben Burnett
  History : 10.01.2007 file created
----------------------------------------------------------------------*/

#if HAVE_CONFIG_H
#include <config.h>
#endif 

#include "error.h"
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/

/* --- error messages --- */
static const char *errmsgs[] = {
  /* E_NONE      0 */  "no error\n",
  /* E_NOMEM    -1 */  "not enough memory\n",
  /* E_FOPEN    -2 */  "cannot open file %s\n",
  /* E_FREAD    -3 */  "read error on file %s\n",
  /* E_FWRITE   -4 */  "write error on file %s\n",
  /* E_NSRC     -5 */  "no source file supplied\n",
  /* E_OPTION   -6 */  "unknown option -%c\n",
  /* E_OPTARG   -7 */  "missing option argument\n",
  /* E_ARGCNT   -8 */  "wrong number of arguments\n",
  /* E_UNKSYB   -9 */  "unknown symbol '%s'\n"
};

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

void error ( error_code code, ... )
{                               /* --- print an error message */
  va_list    args;              /* list of variable arguments */
  const char *msg;              /* error message */

  if ( code < E_UNKNOWN ) {
    code = E_UNKNOWN;
  }
  if ( code < 0 ) {             /* if to report an error, */
    msg = errmsgs[-code];       /* get the error message */
    if (!msg) msg = errmsgs[-E_UNKNOWN];
    fprintf ( stderr, "\n%s: ", PACKAGE );
    va_start ( args, code );    /* get variable arguments */
    vfprintf ( stderr, msg, args ); /* print error message */
    va_end ( args );            /* end argument evaluation */
  }
  exit ( code );                /* abort the program */
}  /* error() */

