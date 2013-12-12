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
#include <exception>
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

enum character_code {
  C_LETTER,
  C_NUMERAL,
  C_SYMBOL,
  C_WS,
  C_ERROR
};

/*----------------------------------------------------------------------
  Main Class
----------------------------------------------------------------------*/

class scanner {

private:
  
  std::ifstream & _fin;         /* input stream reference */
  symboltbl &     _table;       /* main symbol table */
  int             _cpos;        /* current column number */
  int             _cline;       /* current line number */
  character_code  _char_map[256];/* character characterization map */
  token           _ctoken;      /* current token */
  
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

  scanner ( std::ifstream &, symboltbl & );  
  token const & next_token ();  
  int line () const;
  int column () const;   

  /* -- base for scanner exceptions */
  class scanner_error : public std::exception {
  protected:
    std::string _msg;
  public:
    scanner_error ();
    /* -- takes a character string describing the error.  */
    explicit scanner_error ( std::string const & );    
    virtual ~scanner_error () throw ();
    /* -- returns a C-style character string describing the general cause of
          the current error (possibly the same string passed to the ctor). */
    virtual const char* what () const throw ();
  };
  
  /*-- thrown when we don't recognize a symbol in the input stream */
  struct unknown_symbol : public scanner_error {    
    explicit unknown_symbol ( char );    
  };

};

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
