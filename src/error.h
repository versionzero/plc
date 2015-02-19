/*----------------------------------------------------------------------
  File    : error.h
  Contents: Compiler error codes
  Author  : Ben Burnett
  History : 10.01.2007 file created
----------------------------------------------------------------------*/

#ifndef ERROR_H
#define ERROR_H

#include <cstdarg>

/*----------------------------------------------------------------------
  Application error codes
----------------------------------------------------------------------*/

namespace error {
  namespace application {
    enum code {    
      none           =   0,     /* no error */
      no_mem         =  -1,     /* not enough memory */
      file_open      =  -2,     /* cannot open file */
      file_read      =  -3,     /* read error on file */
      file_write     =  -4,     /* write error on file */  
      no_source_file =  -5,     /* no source file */
      no_dest_file   =  -6,     /* no dest file */
      unknown_option =  -7,     /* unknown option */    
      argument_count =  -8,     /* too few/many arguments */
      unknown        =  -9      /* unknown error */
    };
  }
}

/*----------------------------------------------------------------------
  Input error codes
----------------------------------------------------------------------*/

namespace error {
  namespace input {
    enum code {
      /* --- syntax error */
      none             =   0,   /* no error */
      unrecognized     =  -1,   /* unknown symbol */
      unexpected       =  -2,   /* unexpected symbol */
      expected_before  =  -3,   /* expected `x' before `y' token */
      /* --- scope error */
      duplicate        =  -4,   /* duplicate symbol */
      undefined_symbol =  -5,   /* undefined symbol */
      /* --- type error */
      constant         =  -6,   /* non-constant in a constant expression */
      type_mismatch    =  -7,   /* type mismatch */
      procedure        =  -8,   /* not a procedure */
      balance          =  -9,   /* unbalanced assignment statement */
      boolean          = -10,   /* boolean expression expected */ 
      integer          = -11,   /* integer expression expected */ 
      unknown          = -12    /* unknown error */
    };
  }
}

#if 0
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
#endif 

/* --------------------------------------------------------------------*/

/* -- error handling interface */
struct error_interface {

  virtual ~error_interface () {};
  
  virtual void error (error::application::code, ...) const = 0;
  virtual void error (error::input::code, ...) const = 0;  

};

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
