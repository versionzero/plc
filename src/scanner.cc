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
#include <cstdlib>
#include <cassert>

/*----------------------------------------------------------------------
  Namespace Inclusions
----------------------------------------------------------------------*/

using std::string;
using std::ifstream;

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

scanner::scanner (ifstream & s)
  : _fin (s), _column (0), _line (1) {
  unsigned int i;
  for (i = 0; i < 128; ++i) {
    _char_map[i] = character::error;
  }
  for (i = 'a'; i <= 'z'; ++i) {
    _char_map[i] = character::letter;
  }
  for (i = 'A'; i <= 'Z'; ++i) {
    _char_map[i] = character::letter;
  }
  for (i = 0; i < count_of (extra_letters); ++i) {
    _char_map[extra_letters[i]] = character::letter;
  }  
  for (i = '0'; i <= '9'; ++i) {
    _char_map[i] = character::digit;
  }
  for (i = 0; i < count_of (symbols); ++i) {
    _char_map[symbols[i]] = character::symbol;
  }
  for (i = 0; i < count_of (wschars); ++i) {
    _char_map[wschars[i]] = character::ws;
  }
}

/* --------------------------------------------------------------------*/

bool scanner::iswhite (int c) const {  /* --- returns true if c is a */
  return (character::ws == _char_map[c]); /* white-space character */
}

/* --------------------------------------------------------------------*/

bool scanner::iswordchar ( int c ) const { /* --- return true if character */
  return ( character::letter == _char_map[c] || /* is a valid word character */
	   character::digit == _char_map[c] );
           /*
             the '_' is alredy considered a letter
           || '_' == _char_map[c] );
           */
}

/* --------------------------------------------------------------------*/

bool scanner::isnumeral (int c) const { /* --- return true if a number */
  return (character::digit == _char_map[c]);
}

/* --------------------------------------------------------------------*/

void scanner::get (char & c) { /* --- get the next character from */
  _fin.get (c);                /* the input stream; incrementing the */
  _column++;                     /* location and line counter as we go */
  if ('\n' == c) {
    _line++;
    _column = 0;
  }
}

/* --------------------------------------------------------------------*/

void scanner::putback (char c) { /* --- put the last character back into */
  _fin.putback (c);              /* the input stream; decrementing the */
  _column--;                       /* location and line counter as we go */
  if ('\n' == c) {
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
  while (iswhite (peek ())) {
    get (c);                  /* consume and discard the character */
  }
}

/* --------------------------------------------------------------------*/

void scanner::scan_word () {
  char c;
  string s;
  while (iswordchar (peek ())) { 
    get (c); 
    s += c;
  }
  symboltbl::iterator it = _symbols.find (s);
    if (it == _symbols.end ()) {    /* if not in symbol table, then */
    _token = token (IDENTIFIER, s); /* it's an identifier, so create a */
    _symbols.insert (s, _token);    /* new entry in the symbol table */
  } else {
    _token = it->second;
  }
}

/* --------------------------------------------------------------------*/

void scanner::scan_numeral () {
  char c;
  string s;
  while (isnumeral (peek ())) { 
    get (c); 
    s += c;
  }  
  _token = token (NUMBER, ::atoi (s.c_str ()));  
}

/* --------------------------------------------------------------------*/

void scanner::scan_symbol () {
  char c;
  string s;
  token_code code;
  code = UNKNOWN;
  get ( c );  
  switch ( c ) {
  case '.': code = PERIOD;         break;
  case ',': code = COMMA;          break;
  case ';': code = SEMICOLON;      break;
  case '[': 
    code = LEFT_BRACKET;    
    if (']' == peek ()) {     /* [] */
      s += '[';
      code = GUARD_SEPARATOR;
      get (c);                /* eat next char */
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
    if ('>' == peek ()) {     /* -> */
      s += '-';
      code = GUARD_POINT;
      get (c);                /* eat next char */
    } break;
  case '*':  code = MULTIPLY;      break;
  case '/':  code = MODULO;        break;
  case '\\': code = DIVIDE;        break;
  case '(':  code = LEFT_PAREN;    break;
  case ')':  code = RIGHT_PAREN;   break;
  case ':':    
    code = UNKNOWN;             /* no ':' symbol in PL ... */      
    if ('=' == peek ()) {     /* := */
      s += ':';
      code = ASSIGN;
      get (c);                /* eat next char */      
    } break;
  default:
    code = UNKNOWN;
    /* ERROR -- can't get here, as we catch unknown symbols before
                we enter this procedure */
    //assert ( 0 );
    break;
  }  
  /* --- finally, create the token object */
  _token = token (code, s += c);
}

/* --------------------------------------------------------------------*/

/* --- return the next token */
token const & scanner::next_token () { 
  char d; string s;
  skipws ();                    /* -- skip all white-space, then based on */  
  int c = peek ();              /* the first character we decide how to */
  switch (c) {                /* continue scanning from here */
  case EOF:    
    _token = token::eof_token;  /* -- looks like we are done, let the */
    break;                      /* higher level processors know */
  case '$':    
    while ('\n' != peek ()) { /* -- we treat comments as white-space */
      get (d); }              /* and pass it by, then we look for the */ 
    return next_token ();       /* next token in the input stream */
    break;
  default:
    switch (_char_map[c]) {     
    case character::letter: scan_word ();    break; 
    case character::digit:  scan_numeral (); break;
    case character::symbol: scan_symbol ();  break;
    default: 
      _token = token (UNKNOWN, s += c);
      get (d);                /* consume the unknown character */
      break;
    } 
  }
  return _token;                /* finally, return the token */
}

/* --------------------------------------------------------------------*/

unsigned int scanner::line () const {
  return _line;
}

/* --------------------------------------------------------------------*/

unsigned int scanner::column () const {
  return _column;
}

