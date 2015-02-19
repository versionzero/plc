/*----------------------------------------------------------------------
  File    : scanner.h
  Contents: 
  Author  : Ben Burnett
  History : 10.01.2007 file created
----------------------------------------------------------------------*/

#ifndef SCANNER_H
#define SCANNER_H

#include "symboltbl.h"
#include "token.h"
#include "error.h"
#include <stdexcept>
#include <fstream>
#include <string>

/*----------------------------------------------------------------------
  Forward Declarations
----------------------------------------------------------------------*/

class token;
class symboltbl;

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
  
  std::ifstream & _fin;         /* input stream reference */
  symboltbl       _symbols;     /* main symbol table */
  character::code _char_map[256];/* character characterization map */
  unsigned int    _column,      /* current column number */
                  _line;        /* current line number */  
  token           _token;       /* current token */
  
  bool iswhite (int) const;
  bool iswordchar (int) const;
  bool isnumeral (int) const;
  
  void skipws ();
  void get (char &);
  void putback (char);
  char peek ();

  void scan_word ();
  void scan_numeral ();
  void scan_symbol ();

public:

  scanner (std::ifstream &);  
  token const & next_token ();
  unsigned int line () const;
  unsigned int column () const;   
  
};

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
