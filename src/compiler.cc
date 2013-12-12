/*----------------------------------------------------------------------
  File    : compiler.cc
  Contents: 
  Author  : Ben Burnett
  History : 15.03.2007 file created
----------------------------------------------------------------------*/

#if HAVE_CONFIG_H
#include <config.h>
#endif 

#include "compiler.h"
#include "parser.h"
#include <iostream>
#include <exception>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

/*----------------------------------------------------------------------
  Namespace Inclusions
----------------------------------------------------------------------*/

using std::cin;
using std::cout;
using std::cerr;
using std::setw;
using std::setfill;
using std::string;
using std::ifstream;
using std::ofstream;
using std::exception;

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Helper Methods
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Main Methods
----------------------------------------------------------------------*/

compiler::compiler ( int argc, char *argv[] ) {
  parse ( argc, argv );
}

/*--------------------------------------------------------------------*/

void compiler::parse ( int argc, char *argv[] ) {
  int    k = 0;                 /* counter */
  char   *s;                    /* to traverse the options */
  char   **optarg = NULL;       /* option argument */
  
  /* --- evaluate arguments --- */
  for ( int i = 1; i < argc; ++i ) { /* traverse arguments */
    s = argv[i];                /* get option argument */
    if ( optarg ) { *optarg = s; optarg = NULL; continue; }
    if ( '-' == *s && *++s ) {  /* -- if argument is an option */
      while ( *s ) {            /* traverse options */
        switch ( *s++ ) {       /* evaluate switches */
        case 'v': _verbose = true;        break;
        default : 
	  error ( E_OPTION, *--s ); break;
        }                       /* set option variables */
        if ( optarg && *s ) { *optarg = s; optarg = NULL; break; }
      } }                       /* get option argument */
    else {                      /* -- if argument is no option */
      switch ( k++ ) {          /* evaluate non-options */
      case  0: _fn_in = s;               break;
      default: error ( E_ARGCNT ); break;
      }                         /* note filenames */
    }
  } 

  /* --- open source file --- */
  if ( _fn_in ) {
    _fin.open ( _fn_in, ifstream::in );
    if ( !_fin.good () ) {
      error ( E_FOPEN, _fn_in );
    }
  } else {
    error ( E_NSRC, _fn_in );
  }

}

/*--------------------------------------------------------------------*/

int compiler::compile () {

  /* --- create parser --- */  
  try {
    parser pl_parser ( _fin, _fn_in, _symbols );
    pl_parser.parse ();
  } catch ( exception & e ) {   /* -- all fatals */
    cerr << "error: " << e.what () << '\n';
  }

  return 0;

}

