/*----------------------------------------------------------------------
  File    : compiler.h
  Contents: 
  Author  : Ben Burnett
  History : 15.03.2007 file created
----------------------------------------------------------------------*/

#ifndef COMPILER_H
#define COMPILER_H

#include "error.h"
#include "emitter.h"
#include "parser.h"
#include "symboltbl.h"
#include <exception>
#include <fstream>
#include <sstream>
#include <string>

/*----------------------------------------------------------------------
  Forward Declarations
----------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------
  Main Class
----------------------------------------------------------------------*/

class compiler : public error_interface, public emitter_interface {

 private:

  char             *_fn_in;      /* input file-name */
  char             *_fn_out;     /* output file-name */
  std::ifstream     _fin;        /* input file stream */  
  std::stringstream _sasm;       /* asm stream */
  std::ofstream     _fout;       /* final output file stream */
  symboltbl         _symbols;    /* main symbol table */
  parser            _parser;     /* PL language parser */
  bool              _verbose;    /* noisy output */
  
  void parse ( int, char*[] );

  void error ( error::application::code, ... ) const;
  void error ( error::input::code, ... ) const;
  
  void emit ( std::string const& );  
  void emit ( std::string const&, int );  
  void emit ( std::string const&, int , int );  
    
 public:
  
  compiler ( int, char*[] );
  int compile ();

};

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
