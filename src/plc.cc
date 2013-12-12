/*----------------------------------------------------------------------
  File    : ply.cc
  Contents: 
  Author  : Ben Burnett
  History : 10.01.2007 file created
----------------------------------------------------------------------*/

#if HAVE_CONFIG_H
#include <config.h>
#endif 

#include "parser.h"
#include "error.h"
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

#ifdef NDEBUG
#define DESCRIPTION "PL language compiler"
#else
#define DESCRIPTION "PL language compiler [debug build]"
#endif

/*----------------------------------------------------------------------
  Global Variables
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Main Functions
----------------------------------------------------------------------*/

void help () {                  /* --- print help */
}

/*--------------------------------------------------------------------*/

int main ( int argc, char *argv[] ) { /* --- main function */

  int       k = 0;               /* counter */
  char      *s;                  /* to traverse the options */
  char      **optarg = NULL;     /* option argument */
  bool      verbose;             /* show extra information */
  char      *fn_in,              /* source and bytecode files */ 
            *fn_out;
  ifstream  fin;                 /* input stream */
  ofstream  fout;                /* ouput stream */
  symboltbl symbols;             /* main symbol table */
				   
  /* --- print usage message --- */
  if ( argc > 1 ) {             /* if arguments are given */
    cerr << PACKAGE_STRING      /* print a startup message */
	 << " - " << DESCRIPTION << " ("
	 << PACKAGE_BUGREPORT << ")" << '\n';    
  } else {                      /* if no arguments given */
    cout << "usage: " << PACKAGE << " [options] infile outfile" << '\n';
    cout << "infile     file to read PL code" << '\n';
    cout << "outfile    file to write VM code to" << '\n';
    return 0;                   /* print a usage message */
  }                             /* and abort the program */

  /* --- evaluate arguments --- */
  for ( int i = 1; i < argc; ++i ) { /* traverse arguments */
    s = argv[i];                /* get option argument */
    if ( optarg ) { *optarg = s; optarg = NULL; continue; }
    if ( '-' == *s && *++s ) {  /* -- if argument is an option */
      while ( *s ) {            /* traverse options */
        switch ( *s++ ) {       /* evaluate switches */
        case 'v': verbose = true;           break;
        default : error ( E_OPTION, *--s ); break;
        }                       /* set option variables */
        if ( optarg && *s ) { *optarg = s; optarg = NULL; break; }
      } }                       /* get option argument */
    else {                      /* -- if argument is no option */
      switch ( k++ ) {          /* evaluate non-options */
      case  0: fn_in = s;          break;
      case  1: fn_out = s;         break;
      default: error ( E_ARGCNT ); break;
      }                         /* note filenames */
    }
  } 

  /* --- open source file --- */
  if ( fn_in ) {
    fin.open ( fn_in, ifstream::in );
    if ( !fin.good () ) {
      error ( E_FOPEN, fn_in );
    }
  } else {
    error ( E_NSRC, fn_in );
  }

  /* --- create parser --- */  
  try {
    parser pl_parser ( fin, fn_in, symbols );
    pl_parser.parse ();
  } catch ( exception & e ) {   /* -- all errors */
    cerr << "error: " << e.what () << '\n';
  }

#if 0
  /* --- create scanner --- */  
  scanner scan ( fin, table );
  try {
    token tok = scan.next_token ();
    for ( ; tok != eof_token; tok = scan.next_token () ) {
      string name  = token::token_name ( tok ), 
	     value = tok.value ();      
      cout << fn_in << ":" << scan.line () 
	   << "[" << scan.column () - value.length () << "]: "
	   << name << ": " << value << '\n';    
    }
  } catch ( scanner::unknown_symbol & e ) { /* -- scanner error */
    cerr << fn_in << ":" <<  scan.line () << "[" << scan.column () 
	 << "]: error: " << e.what () << '\n';
  } catch ( exception & e ) { /* -- all other errors */
    cerr << "Error: " << e.what () << '\n';
  }
#endif 
  
}
