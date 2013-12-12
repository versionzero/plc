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
#include "assembler.h"
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
using std::exception;
using std::ifstream;
using std::ofstream;
using std::setw;
using std::setfill;
using std::string;
using std::stringstream;

namespace apperr = error::application;

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

compiler::compiler ( int argc, char *argv[] ) 
  : _sasm ( stringstream::in | stringstream::out ),
    _parser ( _fin, _symbols, *this, *this ) {
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
        case 'v': _verbose = true;               break;
        default : 
	  error ( apperr::unknown_option, *--s ); break;
        }                       /* set option variables */
        if ( optarg && *s ) { *optarg = s; optarg = NULL; break; }
      } }                       /* get option argument */
    else {                      /* -- if argument is no option */
      switch ( k++ ) {          /* evaluate non-options */
      case 0: _fn_in  = s;                       break;
      case 1: _fn_out = s;                       break;
      default: error ( apperr::argument_count ); break;
      }                         /* note filenames */
    }
  } 

  /* --- open source file --- */
  if ( _fn_in ) {
    _fin.open ( _fn_in, ifstream::in );
    if ( !_fin.good () ) {
      error ( apperr::file_open, _fn_in );
    }
  } else {
    error ( apperr::no_source_file, _fn_in );
  }

  /* --- open output file --- */
  if ( _fn_out ) {
    _fout.open ( _fn_out, ofstream::out );
    if ( !_fout.good () ) {
      error ( apperr::file_open, _fn_out );
    }	
    /* this is a cute little STL trick: if there is a output file, 
       redirect the stdout to it. */
    _fout.copyfmt ( cout );     /* copy all format information */  
    cout.rdbuf ( _fout.rdbuf () );
  }       

}

/*--------------------------------------------------------------------*/

int compiler::compile () {
    
  try {
  
    /* --- parse the PL source --- */  
    _parser.parse ();           
    _fin.close ();              /* close the PL source file */

    /* --- assemble the source --- */
    Assembler assembler ( _sasm, cout );
    assembler.firstPass (); 
    _sasm.seekg ( stringstream::beg ); 
    assembler.secondPass ();
    _fout.close ();
   
  } catch ( exception & e ) {   /* -- all fatals */
    cerr << "error: " << e.what () << '\n';
  }
  
  return 0;
  
}

