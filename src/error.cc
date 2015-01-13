/*----------------------------------------------------------------------
  File    : error.cc
  Contents: 
  Author  : Ben Burnett
  History : 10.01.2007 file created
----------------------------------------------------------------------*/

#if HAVE_CONFIG_H
#include <config.h>
#endif 

#include "compiler.h"
#include "error.h"
#include <iostream>

/*----------------------------------------------------------------------
  Namespace Inclusions
----------------------------------------------------------------------*/

using std::cerr;
using std::string;

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/

#if 0
/* E_OPTARG   -7 */  "missing option argument\n",
#endif

/* --- error messages --- */
static const char *_fatal_messages[] = {
  /*   0 */  "no error\n",
  /*  -1 */  "not enough memory\n",
  /*  -2 */  "cannot open file %s\n",
  /*  -3 */  "read error on file %s\n",
  /*  -4 */  "write error on file %s\n",
  /*  -5 */  "no source file supplied\n",
  /*  -6 */  "no destination file supplied\n",
  /*  -7 */  "unknown option -%c\n",
  /*  -8 */  "wrong number of arguments\n",
  /*  -9 */  "unknown error\n",
};  

static const char *_input_messages[] = {
  /*   0 */  "no error\n",
  /*  -1 */  "unrecognized symbol `%s'\n",
  /*  -2 */  "unexpected symbol `%s'\n",
  /*  -3 */  "expected `%s' before `%s' token\n",
  /*  -4 */  "duplicate symbol `%s'\n",
  /*  -5 */  "undefined symbol `%s'\n",
  /*  -6 */  "non-constant `%s' used in a constant expression\n",
  /*  -7 */  "type mismatch\n",
  /*  -8 */  "`%s' is not a procedure\n",
  /*  -9 */  "unbalanced assignment statement; %s is heavy\n",
  /* -10 */  "boolean expression expected\n",
  /* -11 */  "integer expression expected\n",
  /* -12 */  "unknown error\n",
};

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

void compiler::error ( error::application::code code, ... ) const
{                               /* --- print an error message and quit */
  va_list     args;             /* list of variable arguments */
  const char *msg;              /* error message */
  if ( code < error::application::unknown ) {
    code = error::application::unknown; }
  if ( code < 0 ) {             /* if to report an error, */
    msg = _fatal_messages[-code]; /* get the error message */
    if ( !msg ) { msg = _fatal_messages[-error::application::unknown]; }   
    fprintf ( stderr, "%s: ", PACKAGE );
    va_start ( args, code );    /* get variable arguments */
    vfprintf ( stderr, msg, args ); /* print error message */
    va_end ( args );            /* end argument evaluation */
  }
  exit ( code );                /* abort the program */
}  

/* --------------------------------------------------------------------*/

void compiler::error ( error::input::code code, ... ) const
{                               /* --- print an error message */
  va_list     args;             /* list of variable arguments */
  const char *msg;              /* error message */
  if ( code < error::input::unknown ) { 
    code = error::input::unknown; }
  if ( code < 0 ) {             /* if to report an error, */
    msg = _input_messages[-code]; /* get the error message */
    if ( !msg ) { msg = _input_messages[-error::input::unknown]; }
    fprintf ( stderr, "%s:%3d: error: ", _fn_in, _parser.line () );
    va_start ( args, code );    /* get variable arguments */
    vfprintf ( stderr, msg, args ); /* print error message */    
    va_end ( args );            /* end argument evaluation */  
  }  
}  
