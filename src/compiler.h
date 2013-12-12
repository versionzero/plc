/*----------------------------------------------------------------------
  File    : compiler.h
  Contents: 
  Author  : Ben Burnett
  History : 15.03.2007 file created
----------------------------------------------------------------------*/

#ifndef COMPILER_H
#define COMPILER_H

#include "error.h"
#include "symboltbl.h"
#include <fstream>

/*----------------------------------------------------------------------
  Forward Declarations
----------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------
  Main Class
----------------------------------------------------------------------*/

class compiler {

 private:

  char            *_fn_in;      /* input source files */
  std::ifstream    _fin;        /* input stream */
  symboltbl        _symbols;    /* main symbol table */
  bool             _verbose;    /* noisy output */

  void parse ( int argc, char *argv[] );

 public:
  
  compiler ( int argc, char *argv[] );
  int compile ();

};

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
