/*----------------------------------------------------------------------
  File    : parser.h
  Contents: 
  Author  : Ben Burnett
  History : 04.02.2007 file created
----------------------------------------------------------------------*/

#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "symboltbl.h"
#include "token.h"
#include "setops.h"
#include <fstream>
#include <exception>

/*----------------------------------------------------------------------
  Forward Declarations
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/

#define NONTERMINAL_PARAMS ( token_set const & stop )
#define NONTERMINAL_HANDLER(r,x) r x NONTERMINAL_PARAMS;

/*----------------------------------------------------------------------
  Main Class
----------------------------------------------------------------------*/

class parser {

private:

  scanner     _scanner;         /* stream scanner (lexer) */
  const char *_fn_in;           /* input filename */
  symboltbl  &_symbols;         /* main symbol table */  
  token       _token;           /* current token */
  token_code  _expected;        /* last expected token */
  token_set   _first_symbols[LAST_TOKEN]; /* BNF derived first symbols */
  
  token_code move ();
  void expect ( token_code, token_set const & );
  void error ( error_code ) const;
  void syntax_error ( token_set const & );
  void syntax_check ( token_set const & );

  /* --- a method for each (useful) non-terminal in the PL grammar */
  NONTERMINAL_HANDLER ( void, program );
  NONTERMINAL_HANDLER ( void, block );
  NONTERMINAL_HANDLER ( void, definition_part );
  NONTERMINAL_HANDLER ( void, constant_definition );
  NONTERMINAL_HANDLER ( void, variable_definition );
  NONTERMINAL_HANDLER ( void, procedure_definition );
  NONTERMINAL_HANDLER ( void, statement_part );
  NONTERMINAL_HANDLER ( void, variable_access_list );
  NONTERMINAL_HANDLER ( void, expression_list );
  NONTERMINAL_HANDLER ( void, guarded_command_list );
  NONTERMINAL_HANDLER ( void, guarded_command );
  NONTERMINAL_HANDLER ( void, expression );
  NONTERMINAL_HANDLER ( void, primary_expression );
  NONTERMINAL_HANDLER ( void, simple_expression );
  NONTERMINAL_HANDLER ( void, term );
  NONTERMINAL_HANDLER ( void, multiplying_operator );
  NONTERMINAL_HANDLER ( void, factor );
  NONTERMINAL_HANDLER ( void, variable_access );
  NONTERMINAL_HANDLER ( void, indexed_selector );
  NONTERMINAL_HANDLER ( void, constant );

public:

  parser ( std::ifstream &, const char*, symboltbl & );  
  void parse ();

};

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
