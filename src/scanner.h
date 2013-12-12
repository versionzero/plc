/*----------------------------------------------------------------------
  File    : scanner.h
  Contents: 
  Author  : Ben Burnett
  History : 10.01.2007 file created
----------------------------------------------------------------------*/

#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"
#include "error.h"
#include <stdexcept>
#include <fstream>
#include <string>

/*----------------------------------------------------------------------
  Forward Declarations
----------------------------------------------------------------------*/

class token;

/*----------------------------------------------------------------------
  Character Codes
----------------------------------------------------------------------*/

namespace character {
  enum code {
    letter,
    digit,
    symbol,
    ws,
    error
  };
}

/*----------------------------------------------------------------------
  Main Class
----------------------------------------------------------------------*/

class scanner {

private:
  
  std::ifstream  &_fin;         /* input stream reference */  
  character::code _char_map[256]; /* characterization map */
  int             _column;      /* current column number */
  int             _line;        /* current line number */  
  token           _token;       /* current token */
  
  bool iswhite ( int ) const;
  bool iswordchar ( int ) const;
  bool isnumeral ( int ) const;
  
  void skipws ();
  void get ( char & );
  void putback ( char );
  char peek ();

  void scan_word ();
  void scan_numeral ();
  void scan_symbol ();

public:

  scanner ( std::ifstream & );  
  token const & next_token () throw ( std::runtime_error );  
  int line () const;
  int column () const;   
  
};

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
