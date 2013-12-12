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

/*----------------------------------------------------------------------
  Namespace Inclusions
----------------------------------------------------------------------*/

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;

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
static const int first_symbols[LAST_TOKEN][8] = {
  /* PROGRAM */              { BEGIN, NONE }, 
  /* BLOCK */                { BEGIN, NONE }, 
  /* DEFINITION_PART */      { BOOLEAN, CONST, INTEGER, PROC, NONE }, 
  /* DEFINITION */           { BOOLEAN, CONST, INTEGER, PROC, NONE }, 
  /* CONSTANT_DEFINITION */  { CONST, NONE }, 
  /* VARIABLE_DEFINITION1 */ { BOOLEAN, INTEGER, NONE }, 
  /* VARIABLE_DEFINITION2 */ { IDENTIFIER, ARRAY, NONE }, 
  /* TYPE_SYMBOL */          { BOOLEAN, INTEGER, NONE }, 
  /* VARIABLE_LIST */        { IDENTIFIER, NONE }, 
  /* PROCEDURE_DEFINITION */ { PROC, NONE }, 
  /* STATEMENT_PART */       { SKIP, READ, WRITE, CALL, IF, DO, 
			       IDENTIFIER, NONE }, 
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
  /* SIMPLE_EXPRESSION */    { FALSE, TRUE, IDENTIFIER, LEFT_PAREN, 
			       LOGICAL_NOT, MINUS, NUMBER, NONE }, 
  /* ADDING_OPERATOR */      { PLUS, MINUS, NONE }, 
  /* TERM */                 { FALSE, TRUE, IDENTIFIER, LEFT_PAREN, 
			       LOGICAL_NOT, NUMBER, NONE }, 
  /* MULTIPLYING_OPERATOR */ { MULTIPLY, DIVIDE, MODULO, NONE },
  /* FACTOR */               { FALSE, TRUE, IDENTIFIER, LEFT_PAREN, 
			       LOGICAL_NOT, NUMBER, NONE }, 
  /* VARIABLE_ACCESS */      { IDENTIFIER, NONE }, 
  /* INDEXED_SELECTOR */     { LEFT_BRACKET, NONE }, 
  /* CONSTANT */             { FALSE, TRUE, IDENTIFIER, NUMBER, NONE }, 
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
  : _scanner ( s, t ), _fn_in ( p ), _symbols ( t ), _expected ( NONE )  {
  unsigned int i, j;
  /* Create the "first symbols" sets.  At this point using STL sets is a
     little excessive, as there will never be duplicates; however, as we 
     descend through the parse tree and use these to build follow sets 
     and stop sets, the fact that sets forbid duplicates will become more 
     important. */
  for ( i = 0; i < LAST_TOKEN; ++i ) {
    for ( j = 0; NONE != first_symbols[i][j] && j < 8; ++j ) {
      _first_symbols[i].insert ( 
	  static_cast<token_code> ( first_symbols[i][j] ) );
    }
  }
}

/* --------------------------------------------------------------------*/

token_code parser::move () {
  return ( _token = _scanner.next_token () );
}

/* --------------------------------------------------------------------*/

void parser::expect ( token_code c, token_set const & stop ) {
  _expected = c;                /* -- set which symbol we expect */
  if ( _expected == _token ) {  /* ensure that we see it and */  
    move ();                    /* move on */
  } else {                      /* -- on failure, signal a syntax */
    syntax_error ( stop ); }    /* error to the user */
  _expected = NONE;             /* -- reset the expected symbol and */
  syntax_check ( stop );        /* ensure that we are in a sane state */
}

/* --------------------------------------------------------------------*/

void parser::syntax_check ( token_set const & stop ) {
  if ( !stop.count ( _token ) ) {
    syntax_error ( stop );
  }
}

/* --------------------------------------------------------------------*/

void parser::error ( error_code err ) const {
  cerr << _fn_in << ":" <<  _scanner.line () << ": ";
  switch ( err ) {    
  case E_SYNTAX:                /* -- if there is a syntax error and */
    if ( UNKNOWN == _token ) {  /* we don't recognize the symbol: */
      cerr << "error: unrecognized symbol `" << _token.value () << "'\n";
    } else {                   /* if we do recognize it ... */
      if ( NONE == _expected ) { /* a) but we are not expecting it: */
	cerr << "error: unexpected symbol `" << _token.value () << "'\n";
      } else {                 /* b ) but were expecting something else: */
	cerr << "error: expected `" << token::friendly_name ( _expected )
	     << "' before `" << _token.value () << "' token\n";
      }
    } break;
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
  /* start a new scope here
   */
  definition_part ( FIRST ( STATEMENT_PART ) + SYMBOLS ( END ) + stop );
  statement_part ( SYMBOLS ( END ) + stop );
  /* end the scope here
   */  
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
BEGIN_NONTERMINAL_HANDLER ( void, constant_definition )  {  
  expect ( CONST, SYMBOLS ( IDENTIFIER ) + SYMBOLS ( EQUAL ) 
	   + FIRST ( CONSTANT ) + stop );
  expect ( IDENTIFIER, SYMBOLS ( EQUAL ) + FIRST ( CONSTANT ) + stop );
  expect ( EQUAL, FIRST ( CONSTANT ) + stop );
  constant ( stop );
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* VariableDefinition = TypeSymbol VariableList 
   | TypeSymbol "array" VariableList "[" Constant "]" . */
/* TypeSymbol =  "integer" | "Boolean" . */
BEGIN_NONTERMINAL_HANDLER ( void, variable_definition )  {
  /* TypeSymbol */
  expect ( BOOLEAN == _token ? BOOLEAN : INTEGER, 
	   SYMBOLS ( ARRAY ) + FIRST ( VARIABLE_LIST ) + stop );
  /* "array" VariableList ... */
  bool array = ( ARRAY == _token );
  if ( array ) {
    expect ( ARRAY, FIRST ( VARIABLE_LIST ) + SYMBOLS ( LEFT_BRACKET, 
							RIGHT_BRACKET ) 
	     + FIRST ( CONSTANT ) + stop );
  }
  /* VariableList = VariableName { "," VariableName } . */
  expect ( IDENTIFIER, FIRST ( VARIABLE_LIST ) + SYMBOLS ( COMMA, LEFT_BRACKET, 
							   RIGHT_BRACKET ) 
	   + FIRST ( CONSTANT ) + stop );
  while ( COMMA == _token ) {
    expect ( COMMA, SYMBOLS ( IDENTIFIER ) + FIRST ( CONSTANT ) 
	     + SYMBOLS ( LEFT_BRACKET, RIGHT_BRACKET ) + stop );
    expect ( IDENTIFIER, FIRST ( VARIABLE_LIST ) + FIRST ( CONSTANT ) 
	     + SYMBOLS ( COMMA, LEFT_BRACKET, RIGHT_BRACKET ) + stop );
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
  expect ( PROC, SYMBOLS ( IDENTIFIER ) + FIRST ( BLOCK ) + stop );
  expect ( IDENTIFIER, FIRST ( BLOCK ) + stop );
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
  syntax_check ( FIRST ( STATEMENT_PART ) + stop );  
  token_set extra = SYMBOLS ( SEMICOLON ) + stop;
  while ( ( _token >= SKIP && _token <= DO ) 
	  || IDENTIFIER == _token ) {
    switch ( _token ) {
    case SKIP: 
      /* EmptyStatement = "skip" . */
      expect ( SKIP, extra );
      break;    
    case READ:
      /* ReadStatement = "read" VariableAccessList . */
      expect ( READ, FIRST ( VARIABLE_ACCESS_LIST ) + extra );
      variable_access_list ( extra );
      break;
    case WRITE:
      /* WriteStatement = "write" ExpressionList . */
      expect ( WRITE, FIRST ( EXPRESSION_LIST ) + extra );
      expression_list ( extra );
      break;
    case CALL:
      /* ProcedureStatement = "call" ProcedureName . */
      expect ( CALL, SYMBOLS ( IDENTIFIER ) + extra );
      expect ( IDENTIFIER, extra );
      break;
    case IF:
      /* IfStatement = "if" GuardedCommandList "fi" . */
      expect ( IF, FIRST ( GUARDED_COMMAND_LIST ) + SYMBOLS ( FI ) + extra );
      guarded_command_list ( SYMBOLS ( FI ) + extra );
      expect ( FI, extra );
      break;
    case DO:
      /* DoStatement = "do" GuardedCommandList "od" . */
      expect ( DO, FIRST ( GUARDED_COMMAND_LIST ) + SYMBOLS ( OD ) + extra );
      guarded_command_list ( SYMBOLS ( OD ) + extra );
      expect ( OD, extra );
      break;
    case IDENTIFIER: /* ASSIGN */
      /* AssignmentStatement = VariableAccessList ":=" ExpressionList . */    
      variable_access_list ( SYMBOLS ( ASSIGN ) + FIRST ( EXPRESSION_LIST ) 
			     + extra );
      expect ( ASSIGN, FIRST ( EXPRESSION_LIST ) + extra );
      expression_list ( extra );
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
BEGIN_NONTERMINAL_HANDLER ( void, variable_access_list )  {  
  variable_access ( SYMBOLS ( COMMA ) + stop );
  while ( COMMA == _token ) {
    expect ( COMMA, FIRST ( VARIABLE_ACCESS ) + stop );
    variable_access ( stop );
  }
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* VariableAccess = VariableName [ IndexedSelector ] . */
BEGIN_NONTERMINAL_HANDLER ( void, variable_access )  {
  expect ( IDENTIFIER, FIRST ( INDEXED_SELECTOR ) + stop );
  /* IndexedSelector = "[" Expression "]" . */
  if ( LEFT_BRACKET == _token ) {
    indexed_selector ( stop );
  }  
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* ExpressionList = Expression { "," Expression } . */
BEGIN_NONTERMINAL_HANDLER ( void, expression_list ) {
  expression ( SYMBOLS ( COMMA ) + stop );
  while ( COMMA == _token ) {
    expect ( COMMA, FIRST ( EXPRESSION ) + stop );
    expression ( stop );
  }
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
  expression ( SYMBOLS ( GUARD_POINT ) + FIRST ( STATEMENT_PART ) + stop );
  expect ( GUARD_POINT, FIRST ( STATEMENT_PART ) + stop );
  statement_part ( stop );
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* Expression = PrimaryExpression { PrimaryOperator PrimaryExpression } . */
/* PrimaryOperator = "&" | "|" . */
BEGIN_NONTERMINAL_HANDLER ( void, expression )  {
  primary_expression ( FIRST ( PRIMARY_OPERATOR ) 
		       + FIRST ( PRIMARY_EXPRESSION ) + stop );
  /* PrimaryOperator (= "&" | "|" .) */  
  syntax_check ( FIRST ( PRIMARY_OPERATOR ) 
		 + FIRST ( PRIMARY_EXPRESSION ) + stop ); 
  while ( LOGICAL_AND == _token || LOGICAL_OR == _token ) {
    expect ( LOGICAL_AND == _token ? LOGICAL_AND : LOGICAL_OR, 
	     FIRST ( PRIMARY_EXPRESSION ) + stop );
    primary_expression ( FIRST ( PRIMARY_OPERATOR ) 
			 + FIRST ( PRIMARY_EXPRESSION ) + stop );     
  }
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* PrimaryExpression = SimpleExpression [ RelationalOperator	\
   SimpleExpression ] . */
/* RelationalOperator = "<" | "=" | ">" . */
BEGIN_NONTERMINAL_HANDLER ( void, primary_expression )  {
  simple_expression ( FIRST ( RELATIONAL_OPERATOR ) 
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
    simple_expression ( FIRST ( RELATIONAL_OPERATOR ) 
			+ FIRST ( SIMPLE_EXPRESSION ) + stop );    
  }  
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* SimpleExpression = [ "-" ] Term { AddingOperator Term } . */
BEGIN_NONTERMINAL_HANDLER ( void, simple_expression )  {
  /* [ "-" ] Term */
  syntax_check ( SYMBOLS ( MINUS ) + stop );
  if ( MINUS == _token ) { 
    expect ( MINUS, FIRST ( TERM ) + FIRST ( ADDING_OPERATOR ) + stop );
  }
  term ( FIRST ( ADDING_OPERATOR ) + FIRST ( TERM ) + stop );
  /* { AddingOperator (= "+" | "-" .) Term } */
  syntax_check ( FIRST ( ADDING_OPERATOR ) + FIRST ( TERM ) + stop );
  while ( PLUS == _token || MINUS == _token ) {
    expect ( PLUS == _token ? PLUS : MINUS, FIRST ( TERM ) + stop );
    term ( FIRST ( ADDING_OPERATOR ) + FIRST ( TERM ) + stop );    
  }
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* Term = Factor { MultiplyingOperator Factor } . */
BEGIN_NONTERMINAL_HANDLER ( void, term )  {
  factor ( FIRST ( MULTIPLYING_OPERATOR ) + FIRST ( FACTOR ) + stop );
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
    factor ( FIRST ( MULTIPLYING_OPERATOR ) + FIRST ( FACTOR ) + stop );
  }
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* Factor = Constant | VariableAccess | "(" Expression ")" | "~" Factor . */
BEGIN_NONTERMINAL_HANDLER ( void, factor )  {
  switch ( _token ) {    
  case LEFT_PAREN:
    /* "(" Expression ")" */
    expect ( LEFT_PAREN, FIRST ( EXPRESSION ) 
	     + SYMBOLS ( RIGHT_PAREN ) + stop );
    expression ( SYMBOLS ( RIGHT_PAREN ) + stop );
    expect ( RIGHT_PAREN, stop );
    break;
  case LOGICAL_NOT:
    /* "~" Factor */
    expect ( LOGICAL_NOT, FIRST ( FACTOR ) + stop );
    factor ( stop );  
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
    /*
      FIX ME!!
      bool identifier = ( IDENTIFIER == _token );
    constant ( FIRST ( INDEXED_SELECTOR ) + stop );
    if ( identifier && LEFT_BRACKET == _token ) {
      indexed_selector ( stop );
    }
    */
    if ( IDENTIFIER == _token ) {
      variable_access ( stop );
    } else {
      constant ( stop );
    }
    break;
    
    /* ERROR! - can't get here 
       default:       
         assert ( 0 );
         break;
    */
  }
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
BEGIN_NONTERMINAL_HANDLER ( void, constant )  {
  switch ( _token ) {
  case IDENTIFIER:
    expect ( IDENTIFIER, stop );
    break;
  case FALSE:
    expect ( FALSE, stop );
    break;
  case TRUE:
    expect ( TRUE, stop );
    break;
  case NUMBER:
    expect ( NUMBER, stop );
    break;
  default:
    /* ERROR! - can't get here */    
    assert ( 0 );
    break;
  }
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

void parser::parse () {  
  move ();                      /* boot-strap the parser and get the */
  program ( SYMBOLS ( END_OF_FILE ) ); /* first token, then match the 
					  main block */
}


