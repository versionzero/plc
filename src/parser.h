/*----------------------------------------------------------------------
  File    : parser.h
  Contents: 
  Author  : Ben Burnett
  History : 04.02.2007 file created
----------------------------------------------------------------------*/

#ifndef PARSER_H
#define PARSER_H

#include "error.h"
#include "emitter.h"
#include "scanner.h"
#include "setops.h"
#include "symboltbl.h"
#include "token.h"
#include <exception>
#include <fstream>
#include <vector>

/*----------------------------------------------------------------------
  Forward Declarations
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/

#define NONTERMINAL_PARAMS (token_set const & stop)
#define NONTERMINAL_HANDLER(r,x) r x NONTERMINAL_PARAMS;
#define NONTERMINAL_HANDLER_X(r,x) r x;

/*----------------------------------------------------------------------
  Main Class
----------------------------------------------------------------------*/

class parser {

private:

  emitter_interface &_emitter;    /* error manager */
  error_interface   &_errors;     /* error manager */
  scanner            _scanner;    /* stream scanner (lexer) */
  symboltbl         &_symbols;    /* main symbol table */  
  token              _token;      /* current token */
  token              _null;       /* null token -- for errors n' stuff */
  token_code         _expected;   /* last expected token */
  token_set          _first_symbols[LAST_TOKEN]; /* BNF derived first symbols */
  static int         _next_label; /* counter to ensure unique labels */  

  token_code move ();
  int new_label (); 
  
  void expect (token_code, token_set const&);
  void expect (std::string&, token_set const&);
  token& define (std::string const&, kind::code, type::code = type::universal,
		  int = 0, int = 0, int = 0, int = 0);
  token& find (std::string const&);
    
  void error (error::input::code) const;
  void error (error::input::code, token const&) const;
  void error (error::input::code, token const&, token const&) const;
  
  void syntax_check (token_set const&);
  void syntax_error (token_set const&);
  
  void type_check (type::code&, type::code);

  typedef std::pair<int, type::code> constant_type;
  typedef std::vector<type::code>    expression_vector;
  typedef std::vector<token>         token_vector;  
      
  /* --- a method for each (useful) non-terminal in the PL grammar */
  NONTERMINAL_HANDLER (void, program);
  NONTERMINAL_HANDLER_X (void, block (int, int, token_set const&));
  NONTERMINAL_HANDLER_X (int, definition_part (int&, token_set const&));
  NONTERMINAL_HANDLER (void, constant_definition);
  NONTERMINAL_HANDLER_X (int, variable_definition (int&, 
						     token_set const&));  
  NONTERMINAL_HANDLER (void, procedure_definition);
  NONTERMINAL_HANDLER (void, statement_part);
  NONTERMINAL_HANDLER (token_vector, variable_access_list);
  NONTERMINAL_HANDLER (expression_vector, expression_list);
  NONTERMINAL_HANDLER_X (void, guarded_command_list (int&, int, 
						       token_set const&));
  NONTERMINAL_HANDLER_X (void, guarded_command (int&, int, 
						token_set const&));
  NONTERMINAL_HANDLER (type::code, expression);
  NONTERMINAL_HANDLER (type::code, primary_expression);
  NONTERMINAL_HANDLER (type::code, simple_expression);
  NONTERMINAL_HANDLER (type::code, term);
  NONTERMINAL_HANDLER (void, multiplying_operator);
  NONTERMINAL_HANDLER (type::code, factor);
  NONTERMINAL_HANDLER (token, variable_access);
  NONTERMINAL_HANDLER_X (void, indexed_selector (int, token_set const&));
  NONTERMINAL_HANDLER (constant_type, constant);

public:

  parser (std::ifstream&, symboltbl&, error_interface&, 
	   emitter_interface&);  
  void parse ();

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
