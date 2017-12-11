/*----------------------------------------------------------------------
  File    : parser.cc
  Contents: 
  Author  : Ben Burnett
  History : 04.02.2007 file created
----------------------------------------------------------------------*/

#if HAVE_CONFIG_H
#include <config.h>
#endif 

#include "error.h"
#include "parser.h"
#include <cassert>
#include <cstdarg>
#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

/*----------------------------------------------------------------------
  Namespace Inclusions
----------------------------------------------------------------------*/

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::make_pair;
using std::pair;
using std::string;
using std::vector;

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/

/* -- quick macro to speed up the process of creating stop sets:
   
   SYMBOLS (COMMA, END, PLUS) 

   Will return an STL set containing those token ids.  These procedures will 
   also eliminate duplicates as well, thus:

   SYMBOLS (COMMA, END, PLUS, PLUS) = { COMMA, END, PLUS }
   
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
#define BEGIN_NONTERMINAL_HANDLER_X(r,x)	\
  r parser::x {
#define END_NONTERMINAL_HANDLER }
#define PREMATURE_END_NONTERMINAL_HANDLER
#define  DEBUG_OUTPUT(x)
#else
static int __indent = 0;
#define BEGIN_NONTERMINAL_HANDLER(r,x)				\
  r parser::x NONTERMINAL_PARAMS {				\
    char const *__fn = #x;					\
    for (int __i = 0; __i < __indent; ++__i) {		\
      cout << ' '; }						\
    cout << "<" << __fn  << ">" << endl;			\
    __indent++;
#define BEGIN_NONTERMINAL_HANDLER_X(r,x)			\
  r parser::x {							\
    char const *__fn = #x;					\
    for (int __i = 0; __i < __indent; ++__i) {		\
      cout << ' '; }						\
    cout << "<" << __fn  << ">" << endl;			\
    __indent++;
#define END_NONTERMINAL_HANDLER				\
  __indent--;						\
  for (int __i = 0; __i < __indent; ++__i) {		\
    cout << ' '; }					\
  cout << "</" << __fn << ">" << endl; }		
#define PREMATURE_END_NONTERMINAL_HANDLER {END_NONTERMINAL_HANDLER
#define DEBUG_OUTPUT(x)						\
  for (int __i = 0; __i < __indent; ++__i) {			\
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

/* --------------------------------------------------------------------*/

int parser::_next_label = 1;

/*----------------------------------------------------------------------
  Friend Functions/Operators
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Exceptions
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Main Methods
----------------------------------------------------------------------*/

parser::parser (ifstream & s, symboltbl & t, error_interface & err,
		 emitter_interface & emit)
  : _emitter (emit), _errors (err), _scanner (s), _symbols (t), 
    _null (token::null), _expected (NONE) {
  unsigned int i, j;
  /* Create the "first symbols" sets.  At this point using STL sets is a
     little excessive, as there will never be duplicates; however, as we 
     descend through the parse tree and use these to build follow sets 
     and stop sets, the fact that sets forbid duplicates will become more 
     important. */
  for (i = 0; i < LAST_TOKEN; ++i) {
    for (j = 0; NONE != first_symbols[i][j] && j < 8; ++j) {
      _first_symbols[i].insert (
	  static_cast<token_code> (first_symbols[i][j]));
    }
  }
}

/* --------------------------------------------------------------------*/

token_code parser::move () {
  return (_token = _scanner.next_token ());
}

/*--------------------------------------------------------------------*/

int parser::new_label () {
  return _next_label++;
}

/* --------------------------------------------------------------------*/

void parser::expect (token_code c, token_set const &stop) {
  _expected = c;                /* -- set which symbol we expect */
  if (_expected == _token) {  /* ensure that we see it and */  
    move ();                    /* move on */
  } else {                      /* -- on failure, signal a syntax */
    syntax_error (stop); }    /* error to the user */
  _expected = NONE;             /* -- reset the expected symbol and */
  syntax_check (stop);        /* ensure that we are in a sane state */
}

/* --------------------------------------------------------------------*/

void parser::expect (string &name, token_set const &stop) {
  _expected = IDENTIFIER;       /* -- we always expect an ID */
  if (_expected == _token) {  /* ensure that we see it, */  
    _token.value (name);      /* record the name and */
    move ();                    /* move on */
  } else {                      /* -- on failure, signal a syntax */
    syntax_error (stop); }    /* error to the user */
  _expected = NONE;             /* -- reset the expected symbol and */
  syntax_check (stop);        /* ensure that we are in a sane state */
}

/* --------------------------------------------------------------------*/

token & parser::define (string const & name, kind::code kind, 
			 type::code type, int value, int size, 
			 int displ, int start) {
  if (!name.empty ()) { 
    /* -- we are only concerned if the ID is defined withing the current
       scope (i.e. the *top*): if one exists, then the creation of a new one
       should *not* be allowed */
    symboltbl::iterator it = _symbols.find_top (name);
    if (it != _symbols.end ()) {
      error (error::input::duplicate, it->second);
      return it->second;    
    } else {

      /* cout << "# " << name << ", level: " << _symbols.level () << "\n"; */

      token tok (IDENTIFIER, kind, type, name, value, size,
		  _symbols.level (), displ, start);
      pair<symboltbl::iterator, bool> p = _symbols.insert (name, tok);
      if (p.second) { 
	return p.first->second; 
      }
    }
  }
  return _null;
}

/* --------------------------------------------------------------------*/

/* --- we never want to call the symbol table's find method directly
   because we would like to add symbols transparently to the table if 
   they do not exist. this means that as far as the searcher is concerned
   anything they search for -- excluding the empty string -- will always
   be found in the symbol table.  it's only a mater of whether it will
   issue a runtime error or not */
token & parser::find (string const &name) {
  symboltbl::iterator it = _symbols.find (name);
  if (it != _symbols.end ()) { 
    return it->second; 
  } 
  token undefined (IDENTIFIER, name);
  error (error::input::undefined_symbol, undefined);
  suggest (name);
  return define (name, kind::undefined, type::universal);
}

/* --------------------------------------------------------------------*/

std::string soundex(const std::string & input, const unsigned int n)
{
  static char lookup[] = {
    '0', /* A */ '1', '2', '3', '0', '1', '2', '0', '0', '2', '2', '4',
    '5', '5', '0', '1', '0', '6', '2', '3', '0', '1', '0', '2', '0', '2'
  };
  
  // keep the first character intact
  std::string result = input.substr(0, 1);

  // compute value for each character thereafter
  for (unsigned int i = 1; i < input.length(); i++) {
    // skip non-alpha characters
    if (!isalpha(input[i])) {
      continue;
    }

    // uppercase the input value
    const char lookupInput = islower(input[i]) ? toupper(input[i]) : input[i];
    // lookup it's value
    const char *lookupVal = &lookup[lookupInput-'A'];

    // make sure this isn't a dupe value
    if (result.find(lookupVal, 0) != 0 ) {
      result.append(lookupVal);
    }
  }

  // make sure we could actually encode something
  if (result.length() >= n) {
    return result.substr(0, n - 1);
  }

  // In cases of empty strings (or strings with no encodable
  // characters), return Z000
  return "Z000";
}

std::string soundex(const std::string &input)
{
  return soundex(input, 10);
}

/* --------------------------------------------------------------------*/

void parser::suggest(std::string const & name)
{
  string s = soundex(name), t;
  symboltbl::iterator it = _symbols.begin();
  while (it != _symbols.end()) {
    t = soundex(it->first);
    if (s == t) {
      token undefined (IDENTIFIER, it->first);
      error (error::input::did_you_mean, undefined);
      break;
    }
  }
}

/* --------------------------------------------------------------------*/

void parser::syntax_check (token_set const & stop) {
  if (!stop.count (_token)) {
    syntax_error (stop);
  }
}

/* --------------------------------------------------------------------*/

void parser::type_check (type::code & t1, type::code t2) {
  if (t1 != t2) {
    if (t1 && t2) { /* != type::univeral */
      error (error::input::type_mismatch);
    }
    t1 = type::universal;
  }
}

/* --------------------------------------------------------------------*/

void parser::error (error::input::code c) const {
  _errors.error (c);  
}

/* --------------------------------------------------------------------*/

void parser::error (error::input::code c, token const & t) const {
  string s;
  t.value (s);
  _errors.error (c, s.c_str ());  
}

/* --------------------------------------------------------------------*/

void parser::error (error::input::code c, token const & t1,
		     token const & t2) const {
  string s1, s2;
  t1.value (s1); t2.value (s2);
  _errors.error (c, s1.c_str (), s2.c_str ());
}

/* --------------------------------------------------------------------*/

void parser::syntax_error (token_set const & stop) {
  /* --- issue the appropriate error message: */  
  if (UNKNOWN == _token) {    /* if we don't recognize the symbol: */
    error (error::input::unrecognized, _token);
  } else {                      /* if we do recognize the symbol: */    
    if (NONE == _expected) {  /* a) we may not have been expecting it */
      error (error::input::unexpected, _token);    
    } else {                    /* or (b) we may have been expecting 
				   something else */
      token expected (_expected, token::friendly_name (_expected));
      error (error::input::expected_before, expected, _token);
    }
  }
  /* --- attempt to return the parser to a sane state --- */
  while (!stop.count (_token)) { 
    move ();                    /* -- find the next recognized symbol */
  }
}

/* --------------------------------------------------------------------*/

/* Program = Block "." . */
BEGIN_NONTERMINAL_HANDLER (void, program) {  
  int begin  = new_label (), 
    variable = new_label ();
  _symbols.push ();             /* start a new scope */
  _emitter.program (variable, begin);
  block (begin, variable, SYMBOLS (PERIOD) + stop);
  _emitter.end_program ();
  _symbols.pop ();              /* end the scope */
  expect (PERIOD, stop);
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* Block = "begin" DefinitionPart StatementPart "end" .  */  
BEGIN_NONTERMINAL_HANDLER_X (void, block (int begin, int variable, 
					    token_set const & stop))  {  
  int count, displacement = 3;
  expect (BEGIN, FIRST (DEFINITION_PART) + FIRST (STATEMENT_PART) 
	   + SYMBOLS (END) + stop);
  count = definition_part (displacement, FIRST (STATEMENT_PART) 
			    + SYMBOLS (END) + stop);
  _emitter.define_argument (variable, count);
  _emitter.define_address (begin);
  statement_part (SYMBOLS (END) + stop);  
  expect (END, stop);
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* DefinitionPart = { Definition ";" } . */
/* Definition = ConstantDefinition | VariableDefinition	\
   | ProcedureDefinition . */
BEGIN_NONTERMINAL_HANDLER_X 
(int, definition_part (int &displacement, token_set const &stop)) {
  int variables = 0;
  syntax_check (FIRST (DEFINITION_PART) + stop);
  token_set extra = SYMBOLS (SEMICOLON) + stop;
  while (_token >= BOOLEAN && _token <= PROC) {
    switch (_token) {
    case CONST:    
      constant_definition (extra);
      break;
    case BOOLEAN:
    case INTEGER:
      variables += variable_definition (displacement, extra);
      break;
    case PROC:
      procedure_definition (extra);
      break;
    default:   
      /* --- for empty definitions -- also quiets the 'enumeration value
	 'bla' not handled in switch' warnings */
      return variables;
    }
    expect (SEMICOLON, FIRST (DEFINITION_PART) + stop);
    syntax_check (FIRST (DEFINITION_PART) + stop);
  }  
  PREMATURE_END_NONTERMINAL_HANDLER;
  return variables;
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* ConstantDefinition = "const" ConstantName "=" Constant . */
BEGIN_NONTERMINAL_HANDLER (void, constant_definition)  {    
  string name;
  expect (CONST, SYMBOLS (IDENTIFIER) + SYMBOLS (EQUAL) 
	   + FIRST (CONSTANT) + stop);    
  expect (name, SYMBOLS (EQUAL) + FIRST (CONSTANT) + stop);
  expect (EQUAL, FIRST (CONSTANT) + stop);
  parser::constant_type c = constant (stop);
  define (name, kind::constant, c.second, c.first);  
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* VariableDefinition = TypeSymbol VariableList 
   | TypeSymbol "array" VariableList "[" Constant "]" . */
/* TypeSymbol =  "integer" | "Boolean" . */
BEGIN_NONTERMINAL_HANDLER_X 
(int, variable_definition (int &displacement, token_set const &stop)) {  
  bool array; string name; constant_type c;
  vector<string> variables; vector<string>::iterator it;
  kind::code kind; type::code type; int value = 0, size = 1; 
  /* TypeSymbol - type::code values based on token_code values */
  type = (BOOLEAN == _token ? type::boolean : type::integer);
  expect (static_cast<token_code> (type), 
	   SYMBOLS (ARRAY) + FIRST (VARIABLE_LIST) + stop); 
  /* "array" VariableList ... */    
  kind = kind::variable;
  if ( ( array = ( ARRAY == _token ) ) ) {
    kind = kind::array;
    expect (ARRAY, FIRST (VARIABLE_LIST) 
	     + SYMBOLS (LEFT_BRACKET, RIGHT_BRACKET) 
	     + FIRST (CONSTANT) + stop);
  }  
  /* VariableList = VariableName { "," VariableName } . */
  do { 
    if (COMMA == _token) {
      expect (COMMA, SYMBOLS (IDENTIFIER) + FIRST (CONSTANT) 
	       + SYMBOLS (LEFT_BRACKET, RIGHT_BRACKET) + stop); 
    }
    expect (name, FIRST (VARIABLE_LIST) 
	     + SYMBOLS (COMMA, LEFT_BRACKET, RIGHT_BRACKET) 
	     + FIRST (CONSTANT) + stop);
    variables.push_back (name);
  } while (COMMA == _token);
  /* if this is an array we will find: ... "[" Constant "]" . */
  if (array) {
    expect (LEFT_BRACKET, FIRST (CONSTANT) 
	     + SYMBOLS (RIGHT_BRACKET) + stop);
    c = constant (SYMBOLS (RIGHT_BRACKET) + stop);
    if (type::integer != c.second && type::universal != c.second) {
      error (error::input::integer);
    } else {
      size = c.first;
    }    
    expect (RIGHT_BRACKET, stop);
  }  
  /* finally, do the actual defining of the variables */
  for (it = variables.begin (); it != variables.end (); ++it) {
    /* cout << "# " << *it << ", kind: " << kind << ", type: " 
       << token::friendly_name ((token_code) type) << ", value: " 
       << value << ", displ: " << displacement << "\n"; 
    */
    define (*it, kind, type, value, size, displacement);
    displacement += size;
  }  
  PREMATURE_END_NONTERMINAL_HANDLER;
  return variables.size () * size;
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* ProcedureDefinition = "proc" ProcedureName Block . */
BEGIN_NONTERMINAL_HANDLER (void, procedure_definition)  {  
  string name;  
  expect (PROC, SYMBOLS (IDENTIFIER) + FIRST (BLOCK) + stop);  
  expect (name, FIRST (BLOCK) + stop);      
  int proc     = new_label (),
      variable = new_label (), 
      begin    = new_label (); 
  define (name, kind::procedure, type::universal, 0, 0, 0, proc);
  _symbols.push ();             /* start a new scope */
  _emitter.define_address (proc); /* record the procedure's address */
  _emitter.procedure (variable, begin);
  block (begin, variable, stop);
  _emitter.end_procedure ();  
  _symbols.pop ();              /* end the scope */
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
BEGIN_NONTERMINAL_HANDLER (void, statement_part)  {
  syntax_check (FIRST (STATEMENT_PART) + stop);  
  token_set extra = SYMBOLS (SEMICOLON) + stop;
  int start, done, loop;
  while ((_token >= SKIP && _token <= DO) 
	  || IDENTIFIER == _token) {
    string name; token tok;
    parser::token_vector      vars;  parser::token_vector::iterator      it;
    parser::expression_vector exprs; parser::expression_vector::iterator jt;
    switch (_token) {
    case SKIP: 
      DEBUG_OUTPUT ("skip");
      /* EmptyStatement = "skip" . */
      expect (SKIP, extra);
      break;    
    case READ:
      DEBUG_OUTPUT ("read");
      /* ReadStatement = "read" VariableAccessList . */
      expect (READ, FIRST (VARIABLE_ACCESS_LIST) + extra);
      vars = variable_access_list (extra);
      _emitter.read (vars.size ());
      break;
    case WRITE:
      DEBUG_OUTPUT ("write");
      /* WriteStatement = "write" ExpressionList . */
      expect (WRITE, FIRST (EXPRESSION_LIST) + extra);
      exprs = expression_list (extra);
      _emitter.write (exprs.size ());
      break;
    case CALL:
      DEBUG_OUTPUT ("call");
      /* ProcedureStatement = "call" ProcedureName . */
      expect (CALL, SYMBOLS (IDENTIFIER) + extra);      
      expect (name, extra);
      if (!name.empty ()) { 
	/* -- here we look up the ID by name and check it's kind, if it is 
	   a procedure then all is well, if it is not, then we have an 
	   error or some sort or another */
	tok = find (name);
	if (tok.kind () != kind::procedure) {
	  token proc (PROC, name);
	  error (error::input::procedure, proc);
	} else {
	  _emitter.call (_symbols.level () - tok.level (), tok.start ());
	  _symbols.push_storage (3);
	}
      }
      break;
    case IF:
      /*
	expect (IF);
	type = expression ()
	check_types (type, type::boolean);
	expect (THEN);
	new_label (label1);
	emit (DO, label1);
	pop_storage (1);
	statement ();
	if current_symbol == ELSE then
	  expect (ELSE);
	  new_label (label2);
	  emit (GOTO, label2);
	  emit (defaddr, label1);
	  statement ();
	  emit (defaddr, label2);
	else
	  emit (defaddr, label1);
	end;

n-1: ...        
  n: found := true;
n+1: if found -> write 1; []
n+2:  ~found -> write 0; 
n+3: fi
n+4: ...
        
        ARROW    L_0
        CONSTANT 1
        WRITE    1
        BAR      L_NEXT
   L_0: ARROW    L_1
        CONSTANT 0
        WRITE    1
        BAR      L_NEXT
   L_1: F1       n+1
L_NEXT: ... next code ...
       */

      DEBUG_OUTPUT ("if");
      /* IfStatement = "if" GuardedCommandList "fi" . */      
      expect (IF, FIRST (GUARDED_COMMAND_LIST) + SYMBOLS (FI) + extra);
      start = new_label (), done = new_label ();
      guarded_command_list (start, done, SYMBOLS (FI) + extra);
      _emitter.define_address (start);
      _emitter.fi (line ());      
      _emitter.define_address (done);
      expect (FI, extra);
      break;
    case DO:
      /*
	new_label (label1);
	emit (defaddr, label1);
	expect (WHILE);
	type = expression ();
	check_type (type, type::boolean);
	expect (DO);
	new_label (label2);
	emit (DO, label2);
	pop_storage (1);
	statement ();
	emit (GOTO, label1);
	emit (defaddr, label2);

	...        
ok := true;
do ok -> 
  ok := (i < 10);
  write 1;
od
...
        
   L_0: ARROW    L_NEXT
        ...      // evaluate ==> ok := (i < 10);
		 CONSTANT 1
        WRITE    1
        BAR      L_0   
L_NEXT: ... next code ...
	
       */
      DEBUG_OUTPUT ("do");
      /* DoStatement = "do" GuardedCommandList "od" . */
      expect (DO, FIRST (GUARDED_COMMAND_LIST) + SYMBOLS (OD) + extra);
      start = new_label (), loop = new_label ();
      _emitter.define_address (loop);
      guarded_command_list (start, loop, SYMBOLS (OD) + extra);
      _emitter.define_address (start);
      expect (OD, extra);
      break;
    case IDENTIFIER: /* ASSIGN */
      DEBUG_OUTPUT ("assign");
      /* AssignmentStatement = VariableAccessList ":=" ExpressionList . */    
      vars = variable_access_list (SYMBOLS (ASSIGN) 
				    + FIRST (EXPRESSION_LIST) 
				    + extra);
      expect (ASSIGN, FIRST (EXPRESSION_LIST) + extra);
      exprs = expression_list (extra);      
      /* -- check types involved in the assignment statement */
      if (vars.size () == exprs.size ()) {
	for (it = vars.begin (), jt = exprs.begin (); 
	      it != vars.end (); ++it, ++jt) {
	  type_check (*jt, it->type ());	  
	}
	_emitter.assign (vars.size ());
	_symbols.pop_storage (vars.size () + 1);
      } else {
	/* -- unbalanced assignment statement (this is a a bit ad-hoc, but 
	 it will do for now ... it just tells the user which side is heavy */	
	token heavy (IDENTIFIER, vars.size () > exprs.size () 
		      ? "lhs" : "rhs");
	error (error::input::balance, heavy);
      }
      break;
    default:
      /* ERROR! - can't get here */
      assert (0);
      break;
    }  
    expect (SEMICOLON, FIRST (STATEMENT_PART) + stop);    
  }  
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* VariableAccessList = VariableAccess { "," VariableAccess } . */
BEGIN_NONTERMINAL_HANDLER (parser::token_vector, variable_access_list) {
  parser::token_vector vars;
  do { 
    if (COMMA == _token) {
      expect (COMMA, FIRST (VARIABLE_ACCESS) + stop);
    }
    vars.push_back (variable_access (SYMBOLS (COMMA) + stop));    
  } while  (COMMA == _token);
  PREMATURE_END_NONTERMINAL_HANDLER;
  return vars;
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* VariableAccess = VariableName [ IndexedSelector ] . */
BEGIN_NONTERMINAL_HANDLER (token, variable_access) {
  string name; token tok;
  expect (name, FIRST (INDEXED_SELECTOR) + stop);  
  tok = find (name);

  /* cout << "# " << name 
     << ", level: " << tok.level () 
     << ", current block level: " << _symbols.level () << "\n"; */
  
  _emitter.variable (_symbols.level () - tok.level (), 
		      tok.displacement ());
  _symbols.push_storage (1);
  /* IndexedSelector = "[" Expression "]" . */
  if (LEFT_BRACKET == _token) {         
    indexed_selector (tok.size (), stop);
  }   
  PREMATURE_END_NONTERMINAL_HANDLER;
  return tok;  
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* ExpressionList = Expression { "," Expression } . */
BEGIN_NONTERMINAL_HANDLER (parser::expression_vector, expression_list) {
  parser::expression_vector exprs;
  do { 
    if (COMMA == _token) {
      expect (COMMA, FIRST (EXPRESSION) + stop); 
    }
    exprs.push_back (expression (SYMBOLS (COMMA) + stop));
  } while (COMMA == _token);
  PREMATURE_END_NONTERMINAL_HANDLER;
  return exprs;  
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* GuardedCommandList = GuardedCommand { "[]" GuardedCommand } . */
BEGIN_NONTERMINAL_HANDLER_X (void, guarded_command_list (int &start, 
							   int go_to, 
							   token_set const
							   &stop)) {
  guarded_command (start, go_to, SYMBOLS (GUARD_SEPARATOR) + stop);
  while (GUARD_SEPARATOR == _token) {
    expect (GUARD_SEPARATOR, FIRST (GUARDED_COMMAND) + stop);
    guarded_command (start, go_to, stop);
  }
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* GuardedCommand = Expression "->" StatementPart . */
BEGIN_NONTERMINAL_HANDLER_X (void, guarded_command (int &this_label, 
						      int go_to, 
						      token_set const 
						      &stop)) {
  _emitter.define_address (this_label);
  type::code type = expression (SYMBOLS (GUARD_POINT) 
				 + FIRST (STATEMENT_PART) + stop);
  /* -- ensure that the expression given before the arrow is of type
     boolean; otherwise, issue an error to the user */
  if (type::boolean != type && type::universal != type) {
    error (error::input::boolean);
  }  
  this_label = new_label ();
  _emitter.arrow (this_label);
  expect (GUARD_POINT, FIRST (STATEMENT_PART) + stop);
  statement_part (stop);
  _emitter.bar (go_to);
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* Expression = PrimaryExpression { PrimaryOperator PrimaryExpression } . */
/* PrimaryOperator = "&" | "|" . */
BEGIN_NONTERMINAL_HANDLER (type::code, expression)  {
  type::code t1, t2; token_code code;
  t1 = primary_expression (FIRST (PRIMARY_OPERATOR) 
			    + FIRST (PRIMARY_EXPRESSION) + stop);
  /* PrimaryOperator (= "&" | "|" .) */  
  syntax_check (FIRST (PRIMARY_OPERATOR) 
		 + FIRST (PRIMARY_EXPRESSION) + stop); 
  while (LOGICAL_AND == _token || LOGICAL_OR == _token) {
    code = _token;
    expect (_token, FIRST (PRIMARY_EXPRESSION) + stop);
    t2 = primary_expression (FIRST (PRIMARY_OPERATOR) 
			      + FIRST (PRIMARY_EXPRESSION) + stop);     
    type_check (t1, t2);
    switch (code) {
    case LOGICAL_AND: _emitter.and$ (); break;
    case  LOGICAL_OR: _emitter.or$ ();  break;
    default:          /* do nothing */  break;
    }
    _symbols.pop_storage (1);
  }
  PREMATURE_END_NONTERMINAL_HANDLER;
  return t1;
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* PrimaryExpression = SimpleExpression [ RelationalOperator	\
   SimpleExpression ] . */
/* RelationalOperator = "<" | "=" | ">" . */
BEGIN_NONTERMINAL_HANDLER (type::code, primary_expression)  {
  type::code t1, t2; token_code code;
  bool relational = false;
  t1 = simple_expression (FIRST (RELATIONAL_OPERATOR) 
			   + FIRST (SIMPLE_EXPRESSION) + stop);
  /* RelationalOperator (= "<" | "=" | ">" .) */
  syntax_check (FIRST (RELATIONAL_OPERATOR) 
		 + FIRST (SIMPLE_EXPRESSION) + stop);
  while (_token >= EQUAL && _token <= LESS_THAN) {
    code = _token;
    relational = true;    
    expect (_token, FIRST (SIMPLE_EXPRESSION) + stop);
    t2 = simple_expression (FIRST (RELATIONAL_OPERATOR) 
			     + FIRST (SIMPLE_EXPRESSION) + stop);    
    type_check (t1, t2);
    switch (code) {
    case GREATER_THAN: _emitter.greater (); break;
    case        EQUAL: _emitter.equal ();   break;    
    case    LESS_THAN: _emitter.less ();    break;
    default:           /* do nothing */     break;
    } 
    _symbols.pop_storage (1);
  }  
  PREMATURE_END_NONTERMINAL_HANDLER;
  /* -- here we determine if we have done a comparison of if we've 
     simply wrapped a simple expression.  if we've done a comparison
     then we change the type to boolean; otherwise, keep the wrapped 
     type */
  return relational ? type::boolean : t1;
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* SimpleExpression = [ "-" ] Term { AddingOperator Term } . */
BEGIN_NONTERMINAL_HANDLER (type::code, simple_expression)  {  
  type::code t1, t2; token_code code;
  /* [ "-" ] Term */
  bool negative = false; 
  syntax_check (SYMBOLS (MINUS) + stop);
  if (MINUS == _token) { 
    negative = true;
    expect (MINUS, FIRST (ADDING_OPERATOR) + FIRST (TERM) + stop);
  }
  t1 = term (FIRST (ADDING_OPERATOR) + FIRST (TERM) + stop);
  if (negative) { 
    type_check (t1, type::integer); 
    _emitter.minus ();
  }
  /* { AddingOperator (= "+" | "-" .) Term } */
  syntax_check (FIRST (ADDING_OPERATOR) + FIRST (TERM) + stop);
  while (PLUS == _token || MINUS == _token) {
    code = _token;
    expect (_token, FIRST (TERM) + stop);
    t2 = term (FIRST (ADDING_OPERATOR) + FIRST (TERM) + stop);    
    type_check (t1, t2); 
    switch (code) {
    case  PLUS: _emitter.add ();      break;
    case MINUS: _emitter.subtract (); break;
    default:    /* do nothing */      break;
    }
    _symbols.pop_storage (1);
  }  
  PREMATURE_END_NONTERMINAL_HANDLER;
  return t1;
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* Term = Factor { MultiplyingOperator Factor } . */
BEGIN_NONTERMINAL_HANDLER (type::code, term)  {
  type::code t1, t2; token_code code;
  t1 = factor (FIRST (MULTIPLYING_OPERATOR) + FIRST (FACTOR) + stop);
  /* MultiplyingOperator (= "*" | "/" | "\" .) Factor */
  while (_token >= MULTIPLY && _token <= MODULO) {
    code = _token;
    expect (_token, FIRST (FACTOR) + stop);
    syntax_check (FIRST (MULTIPLYING_OPERATOR) + FIRST (FACTOR) + stop);
    t2 = factor (FIRST (MULTIPLYING_OPERATOR) + FIRST (FACTOR) + stop);
    type_check (t1, t2);    
    switch (code) {
    case MULTIPLY: _emitter.multiply (); break;
    case   DIVIDE: _emitter.divide ();   break;
    case   MODULO: _emitter.modulo ();   break;
    default:       /* do nothing */      break;
    }
    _symbols.pop_storage (1);
  }
  PREMATURE_END_NONTERMINAL_HANDLER;
  return t1;
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* Factor = Constant | VariableAccess | "(" Expression ")" | "~" Factor . */
BEGIN_NONTERMINAL_HANDLER (type::code, factor)  {
  string name; constant_type c; token tok;
  type::code type = type::universal;
  // bool is_constant = false;
  switch (_token) {    
  case LEFT_PAREN:
    /* "(" Expression ")" */
    expect (LEFT_PAREN, FIRST (EXPRESSION) 
	     + SYMBOLS (RIGHT_PAREN) + stop);
    type = expression (SYMBOLS (RIGHT_PAREN) + stop);
    expect (RIGHT_PAREN, stop);
    break;
  case LOGICAL_NOT:
    /* "~" Factor */
    expect (LOGICAL_NOT, FIRST (FACTOR) + stop);
    type = factor (stop);
    type_check (type, type::boolean);
    _emitter.not$ ();
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
         match (IDENTIFIER);
         if (LEFT_BRACKET == _token) {
           indexed_selector (stop);
         }  
       }
    */
        
    if (IDENTIFIER == _token) {
      _token.value (name);
      tok = find (name);
      if (tok.kind () == kind::constant) { 
	c = constant (stop);      
	_emitter.constant (c.first);
	_symbols.push_storage (1);
	type = c.second;      
      } else {
	tok = variable_access (stop);
	_emitter.value ();
	_symbols.push_storage (1);
	type = tok.type ();
      }
    } else {
      c = constant (stop);      
      _emitter.constant (c.first);
      _symbols.push_storage (1);
      type = c.second;      
    }
    
    /*
    if (IDENTIFIER == _token) {
      _token.value (name);
      tok = find (name);
      if (tok.kind () == kind::constant) { 
	is_constant = true;
      } else {
	tok = variable_access (stop);
	_emitter.value ();
	_symbols.push_storage (1);
	type = tok.type ();   
      } 
    }      
    if (is_constant) {
      c = constant (stop);      
      _emitter.constant (c.first);
      _symbols.push_storage (1);
      type = c.second;      
    }
    */
    break;    
  }
  PREMATURE_END_NONTERMINAL_HANDLER;
  return type;
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* IndexedSelector = "[" Expression "]" . */
BEGIN_NONTERMINAL_HANDLER_X (void, indexed_selector (int upper, token_set
						       const &stop))  {
  int line = _scanner.line ();  
  expect (LEFT_BRACKET, FIRST (EXPRESSION) 
	   + SYMBOLS (RIGHT_BRACKET) + stop);  
  type::code type = expression (SYMBOLS (RIGHT_BRACKET) + stop);
  if (type::integer != type && type::universal != type) {
    error (error::input::integer);
  }   
  _emitter.index (upper, line);
  expect (RIGHT_BRACKET, stop);
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

/* Constant = Numeral | BooleanSymbol | ConstantName . */
/* Numeral = Digit { Digit } . */
/* BooleanSymbol = "false" | "true" . */
/* Name = Letter { Letter | Digit | "_" } . */
BEGIN_NONTERMINAL_HANDLER (parser::constant_type, constant)  {
  string name; token t; int x = -1;
  type::code type = type::universal;    
  switch (_token) {
  case IDENTIFIER:
    /* -- here the type of the constant will be inffered from the 
       identifier we are given */    
    _token.value ( name );
    t = find ( name );
    if ( t.kind () != kind::constant ) {
      type = t.type ();
      t.value (x);
    } else {
      error (error::input::constant, t);
    }    
    break;
  case TRUE:
    type = type::boolean;
    x    = 1;
    break;
  case FALSE:    
    type = type::boolean;
    x    = 0;
    break;
  case NUMBER:
    type = type::integer;
    _token.value (x);    
    break;
  default:
    /* ERROR! - can't get here */    
    assert (0); 
    break;
  }  
  expect (_token, stop);
  PREMATURE_END_NONTERMINAL_HANDLER;
  return make_pair (x, type);
} END_NONTERMINAL_HANDLER;

/* --------------------------------------------------------------------*/

void parser::parse () {  
  move ();                      /* boot-strap the parser and get the */
  program (SYMBOLS (END_OF_FILE)); /* first token, then match the 
					  main block */
}

/* --------------------------------------------------------------------*/

unsigned int parser::line () const {
  return _scanner.line ();
}

/* --------------------------------------------------------------------*/

unsigned int parser::column () const {
  return _scanner.column ();
}
