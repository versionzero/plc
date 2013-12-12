/*----------------------------------------------------------------------
  File    : scanner.dc
  Contents: 
  Author  : Ben Burnett
  History : 10.01.2007 file created
----------------------------------------------------------------------*/

#if HAVE_CONFIG_H
#include <config.h>
#endif 

#include "scanner.h"
#include "misc.h"
#include <stdexcept>
#include <string>

/*----------------------------------------------------------------------
  Namespace Inclusions
----------------------------------------------------------------------*/

using std::string;
using std::ifstream;
using std::runtime_error;

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/

/* --- all the symbols recognized by the PL language --- */
static const int symbols[] = { '.', ',', ';', '[', ']', '&', '|', '~', 
				'<', '=', '>', '+', '-', '*', '/', '\\', 
			       '(', ')', '$' };

/* --- all "whitespace" characters --- */
static const int wschars[] = { ' ', '\t', '\n' };

/*----------------------------------------------------------------------
  Friend Functions/Operators
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Exceptions
----------------------------------------------------------------------*/

scanner::scanner_error::scanner_error ( string const & s )
  : _msg ( s ) {
}

/* ------------------------------------------------------------------ */

scanner::scanner_error::scanner_error () {}

/* ------------------------------------------------------------------ */

scanner::scanner_error::~scanner_error () throw () {}

/* ------------------------------------------------------------------ */

const char* scanner::scanner_error::what () const throw () {
  return _msg.c_str ();
}

/* ------------------------------------------------------------------ */

scanner::unknown_symbol::unknown_symbol ( char c ) {
  /* -- formating messages using the STL is a royal pain */
  string s = "unknown symbol '"; s += c; s += "'"; _msg = s;
}

/*----------------------------------------------------------------------
  Main Methods
----------------------------------------------------------------------*/

scanner::scanner ( ifstream & s, symboltbl & t )
  : _fin ( s ), _table ( t ), 
    _cpos ( 0 ), _cline ( 1 ) {
  unsigned int i;
  for ( i = 0; i < 128; ++i ) {
    _char_map[i] = C_ERROR;
  }
  for ( i = 'a'; i <= 'z'; ++i ) {
    _char_map[i] = C_LETTER;
  }
  for ( i = 'A'; i <= 'Z'; ++i ) {
    _char_map[i] = C_LETTER;
  }
  for ( i = '0'; i <= '9'; ++i ) {
    _char_map[i] = C_NUMERAL;
  }
  for ( i = 0; i < count_of ( symbols ); ++i ) {
    _char_map[symbols[i]] = C_SYMBOL;
  }
  for ( i = 0; i < count_of ( wschars ); ++i ) {
    _char_map[wschars[i]] = C_WS;
  }
}

/* --------------------------------------------------------------------*/

bool scanner::iswhite ( int c ) const {  /* --- returns true if c is a */
  return ( C_WS == _char_map[c] ); /* white-space character */
}

/* --------------------------------------------------------------------*/

bool scanner::iswordchar ( int c ) const { /* --- return true if character */
  return ( C_LETTER == _char_map[c] || /* is a valid word character */
	   C_NUMERAL == _char_map[c] ||
	   '_' == _char_map[c] );
}

/* --------------------------------------------------------------------*/

bool scanner::isnumeral ( int c ) const { /* --- return true if a number */
  return ( C_NUMERAL == _char_map[c] );
}

/* --------------------------------------------------------------------*/

void scanner::get ( char & c ) { /* --- get the next character from */
  _fin.get ( c );                /* the input stream; incrementing the */
  _cpos++;                       /* location and line counter as we go */
  if ( '\n' == c ) {
    _cline++;
    _cpos = 0;
  }
}

/* --------------------------------------------------------------------*/

void scanner::putback ( char c ) { /* --- put the last character back into */
  _fin.putback ( c );              /* the input stream; decrementing the */
  _cpos--;                         /* location and line counter as we go */
  if ( '\n' == c ) {
    _cline--;
  }
}

/* --------------------------------------------------------------------*/

char scanner::peek () {
  return _fin.peek ();
}

/* --------------------------------------------------------------------*/

void scanner::skipws () {       /* --- skip over white space */
  char c;
  while ( iswhite ( peek () ) ) {
    get ( c );                  /* consume and discard the character */
  }
}

/* --------------------------------------------------------------------*/

void scanner::scan_word () {
  char c;
  string s;
  while ( iswordchar ( peek () ) ) { 
    get ( c ); 
    s += c;
  }  
  if ( !_table.exists ( s ) ) { /* if not in symbol table, then it's */
    _ctoken = token ( IDENTIFIER, s ); /* an identifier, so create a */
    _table.insert ( s, _ctoken );      /* new entry in the symbol table */
  } else {
    _ctoken = _table.get ( s );
  }
}

/* --------------------------------------------------------------------*/

void scanner::scan_numeral () {
  char c;
  string s;
  while ( isnumeral ( peek () ) ) { 
    get ( c ); 
    s += c;
  }  
  _ctoken = token ( INTEGER, s );
}

/* --------------------------------------------------------------------*/

void scanner::scan_symbol () {
  char c;
  string s;
  token_code code;
  get ( c );  
  switch ( c ) {
  case '.': code = PERIOD;         break;
  case ',': code = COMMA;          break;
  case ';': code = SEMICOLON;      break;
  case '[': 
    code = LEFT_BRACKET;    
    if ( ']' == peek () ) {     /* [] */
      s += '[';
      code = GUARD_SEPARATOR;
      get ( c );                /* eat next char */
    } break;
  case ']': code = RIGHT_BRACKET;  break;
  case '&': code = LOGICAL_AND;    break;
  case '|': code = LOGICAL_OR;     break;
  case '~': code = LOGICAL_NOT;    break;
  case '<': code = LESS_THAN;      break;
  case '=': code = EQUAL;          break;
  case '>': code = GREATER_THAN;   break;
  case '+': code = PLUS;           break;
  case '-': 
    code = MINUS;
    if ( '>' == peek () ) {     /* -> */
      s += '-';
      code = GUARD_POINT;
      get ( c );                /* eat next char */
    } break;
  case '*':  code = MULTIPLY;      break;
  case '/':  code = MODULO;        break;
  case '\\': code = DIVIDE;        break;
  case '(':  code = LEFT_PAREN;    break;
  case ')':  code = RIGHT_PAREN;   break;
  case '$':  
    s += c;
    code = COMMENT;    
    while ( '\n' != peek () ) { 
      get ( c ); 
      s += c;
    }
    c = ' ';
    break;
  case ':':
    if ( '=' == peek () ) {     /* := */
      s += ':';
      code = ASSIGN;
      get ( c );                /* eat next char */      
    } else {
      /* ERROR -- no : symbol in PL, bail */
      throw scanner::unknown_symbol ( c );
    }
    break;
  default:
    /* ERROR -- can't get here, as we catch most unknown symbols before
                we enter this procedure */
    throw runtime_error ( "unreachable case in scanner.cc" );
    break;
  }  
  /* --- finally, create the token object */
  _ctoken = token ( code, s += c );
}

/* --------------------------------------------------------------------*/

token const & scanner::next_token () { /* --- return the next token */
  string s;
  skipws ();                    /* -- skip all white-space, then based on */  
  int c = peek ();              /* the first character we decide how to */
  if ( EOF != c ) {             /* continue scanning from here */
    switch ( _char_map[c] ) {     
    case C_LETTER:  scan_word ();    break; 
    case C_NUMERAL: scan_numeral (); break;
    case C_SYMBOL:  scan_symbol ();  break;
    default: 
      throw scanner::unknown_symbol ( c ); 
      break;
    } 
  } else {
    _ctoken = eof_token; 
  }
  return _ctoken;               /* finally, return the token */
}

/* --------------------------------------------------------------------*/

int scanner::line () const {
  return _cline;
}

/* --------------------------------------------------------------------*/

int scanner::column () const {
  return _cpos;
}

