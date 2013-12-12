/*----------------------------------------------------------------------
  File    : error.h
  Contents: 
  Author  : Ben Burnett
  History : 10.01.2007 file created
----------------------------------------------------------------------*/

#ifndef ERROR_H
#define ERROR_H

#include <exception>
#include <string>

/*----------------------------------------------------------------------
  Compiler Error Codes (Temporary, during beggining phases)
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
  E_UNKSYB  = -9,               /* unknown symbol */
  E_UNKNOWN = -10               /* unknown error */
};

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

void error ( error_code code, ... );

/*----------------------------------------------------------------------
  Exceptions (Future error handling, once the project is all done)
----------------------------------------------------------------------*/

/* --- project language compiler base exception */
class plc_exception : public std::exception { 
  
private:
  
  error_code _code;

public:

  plc_exception ( error_code ) throw ();
  operator error_code () const throw ();

};

/* --------------------------------------------------------------------*/

/* --- application level exception base */
/*
class compiler_exception : public pcl_exception { 
};
*/

/* --------------------------------------------------------------------*/

/* --- scanner level exception base */
/*
class scanner_exception : public pcl_exception { 
};
*/

/* --------------------------------------------------------------------*/

/*
class unkown_symbol : public scanner_exception {

  char _symbol;

 public:

  unkown_symbol ( char c ) throw ();
  const char* what () const throw ();

};
*/

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/



