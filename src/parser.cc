/*----------------------------------------------------------------------
  File    : parser.cc
  Contents: 
  Author  : Ben Burnett
  History : 04.02.2007 file created
----------------------------------------------------------------------*/

#if HAVE_CONFIG_H
#include <config.h>
#endif 

#include "parser.h"
#include "error.h"
#include <iostream>
#include <string>

/*----------------------------------------------------------------------
  Namespace Inclusions
----------------------------------------------------------------------*/

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::pair;
using std::string;
using std::vector;

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/

/* -- quick macro to speed up the process of creating stop sets:
   
   SYMBOLS ( COMMA, END, PLUS ) 

   Will return an STL set containing those token ids.  These procedures will 
   also eliminate duplicates as well, thus:

   SYMBOLS ( COMMA, END, PLUS, PLUS ) = { COMMA, END, PLUS }
   
 */
#define SYMBOLS(...) make_set(__VA_ARGS__)

/* -- the following is only for syntactic sugar, it returns the set of
   first symbols for a particular BNF rule */
#define FIRST(x) _first_symbols[x]

/* --- when debugging it may be useful to know where we are situated 
   in the parse tree (use --enable-debug=true with the configure script
   to enable this). */
#ifdef NDEBUG
#define BEGIN_NONTERMINAL_HANDLER(r,x)		\
  r parser::x NONTERMINAL_PARAMS {
#define END_NONTERMINAL_HANDLER }
#define PREMATURE_END_NONTERMINAL_HANDLER
#deinf  DEBUG_OUTPUT(x)
#else
static int __indent = 0;
#define BEGIN_NONTERMINAL_HANDLER(r,x)				\
  r parser::x NONTERMINAL_PARAMS {				\
    char const *__fn = #x;					\
    for ( int __i = 0; __i < __indent; ++__i ) {		\
      cout << ' '; }						\
    cout << "<" << __fn  << ">" << endl;			\
    __indent++;
#define END_NONTERMINAL_HANDLER				\
  __indent--;						\
  for ( int __i = 0; __i < __indent; ++__i ) {		\
    cout << ' '; }					\
  cout << "</" << __fn << ">" << endl; }		
#define PREMATURE_END_NONTERMINAL_HANDLER {END_NONTERMINAL_HANDLER
#define DEBUG_OUTPUT(x)						\
  for ( int __i = 0; __i < __indent; ++__i ) {			\
    cout << ' '; }						\
  cout << "<" << x << " />\n";
#endif

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/

/* --- BNF rule names (for first set indexing) */
enum {
  PROGRAM, BLOCK, DEFINITION_PART, DEFINITION, CONSTANT_DEFINITION,
  VARIABLE_DEFINITION1, VARIABLE_DEFINITION2, TYPE_SYMBOL, VARIABLE_LIST,
  PROCEDURE_DEFINITION, STATEMENT_PART, STATEMENT, EMPTY_STATEMENT,
  READ_STATEMENT, VARIABLE_ACCESS_LIST, WRITE_STATEMENT, EXPRESSION_LIST,
  ASSIGNMENT_STATEMENT, PROCEDURE_STATEMENT, IF_STATEMENT, DO_STATEMENT,
  GUARDED_COMMAND_LIST, GUARDED_COMMAND, EXPRESSION, PRIMARY_OPERATOR,
  PRIMARY_EXPRESSION, RELATIONAL_OPERATOR, SIMPLE_EXPRESSION, ADDING_OPERATOR,
  TERM, MULTIPLYING_OPERATOR, FACTOR, VARIABLE_ACCESS, INDEXED_SELECTOR,
  CONSTANT, NUMERAL, BOOLEAN_SYMBOL, NAME, LAST_BNF_RULE
};

/* --- first sets (8 is just a magic number, it's the currently longest
   set of first symbols) */
static const int first_symbols[LAST_TOKEN][9] = {
  /* PROGRAM */              { BEGIN, NONE }, 
  /* BLOCK */                { BEGIN, NONE }, 
  /* DEFINITION_PART */      { BOOLEAN, CONST, INTEGER, PROC, NONE }, 
  /* DEFINITION */           { BOOLEAN, CONST, INTEGER, PROC, NONE }, 
  /* CONSTANT_DEFINITION */  { CONST, NONE }, 
  /* VARIABLE_DEFINITION1 */ { BOOLEAN, INTEGER, NONE }, 
  /* VARIABLE_DEFINITION2 */ { IDENTIFIER, ARRAY, NONE }, 
  /* TYPE_SYMBOL */          { BOOLEAN, INTEGER, NONE }, 
  /* VARIABLE_LIST */        { IDENTIFIER, WORD, NONE }, 
  /* -- added WORD to VARIABLE_LIST because on the first declaration a token
     `bla' is a WORD rather than an IDENTIFIER.  This may be ad-hoc, but it
     seems to work right now. */
  /* PROCEDURE_DEFINITION */ { PROC, NONE }, 
  /* STATEMENT_PART */       { SKIP, READ, WRITE, CALL, IF, DO, 
			       IDENTIFIER, WORD, NONE },   
  /* -- assignemnt statements start with a WORD/ID */
  /* STATEMENT */            { SKIP, READ, WRITE, CALL, IF, DO, 
			       IDENTIFIER, NONE }, 
  /* EMPTY_STATEMENT */      { SKIP, NONE }, 
  /* READ_STATEMENT */       { READ, NONE }, 
  /* VARIABLE_ACCESS_LIST */ { IDENTIFIER, NONE }, 
  /* WRITE_STATEMENT */      { WRITE, NONE }, 
  /* EXPRESSION_LIST */      { FALSE, TRUE, IDENTIFIER, LEFT_PAREN, 
			       LOGICAL_NOT, MINUS, NUMBER, NONE }, 
  /* ASSIGNMENT_STATEMENT */ { IDENTIFIER, NONE }, 
  /* PROCEDURE_STATEMENT */  { CALL, NONE }, 
  /* IF_STATEMENT */         { IF, NONE }, 
  /* DO_STATEMENT */         { DO, NONE }, 
  /* GUARDED_COMMAND_LIST */ { FALSE, TRUE, IDENTIFIER, LEFT_PAREN, 
			       LOGICAL_NOT, MINUS, NUMBER, NONE }, 
  /* GUARDED_COMMAND */      { FALSE, TRUE, IDENTIFIER, LEFT_PAREN, 
			       LOGICAL_NOT, MINUS, NUMBER, NONE }, 
  /* EXPRESSION */           { FALSE, TRUE, IDENTIFIER, LEFT_PAREN, 
			       LOGICAL_NOT, MINUS, NUMBER, NONE },
  /* PRIMARY_OPERATOR */     { LOGICAL_OR, LOGICAL_AND, NONE }, 
  /* PRIMARY_EXPRESSION */   { FALSE, TRUE, IDENTIFIER, LEFT_PAREN, 
			       LOGICAL_NOT, MINUS, NUMBER, NONE }, 
  /* RELATIONAL_OPERATOR */  { EQUAL, GREATER_THAN, LESS_THAN, NONE }, 
  /* SIMPLE_EXPRESSION */    { FALSE, TRUE, IDENTIFIER, WORD, LEFT_PAREN, 
			       LOGICAL_NOT, MINUS, NUMBER, NONE }, 
  /* ADDING_OPERATOR */      { PLUS, MINUS, NONE }, 
  /* TERM */                 { FALSE, TRUE, IDENTIFIER, LEFT_PAREN, 
			       LOGICAL_NOT, NUMBER, NONE }, 
  /* MULTIPLYING_OPERATOR */ { MULTIPLY, DIVIDE, MODULO, NONE },
  /* FACTOR */               { FALSE, TRUE, IDENTIFIER, LEFT_PAREN, 
			       LOGICAL_NOT, NUMBER, NONE }, 
  /* VARIABLE_ACCESS */      { IDENTIFIER, NONE }, 
  /* INDEXED_SELECTOR */     { LEFT_BRACKET, NONE }, 
  /* CONSTANT */             { FALSE, TRUE, IDENTIFIER, WORD, NUMBER, NONE }, 
  /* -- added WORD to CONSTANT becuase it may be the case the we spell the
     name of the r-value wrong; in which case we do not want the less
     helpful "unexpected symbol `bla'" rather, we would like the more
     informative "undefined symbol `bla'" ... but maybe that's just me. */
  /* NUMERAL */              { NUMBER, NONE }, 
  /* BOOLEAN_SYMBOL */       { FALSE, TRUE, NONE }, 
  /* NAME */                 { IDENTIFIER, NONE }
};

/*----------------------------------------------------------------------
  Friend Functions/Operators
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Exceptions
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Main Methods
----------------------------------------------------------------------*/

parser::parser ( ifstream & s, const char *p, symboltbl & t )
  : _scanner ( s ), _fn_in ( p ), _symbols ( t ), _expected ( NONE )  {
  unsigned int i, j;
  /* Create the "first symbols" sets.  At this point using STL sets is a
     little excessive, as there will never be duplicates; however, as we 
     descend through the parse tree and use these to build follow sets 
     and stop sets, the fact that sets forbid duplicates will become more 
     important. */
  for ( i = 0; i < LAST_TOKEN; ++i ) {
    for ( j = 0; NONE != first_symbols[i][j] && j < 9; ++j ) {
      _first_symbols[i].insert ( 
	  static_cast<token_code> ( first_symbols[i][j] ) );
    }
  }
}

/* --------------------------------------------------------------------*/

#if 0
token_code parser::move () {
  return ( _token = _scanner.next_token () );
}

token_code parser::move () {
  _token = _scanner.next_token ();  
  /* --- if we find a word, then determine if it is a keyword or otherwise */
  if ( WORD == _token ) {
    string s = _token.value ();
    symboltbl::iterator it = _symbols.find ( s );
    if ( it == _symbols.end () ) {      /* if not in symbol table, then */
      _token = token ( IDENTIFIER, s ); /* it's an identifier, so create a */
      _symbols.insert ( s, _token );    /* new entry in the symbol table */
    } else {
      _token = it->second;
    }
  }
  return _token;
}
#endif

token_code parser::move () {
  _token = _scanner.next_token ();  
  /* --- if we find a word, then determine if it is a keyword (i.e.
   *bottom* of the symbol table); other identifiers will be taken care
   of later ... */
  if ( WORD == _token ) {
    symboltbl::iterator it = _symbols.find_bottom ( _token.value () );
    if ( it != _symbols.end () ) {
      _token = it->second;
    }
  }
  return _token;
}

/* --------------------------------------------------------------------*/

#if 0
void parser::expect ( token_code c, token_set const & stop ) {
  _expected = c;                /* -- set which symbol we expect */  
  if ( _expected == _token ) {  /* ensure that we have received it */  
    move ();                    /* and move on */
  } else {                      /* -- on failure, signal a syntax */
    syntax_error ( stop ); }    /* error to the user */
  _expected = NONE;             /* -- reset the expected symbol and */
  syntax_check ( stop );        /* ensure that we are in a sane state */
}
#endif

void parser::expect ( token_code c, token_set const & stop ) {
  _expected = c;                /* -- set which symbol we expect */

  
  /*cout << "1. EXPECTING (" << token::friendly_name ( c ) << ") : got = " 
       << token::friendly_name ( _token ) 
       << " (" << _token << ") : "
       << _token.value () << "\n";
  */

  if ( WORD == _token ) {
    symboltbl::iterator it = _symbols.find ( _token.value () );
    if ( it != _symbols.end () ) {
      _token = it->second;
    }
  }

  
  /*cout << "2. EXPECTING (" << token::friendly_name ( c ) << ") : got = " 
       << token::friendly_name ( _token ) 
       << " (" << _token << ") : "
       << _token.value () << "\n";
  */

  if ( _expected == _token ) {  /* ensure that we have received it */  
    move ();                    /* and move on */
  } else {                      /* -- on failure, signal a syntax */
    syntax_error ( stop ); }    /* error to the user */
  _expected = NONE;             /* -- reset the expected symbol and */
  syntax_check ( stop );        /* ensure that we are in a sane state */
}

/* --------------------------------------------------------------------*/

void parser::expect_impl ( string & name, token_set const & stop ) {
  name = "";                    /* set to "no-name" */
  if ( _expected == _token ) {  /* ensure that we have received one, */  
    name = _token.value ();     /* store the id's name for later, */
    move ();                    /* and move on */
  } else {                      /* -- on failure, signal a syntax */
    syntax_error ( stop ); }    /* error to the user */
  _expected = NONE;             /* -- reset the expected symbol and */
  syntax_check ( stop );        /* ensure that we are in a sane state */  
}

/* --------------------------------------------------------------------*/

void parser::expect_word ( string & name, token_set const & stop ) {  
  _expected = WORD;             /* -- we are looking for word */
  expect_impl ( name, stop );
}

/* --------------------------------------------------------------------*/

void parser::expect_identifier ( string & name, token_set const & stop ) {  
  _expected = IDENTIFIER;       /* -- we are looking for ids */
  if ( WORD == _token ) {       /* -- find the id */
    symboltbl::iterator it = _symbols.find ( _token.value () );
    if ( it != _symbols.end () ) {
      _token = it->second;
    }
  }
  expect_impl ( name, stop );
}

/* --------------------------------------------------------------------*/

void parser::define ( std::string const & name, kind::code k, token & t ) {
  if ( name.empty () ) { return; }

  /*cout << "DEFINE: " << name << " : " << t 
       << " (value: " << _token.value () << " ) ... ";
  cout << "SEARCH: >" << name << "<\n";
  
  symboltbl::iterator itt = _symbols.find ( name );
  if ( itt == _symbols.end () ) {
    cout << "NOT ";
  }
  cout << "found!\n";
  */

  /* -- we are only concerned if the ID is defined withing the current
     scope (i.e. the *top*): if one exists, then a new one should *not* be
     allowed */
  symboltbl::iterator it = _symbols.find_top ( name );
  if ( it != _symbols.end () ) {
    token tmp = _token;
    _token = it->second;
    error ( E_DUP );
    _token = tmp;
    t = it->second;    
  } else {
    pair<symboltbl::iterator, bool> p = _symbols.insert ( name, 
      token ( IDENTIFIER, k, type::universal, name ) );
    if ( p.second ) { t = p.first->second; }
  }
}

/* --------------------------------------------------------------------*/

void parser::syntax_check ( token_set const & stop ) {
  if ( !stop.count ( _token ) ) {
    syntax_error ( stop );
  }
}

/* --------------------------------------------------------------------*/

void parser::type_check ( type::code & t1, type::code const & t2 ) {
  if ( t1 != t2 ) {
    if ( t1 && t2 ) { /* type::univeral */
      error ( E_TYPE );
    }
    t1 = type::universal;
  }
}

/* --------------------------------------------------------------------*/

void parser::error ( error_code err ) const {
  cerr << _fn_in << ":" <<  _scanner.line () << ": error: ";
  switch ( err ) {    
  case E_SYNTAX:                /* -- if there is a syntax error and */
    if ( UNKNOWN == _token ) {  /* we don't recognize the symbol: */
      cerr << "unrecognized symbol `" << _token.value () << "'\n";
    } else {                    /* if we do recognize it ... */
      if ( NONE == _expected ) {/* a) but we are not expecting it: */
	cerr << "unexpected symbol `" << _token.value () << "'\n";
      } else {                  /* b ) but were expecting something else: */
	cerr << "expected `" << token::friendly_name ( _expected )
	     << "' before `" << _token.value () << "' token\n";
      }
    } break;  
  case E_SYMBOL:
    cerr << "undefined symbol `" << _token.value () << "'\n";
    break;
  case E_CONST:                 /* non-constant used in a constant expression */
    cerr << "non-constant `" << _token.value ()
	 << "' used in a constant expression\n";
    break;
  case E_TYPE:                  /* type error */
    cerr << "type error\n";
    break;
  case E_DUP:
    cerr << "duplicate symbol `" << _token.value () << "'\n";
    break;
  case E_PROC:                 /* not a procedure */
    cerr << "`" << _token.value () << "' is not a procedure\n";
    break;
  case E_BOOLEAN:
    cerr << "boolean expression expected\n";
    break;
  default:
    cerr << "unknown error\n";
    break;
  }  
}

/* --------------------------------------------------------------------*/

void parser::syntax_error ( token_set const & stop ) {
  error ( E_SYNTAX );           /* issue an error message */
  while ( !stop.count ( _token ) ) { 
    move ();                    /* -- find the next recognized symbol */
  }
}

/* --------------------------------------------------------------------*/

void parser::kind_error ( error_code c ) {
  error ( c );
}

/* --------------------------------------------------------------------*/

/* Program = Block "." . */
BEGIN_NONTERMINAL_HANDLER ( void, program ) {  
  block ( SYMBOLS ( PERIOD ) + stop );
  expect ( PERIOD, stop );
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* Block = "begin" DefinitionPart StatementPart "end" .  */  
BEGIN_NONTERMINAL_HANDLER ( void, block )  {  
  expect ( BEGIN, FIRST ( DEFINITION_PART ) + FIRST ( STATEMENT_PART ) 
	   + SYMBOLS ( END ) + stop );
  _symbols.push ();             /* start a new scope */
  definition_part ( FIRST ( STATEMENT_PART ) + SYMBOLS ( END ) + stop );
  statement_part ( SYMBOLS ( END ) + stop );
  _symbols.pop ();              /* end the scope */  
  expect ( END, stop );
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* DefinitionPart = { Definition ";" } . */
/* Definition = ConstantDefinition | VariableDefinition	\
   | ProcedureDefinition . */
BEGIN_NONTERMINAL_HANDLER ( void, definition_part )  {
  syntax_check ( FIRST ( DEFINITION_PART ) + stop );
  token_set extra = SYMBOLS ( SEMICOLON ) + stop;
  while ( _token >= BOOLEAN && _token <= PROC ) {
    switch ( _token ) {
    case CONST:    
      constant_definition ( extra );
      break;
    case BOOLEAN:
    case INTEGER:
      variable_definition ( extra );
      break;
    case PROC:
      procedure_definition ( extra );
      break;
    default:   
      /* for empty definitions -- also quiets the enumeration value 'bla'
	 not handled in switch warnings */
      return;
    }
    expect ( SEMICOLON, FIRST ( DEFINITION_PART ) + stop );
    syntax_check ( FIRST ( DEFINITION_PART ) + stop );
  }  
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* ConstantDefinition = "const" ConstantName "=" Constant . */
#if 0
BEGIN_NONTERMINAL_HANDLER ( void, constant_definition )  {  
  expect ( CONST, SYMBOLS ( WORD ) + SYMBOLS ( EQUAL ) 
	   + FIRST ( CONSTANT ) + stop );
  /* here we have to insert the WORD token into the symbol table if
     it is not already there.  if it is there, then this is an error */
  symboltbl::iterator it = _symbols.find ( _token.value () );
  if ( it == _symbols.end () ) {        /* if not in symbol table, then */
    _token = token ( IDENTIFIER, _token.value () ); /* it's an identifier, */
    _token.set_kind ( kind::constant ); /* so create a new entry in the */  
    _symbols.insert ( _token.value (), _token ); /* symbol table */
  } else {
    error ( E_DUP );
  }  
  expect ( IDENTIFIER, SYMBOLS ( EQUAL ) + FIRST ( CONSTANT ) + stop ); 
  expect ( EQUAL, FIRST ( CONSTANT ) + stop );
  ast::constant c = constant ( stop );
  if ( it == _symbols.end () ) {      
    it->second.set_type ( c.type () );    
  } /* else {
    it->second.set_type ( type::universal );
    } */
} END_NONTERMINAL_HANDLER;
#endif

BEGIN_NONTERMINAL_HANDLER ( void, constant_definition )  {    
  string name; token tok; 
  expect ( CONST, SYMBOLS ( WORD ) + SYMBOLS ( EQUAL ) 
	   + FIRST ( CONSTANT ) + stop );    
  expect_word ( name, SYMBOLS ( EQUAL ) + FIRST ( CONSTANT ) + stop );
  expect ( EQUAL, FIRST ( CONSTANT ) + stop );  
  type::code t = constant ( stop );  
  define ( name, kind::constant, tok );
  tok.set_type ( t );
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* VariableDefinition = TypeSymbol VariableList 
   | TypeSymbol "array" VariableList "[" Constant "]" . */
/* TypeSymbol = type "integer" | "Boolean" . */
BEGIN_NONTERMINAL_HANDLER ( void, variable_definition )  {  
  bool array; string name; token t; 
  kind::code vkind; type::code vtype;
  /* TypeSymbol - type::code values based on token_code values */
  vtype = ( BOOLEAN == _token ? type::boolean : type::integer );
  expect ( static_cast<token_code> ( vtype ), 
	   SYMBOLS ( ARRAY ) + FIRST ( VARIABLE_LIST ) + stop ); 
  /* "array" VariableList ... */    
  vkind = kind::variable;
  if ( array = ( ARRAY == _token ) ) {
    vkind = kind::array;
    expect ( ARRAY, FIRST ( VARIABLE_LIST ) 
	     + SYMBOLS ( LEFT_BRACKET, RIGHT_BRACKET ) 
	     + FIRST ( CONSTANT ) + stop );
  }  
  /* VariableList = VariableName { "," VariableName } . */
  expect_word ( name, FIRST ( VARIABLE_LIST ) 
		+ SYMBOLS ( COMMA, LEFT_BRACKET, RIGHT_BRACKET ) 
		+ FIRST ( CONSTANT ) + stop );
  define ( name, kind::variable, t );
  t.set_type ( vtype );
  while ( COMMA == _token ) {
    expect ( COMMA, SYMBOLS ( WORD ) + FIRST ( CONSTANT ) 
	     + SYMBOLS ( LEFT_BRACKET, RIGHT_BRACKET ) + stop );
    expect_word ( name, FIRST ( VARIABLE_LIST ) + FIRST ( CONSTANT ) 
		  + SYMBOLS ( COMMA, LEFT_BRACKET, RIGHT_BRACKET ) + stop );
    define ( name, kind::variable, t );
    t.set_type ( vtype );
  }
  /* if this is an array we will find: ... "[" Constant "]" . */
  if ( array ) {
    expect ( LEFT_BRACKET, FIRST ( CONSTANT ) 
	     + SYMBOLS ( RIGHT_BRACKET ) + stop );
    constant ( SYMBOLS ( RIGHT_BRACKET ) + stop );
    expect ( RIGHT_BRACKET, stop );
  }
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* ProcedureDefinition = "proc" ProcedureName Block . */
BEGIN_NONTERMINAL_HANDLER ( void, procedure_definition )  {
  string name; token t;
  expect ( PROC, SYMBOLS ( WORD ) + FIRST ( BLOCK ) + stop );
  expect_word ( name, FIRST ( BLOCK ) + stop );
  define ( name, kind::procedure, t );
  block ( stop );
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* StatementPart = { Statement ";" } . */
/* Statement = EmptyStatement | ReadStatement | WriteStatement
   | AssignmentStatement | ProcedureStatement | IfStatement
   | DoStatement . */
/* EmptyStatement = "skip" . */
/* ReadStatement = "read" VariableAccessList . */
/* WriteStatement = "write" ExpressionList . */
/* AssignmentStatement = VariableAccessList ":=" ExpressionList . */
/* ProcedureStatement = "call" ProcedureName . */
/* IfStatement = "if" GuardedCommandList "fi" . */
/* DoStatement =  "do" GuardedCommandList "od" . */
BEGIN_NONTERMINAL_HANDLER ( void, statement_part )  {
  parser::var_vector  vars;  parser::var_vector::iterator  jt;
  parser::expr_vector exprs; parser::expr_vector::iterator kt;
  symboltbl::iterator it;
  syntax_check ( FIRST ( STATEMENT_PART ) + stop );  
  token_set extra = SYMBOLS ( SEMICOLON ) + stop;
  while ( ( _token >= SKIP && _token <= DO ) 
	  || IDENTIFIER == _token ) {
    switch ( _token ) {
    case SKIP: 
      DEBUG_OUTPUT ( "skip" );
      /* EmptyStatement = "skip" . */
      expect ( SKIP, extra );
      break;    
    case READ:
      DEBUG_OUTPUT ( "read" );
      /* ReadStatement = "read" VariableAccessList . */
      expect ( READ, FIRST ( VARIABLE_ACCESS_LIST ) 
	       + SYMBOLS ( WORD ) + extra );
      variable_access_list ( extra );
      break;
    case WRITE:
      DEBUG_OUTPUT ( "write" );
      /* WriteStatement = "write" ExpressionList . */
      expect ( WRITE, FIRST ( EXPRESSION_LIST ) + SYMBOLS ( WORD ) + extra );
      expression_list ( extra );
      break;
    case CALL:
      DEBUG_OUTPUT ( "call" );
      /* ProcedureStatement = "call" ProcedureName . */
      /* -- even though we do not support WORD as procedure names, we must 
	 use them here because IDENTIFIERs won't be transformed from WORDS
	 until during the second expect call (of course it will be looked
	 up explicitly in the symbol table so it won't actually accept WORDs
	 anyway). */
      expect ( CALL, SYMBOLS ( IDENTIFIER ) + SYMBOLS ( WORD ) + extra );
      it = _symbols.find ( _token.value () );
      if ( it != _symbols.end () ) { /* if there is no symbol, then we have */
	if ( it->second.kind () != kind::procedure ) {   /* bigger problems */
	  kind_error ( E_PROC );
	}
      } else {
	error ( E_SYMBOL );
      }
      expect ( IDENTIFIER, extra );
      break;
    case IF:
      DEBUG_OUTPUT ( "if" );
      /* IfStatement = "if" GuardedCommandList "fi" . */
      expect ( IF, FIRST ( GUARDED_COMMAND_LIST ) + SYMBOLS ( FI ) + extra );
      guarded_command_list ( SYMBOLS ( FI ) + extra );
      expect ( FI, extra );
      break;
    case DO:
      DEBUG_OUTPUT ( "do" );
      /* DoStatement = "do" GuardedCommandList "od" . */
      expect ( DO, FIRST ( GUARDED_COMMAND_LIST ) + SYMBOLS ( OD ) + extra );
      guarded_command_list ( SYMBOLS ( OD ) + extra );
      expect ( OD, extra );
      break;
      // case WORD:
    case IDENTIFIER: /* ASSIGN */
      DEBUG_OUTPUT ( "assign (:=)" );
      /* AssignmentStatement = VariableAccessList ":=" ExpressionList . */    
      vars = variable_access_list ( SYMBOLS ( ASSIGN ) 
				    + FIRST ( EXPRESSION_LIST )
				    + extra );
      expect ( ASSIGN, FIRST ( EXPRESSION_LIST ) + extra );
      exprs = expression_list ( extra );      
      if ( vars.size () == exprs.size () ) {
	for ( jt = vars.begin (), kt = exprs.begin (); 
	      jt != vars.end (); ++jt, ++kt ) {
	  type_check ( jt->second, *kt );	  
	}
      } else {
	cerr << "mismatched lhs and rhs count\n";
      }
      break;
    default:
      /* ERROR! - can't get here */
      assert ( 0 );
      break;
    }  
    expect ( SEMICOLON, FIRST ( STATEMENT_PART ) + stop );    
  }  
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* VariableAccessList = VariableAccess { "," VariableAccess } . */
BEGIN_NONTERMINAL_HANDLER ( parser::var_vector, variable_access_list )  {  
  parser::var_vector vars;
  vars.push_back ( variable_access ( SYMBOLS ( COMMA ) + stop ) );
  while ( COMMA == _token ) {
    expect ( COMMA, FIRST ( VARIABLE_ACCESS ) + SYMBOLS ( WORD ) + stop );
    vars.push_back ( variable_access ( SYMBOLS ( COMMA ) + stop ) );
  }
  PREMATURE_END_NONTERMINAL_HANDLER;
  return vars;
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* VariableAccess = VariableName [ IndexedSelector ] . */
BEGIN_NONTERMINAL_HANDLER ( parser::var_type, variable_access )  {
  /* expect ( IDENTIFIER, FIRST ( INDEXED_SELECTOR ) + stop ); */
  string name; 
  expect_identifier ( name, FIRST ( INDEXED_SELECTOR ) + stop );
  symboltbl::iterator it = _symbols.find ( _token.value () );
  type::code t = type::universal;
  if ( it != _symbols.end () ) {
    t = it->second.type (); 
  }
  /* IndexedSelector = "[" Expression "]" . */
  if ( LEFT_BRACKET == _token ) {
    indexed_selector ( stop );
  }
  PREMATURE_END_NONTERMINAL_HANDLER;
  return make_pair ( name, t );
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* ExpressionList = Expression { "," Expression } . */
BEGIN_NONTERMINAL_HANDLER ( parser::expr_vector, expression_list ) {
  parser::expr_vector exprs;
  exprs.push_back ( expression ( SYMBOLS ( COMMA ) + stop ) );
  while ( COMMA == _token ) {
    expect ( COMMA, FIRST ( EXPRESSION ) + stop );
    exprs.push_back ( expression ( stop ) );
  }
  PREMATURE_END_NONTERMINAL_HANDLER;
  return exprs;
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* GuardedCommandList = GuardedCommand { "[]" GuardedCommand } . */
BEGIN_NONTERMINAL_HANDLER ( void, guarded_command_list )  {
  guarded_command ( SYMBOLS ( GUARD_SEPARATOR ) + stop );
  while ( GUARD_SEPARATOR == _token ) {
    expect ( GUARD_SEPARATOR, FIRST ( GUARDED_COMMAND ) + stop );
    guarded_command ( stop );
  }
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* GuardedCommand = Expression "->" StatementPart . */
BEGIN_NONTERMINAL_HANDLER ( void, guarded_command )  {
  type::code t = expression ( SYMBOLS ( GUARD_POINT ) 
			      + FIRST ( STATEMENT_PART ) + stop );
  if ( type::boolean != t ) {
    error ( E_BOOLEAN );
  }
  expect ( GUARD_POINT, FIRST ( STATEMENT_PART ) + stop );
  statement_part ( stop );
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* Expression = PrimaryExpression { PrimaryOperator PrimaryExpression } . */
/* PrimaryOperator = "&" | "|" . */
BEGIN_NONTERMINAL_HANDLER ( type::code, expression )  {
  type::code t1, t2;
  t1 = primary_expression ( FIRST ( PRIMARY_OPERATOR ) 
			    + FIRST ( PRIMARY_EXPRESSION ) + stop );
  /* PrimaryOperator (= "&" | "|" .) */  
  syntax_check ( FIRST ( PRIMARY_OPERATOR ) 
		 + FIRST ( PRIMARY_EXPRESSION ) + stop ); 
  while ( LOGICAL_AND == _token || LOGICAL_OR == _token ) {
    expect ( LOGICAL_AND == _token ? LOGICAL_AND : LOGICAL_OR, 
	     FIRST ( PRIMARY_EXPRESSION ) + stop );
    t2 = primary_expression ( FIRST ( PRIMARY_OPERATOR ) 
			      + FIRST ( PRIMARY_EXPRESSION ) + stop );     
    type_check ( t1, t2 );
  }
  PREMATURE_END_NONTERMINAL_HANDLER;
  return t1;
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* PrimaryExpression = SimpleExpression [ RelationalOperator	\
   SimpleExpression ] . */
/* RelationalOperator = "<" | "=" | ">" . */
BEGIN_NONTERMINAL_HANDLER ( type::code, primary_expression )  {
  type::code t1, t2;
  t1 = simple_expression ( FIRST ( RELATIONAL_OPERATOR ) 
			   + FIRST ( SIMPLE_EXPRESSION ) + stop );
  /* RelationalOperator (= "<" | "=" | ">" .) */
  syntax_check ( FIRST ( RELATIONAL_OPERATOR ) 
		 + FIRST ( SIMPLE_EXPRESSION ) + stop );
  while ( _token >= EQUAL && _token <= LESS_THAN ) {
    switch ( _token ) {
    case EQUAL:
      expect ( EQUAL, FIRST ( SIMPLE_EXPRESSION ) + stop );
      break;
    case GREATER_THAN:
      expect ( GREATER_THAN, FIRST ( SIMPLE_EXPRESSION ) + stop );
      break;
    case LESS_THAN:
      expect ( LESS_THAN, FIRST ( SIMPLE_EXPRESSION ) + stop );
      break;
    default:
      /* ERROR! - can't get here */
      assert ( 0 );
      break;
    }
    t2 = simple_expression ( FIRST ( RELATIONAL_OPERATOR ) 
			     + FIRST ( SIMPLE_EXPRESSION ) + stop );    
    type_check ( t1, t2 );
  }  
  PREMATURE_END_NONTERMINAL_HANDLER;
  return t1;
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* SimpleExpression = [ "-" ] Term { A
ddingOperator Term } . */
BEGIN_NONTERMINAL_HANDLER ( type::code, simple_expression )  {  
  type::code t1, t2;
  /* [ "-" ] Term */
  bool negative = false; 
  syntax_check ( SYMBOLS ( MINUS ) + stop );
  if ( MINUS == _token ) { 
    negative = true;
    expect ( MINUS, FIRST ( ADDING_OPERATOR ) + FIRST ( TERM ) + stop );
  }
  t1 = term ( FIRST ( ADDING_OPERATOR ) + FIRST ( TERM ) + stop );
  if ( negative ) { 
    type_check ( t1, type::integer ); 
  }
  /* { AddingOperator (= "+" | "-" .) Term } */
  syntax_check ( FIRST ( ADDING_OPERATOR ) + FIRST ( TERM ) + stop );
  while ( PLUS == _token || MINUS == _token ) {
    expect ( PLUS == _token ? PLUS : MINUS, FIRST ( TERM ) + stop );
    t2 = term ( FIRST ( ADDING_OPERATOR ) + FIRST ( TERM ) + stop );    
    type_check ( t1, t2 ); 
  }
  PREMATURE_END_NONTERMINAL_HANDLER;
  return t1;
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* Term = Factor { MultiplyingOperator Factor } . */
#if 0
BEGIN_NONTERMINAL_HANDLER ( type::code, term )  {
  type::code t1, t2;
  t1 = factor ( FIRST ( MULTIPLYING_OPERATOR ) + FIRST ( FACTOR ) + stop );
  /* MultiplyingOperator (= "*" | "/" | "\" .) Factor */
  while ( _token >= MULTIPLY && _token <= MODULO ) {
    switch ( _token ) {
    case MULTIPLY:
      expect ( MULTIPLY, FIRST ( FACTOR ) + stop );    
      break;
    case DIVIDE:
      expect ( DIVIDE, FIRST ( FACTOR ) + stop );    
      break;
    case MODULO:
      expect ( MODULO, FIRST ( FACTOR ) + stop );    
      break;
    default:
      /* ERROR! - can't get here */
      assert ( 0 );
      break;
    }
    syntax_check ( FIRST ( MULTIPLYING_OPERATOR ) + FIRST ( FACTOR ) + stop );
    t2 = factor ( FIRST ( MULTIPLYING_OPERATOR ) + FIRST ( FACTOR ) + stop );
    type_check ( t1, t2 );
  }
  PREMATURE_END_NONTERMINAL_HANDLER;
  return t1;
} END_NONTERMINAL_HANDLER;
#endif

BEGIN_NONTERMINAL_HANDLER ( type::code, term )  {
  type::code t1, t2;
  t1 = factor ( FIRST ( MULTIPLYING_OPERATOR ) + FIRST ( FACTOR ) + stop );
  /* MultiplyingOperator (= "*" | "/" | "\" .) Factor */
  while ( _token >= MULTIPLY && _token <= MODULO ) {
    expect ( _token, FIRST ( FACTOR ) + stop );
    syntax_check ( FIRST ( MULTIPLYING_OPERATOR ) + FIRST ( FACTOR ) + stop );
    t2 = factor ( FIRST ( MULTIPLYING_OPERATOR ) + FIRST ( FACTOR ) + stop );
    type_check ( t1, t2 );
  }
  PREMATURE_END_NONTERMINAL_HANDLER;
  return t1;
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* Factor = Constant | VariableAccess | "(" Expression ")" | "~" Factor . */
BEGIN_NONTERMINAL_HANDLER ( type::code, factor )  {
  type::code ret = type::universal;
  switch ( _token ) {    
  case LEFT_PAREN:
    /* "(" Expression ")" */
    expect ( LEFT_PAREN, FIRST ( EXPRESSION ) 
	     + SYMBOLS ( RIGHT_PAREN ) + stop );
    ret = expression ( SYMBOLS ( RIGHT_PAREN ) + stop );
    expect ( RIGHT_PAREN, stop );
    break;
  case LOGICAL_NOT:
    /* "~" Factor */
    expect ( LOGICAL_NOT, FIRST ( FACTOR ) + stop );
    ret = factor ( stop );  
    break;
  default:
    /* Constant | VariableAccess (= VariableName [ IndexedSelector ] .) */
    /* NOTE: this is ambiguous, as a constant can be a variable name too.
       To solve this we take note of the starting token, if it is an 
       identifier, then we will test for indexed selector after we parse
       the next token, then parse the index accessor, if nessesary.

       This works because constant() is equivalent to variable_access() in
       that it matches IDENTIFIERs, but not indexed_selector(); hence, we
       simulate the remainder of variable_access() here.
       
       void variable_access () {
         match ( IDENTIFIER );
         if ( LEFT_BRACKET == _token ) {
           indexed_selector ( stop );
         }  
       }
    */
    
    if ( WORD == _token ) {
      ret = variable_access ( stop ).second;
    } else {
      ret = constant ( stop );
    }
    break;
    
    /* ERROR! - can't get here 
       default:       
         assert ( 0 );
         break;
    */    
  }
  PREMATURE_END_NONTERMINAL_HANDLER;
  return ret;
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* IndexedSelector = "[" Expression "]" . */
BEGIN_NONTERMINAL_HANDLER ( void, indexed_selector )  {
  expect ( LEFT_BRACKET, FIRST ( EXPRESSION ) 
	   + SYMBOLS ( RIGHT_BRACKET ) + stop );
  expression ( SYMBOLS ( RIGHT_BRACKET ) + stop );
  expect ( RIGHT_BRACKET, stop );  
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* Constant = Numeral | BooleanSymbol | ConstantName . */
/* Numeral = Digit { Digit } . */
/* BooleanSymbol = "false" | "true" . */
/* Name = Letter { Letter | Digit | "_" } . */
BEGIN_NONTERMINAL_HANDLER ( type::code, constant )  {
  type::code ret = type::universal;
  symboltbl::iterator it;

  cout << "TOKEN: " << _token.value () << "\n";
  
  switch ( _token ) {    
  case WORD: 
    /* -- actually getting a WORD is an error, but the bellow will 
       handle it because the search for the symbol will fail */
  case IDENTIFIER:    
    /* -- here the type of the constant will be inffered from the type
       of the constant associated with the identifier we are given */
    it = _symbols.find ( _token.value () );
    if ( it != _symbols.end () ) {
      if ( it->second.kind () == kind::constant ) {
	ret = it->second.type ();
      } else {
	kind_error ( E_CONST );
	/* type::universal; */
      }
    } else {
      error ( E_SYMBOL );
      /* type::universal; */
    }
    expect ( IDENTIFIER, stop );     
    break;
  case FALSE:
  case TRUE:
    expect ( FALSE == _token ? FALSE : TRUE, stop );
    ret = type::boolean;
    break;
  case NUMBER:
    expect ( NUMBER, stop );
    ret = type::integer;
    break;
  default:
    /* ERROR! - can't get here */    
    assert ( 0 );
    break;
  }
  PREMATURE_END_NONTERMINAL_HANDLER;
  return ret;
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

void parser::parse () {  
  move ();                      /* boot-strap the parser and get the */
  program ( SYMBOLS ( END_OF_FILE ) ); /* first token, then match the 
					  main block */
}


