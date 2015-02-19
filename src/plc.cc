/*----------------------------------------------------------------------
  File    : ply.cc
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

using std::cout;
using std::cerr;
using std::string;

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

int main (int argc, char *argv[]) { /* --- main function */
  
  /* --- print usage message --- */
  if (argc > 1) {             /* if arguments are given */
    cerr << PACKAGE_STRING      /* print a startup message */
	 << " - " << DESCRIPTION << " ("
	 << PACKAGE_BUGREPORT << ")" << '\n';    
  } else {                      /* if no arguments given */
    cout << "usage: " << PACKAGE << " [options] infile [outfile]" << '\n';
    cout << "infile     file to read PL code" << '\n';
    cout << "outfile    file to write VM code to" << '\n';
    return 0;                   /* print a usage message */
  }                             /* and abort the program */

  /* --- create an instance of the compiler and attempt to compile
     the source file declared on the command-line */
  compiler plc (argc, argv);
  return plc.compile ();
  
}
