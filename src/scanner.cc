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


#include <iostream>
using std::cout;


/*----------------------------------------------------------------------
  Namespace Inclusions
----------------------------------------------------------------------*/

using std::string;
using std::ifstream;
using std::runtime_error;

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/

/* --- all the symbols recognized by the PL language as 'letters'
   for identifiers --- */
static const int extra_letters[] = { '_' };

/* --- all the symbols recognized by the PL language --- */
static const int symbols[] = { '.', ',', ';', '[', ']', '&', '|', '~', 
				'<', '=', '>', '+', '-', '*', '/', '\\', 
			       '(', ')', ':', '$' };

/* --- all "whitespace" characters --- */
static const int wschars[] = { ' ', '\t', '\n' };

/*----------------------------------------------------------------------
  Friend Functions/Operators
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Main Methods
----------------------------------------------------------------------*/

scanner::scanner ( ifstream & s, symboltbl & t )
  : _fin ( s ), _symbols ( t ), 
    _column ( 0 ), _line ( 1 ) {
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
  for ( i = 0; i < count_of ( extra_letters ); ++i ) {
    _char_map[extra_letters[i]] = C_LETTER;
  }  
  for ( i = '0'; i <= '9'; ++i ) {
    _char_map[i] = C_DIGIT;
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
	   C_DIGIT == _char_map[c] ||
	   '_' == _char_map[c] );
}

/* --------------------------------------------------------------------*/

bool scanner::isnumeral ( int c ) const { /* --- return true if a number */
  return ( C_DIGIT == _char_map[c] );
}

/* --------------------------------------------------------------------*/

void scanner::get ( char & c ) { /* --- get the next character from */
  _fin.get ( c );                /* the input stream; incrementing the */
  _column++;                     /* location and line counter as we go */
  if ( '\n' == c ) {
    _line++;
    _column = 0;
  }
}

/* --------------------------------------------------------------------*/

void scanner::putback ( char c ) { /* --- put the last character back into */
  _fin.putback ( c );              /* the input stream; decrementing the */
  _column--;                       /* location and line counter as we go */
  if ( '\n' == c ) {
    _line--;
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
  symboltbl::iterator it = _symbols.find ( s );
    if ( it == _symbols.end () ) {    /* if not in symbol table, then */
    _token = token ( IDENTIFIER, s ); /* it's an identifier, so create a */
    _symbols.insert ( s, _token );    /* new entry in the symbol table */
  } else {
    _token = it->second;
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
  _token = token ( NUMBER, s );
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
  case ':':    
    code = UNKNOWN;             /* no ':' symbol in PL ... */      
    if ( '=' == peek () ) {     /* := */
      s += ':';
      code = ASSIGN;
      get ( c );                /* eat next char */      
    } break;
  default:
    /* ERROR -- can't get here, as we catch most unknown symbols before
                we enter this procedure */
    throw runtime_error ( "unreachable case in scanner.cc" );
    break;
  }  
  /* --- finally, create the token object */
  _token = token ( code, s += c );
}

/* -- see next_token 
  case '$':  
    s += c;
    code = COMMENT;    
    while ( '\n' != peek () ) { 
      get ( c ); 
      s += c;
    }
    c = ' ';
    break;
*/

/* --------------------------------------------------------------------*/

/* --- return the next token */
token const & scanner::next_token () throw ( runtime_error ) { 
  char d; string s;
  skipws ();                    /* -- skip all white-space, then based on */  
  int c = peek ();              /* the first character we decide how to */
  switch ( c ) {                /* continue scanning from here */
  case EOF:    
    _token = eof_token;         /* -- looks like we are done, let the */
    break;                      /* higher level processors know */
  case '$':    
    while ( '\n' != peek () ) { /* -- we treat comments as white-space */
      get ( d ); }              /* and pass it by, then we look for the */ 
    return next_token ();       /* next token in the input stream */
    break;
  default:
    switch ( _char_map[c] ) {     
    case C_LETTER: scan_word ();    break; 
    case C_DIGIT:  scan_numeral (); break;
    case C_SYMBOL: scan_symbol ();  break;
    default: 
      _token = token ( UNKNOWN, s += c );
      get ( d );                /* consume the unknown character */
      break;
    } 
  }
  return _token;                /* finally, return the token */
}

/* --------------------------------------------------------------------*/

int scanner::line () const {
  return _line;
}

/* --------------------------------------------------------------------*/

int scanner::column () const {
  return _column;
}

