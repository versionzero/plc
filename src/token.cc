/*----------------------------------------------------------------------
  File    : context.cc
  Contents: 
  Author  : Ben Burnett
  History : 10.01.2007 file created
----------------------------------------------------------------------*/

#if HAVE_CONFIG_H
#include <config.h>
#endif 

#include <cassert>
#include "token.h"

/*----------------------------------------------------------------------
  Namespace Inclusions
----------------------------------------------------------------------*/

using std::string;

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/

static const char *token_names[] = {
  "UNKNOWN",
  "END_OF_FILE",
  "BEGIN", 
  "END", 
  "SKIP", 
  "READ", 
  "WRITE", 
  "CALL", 
  "IF",
  "DO", 
  "FI", 
  "OD", 
  "SEMICOLON",                    
  "COMMA",                        
  "PERIOD",                       
  "FALSE", 
  "TRUE",
  "IDENTIFIER",
  "LEFT_PAREN",                   
  "LEFT_BRACKET",                 
  "LOGICAL_NOT",
  "PLUS",
  "MINUS",                        
  "LOGICAL_OR",                   
  "LOGICAL_AND",                  
  "EQUAL",                        
  "GREATER_THAN",                 
  "LESS_THAN",                    
  "MULTIPLY",                     
  "DIVIDE",                       
  "MODULO",                       
  "ASSIGN",                       
  "GUARD_SEPARATOR",                
  "GUARD_POINT",                  
  "RIGHT_PAREN",                  
  "RIGHT_BRACKET",                
  "COMMENT",                      
  "ARRAY", 
  "BOOLEAN", 
  "CONST",
  "INTEGER",
  "PROC", 
  "NUMBER"
};

/* --------------------------------------------------------------------*/

static const char *friendly_names[] = {
  "UNKNOWN",
  "EOF",
  "begin", 
  "end",   
  "skip", 
  "read", 
  "write", 
  "call", 
  "if",
  "do", 
  "fi", 
  "od", 
  ";",                    
  ",",                        
  ".",                       
  "false", 
  "true",
  "identifier",
  "(",                   
  "[",                 
  "~",
  "+",
  "-",                        
  "|",                   
  "&",                  
  "=",                        
  ">",                 
  "<",                    
  "*",                     
  "/",                       
  "\\",                       
  ":=",
  "[]",              
  "->",                  
  ")",                  
  "]",
  "comment",                      
  "array", 
  "Boolean", 
  "const",
  "integer",
  "proc", 
  "number"
};

/* --------------------------------------------------------------------*/

const token token::eof_token ( END_OF_FILE );
const token token::null ( NONE );

/*----------------------------------------------------------------------
  Helper Functions
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Main Methods
----------------------------------------------------------------------*/

token::token ( token_code c, string const & s ) 
  : _code ( c ), _svalue ( s ), _ivalue ( 0 ), _kind ( kind::undefined ), 
    _type ( type::universal ), _size ( 1 ), _level ( 0 ), 
    _displacement ( 0 ), _start ( -1 ) {
}

/* --------------------------------------------------------------------*/

token::token ( token_code c, int i ) 
  : _code ( c ), _svalue (), _ivalue ( i ), _kind ( kind::undefined ), 
    _type ( type::universal ), _size ( 1 ), _level ( 0 ), 
    _displacement ( 0 ), _start ( -1 ) {
}

/* --------------------------------------------------------------------*/

token::token ( token_code c, kind::code kind, type::code type,
	       std::string const & svalue, int ivalue, int size, int level, 
	       int displacement, int start ) 
  : _code ( c ), _svalue ( svalue ), _ivalue ( ivalue ), _kind ( kind ), 
    _type ( type ), _size ( size ), _level ( level ), 
    _displacement ( displacement ), _start ( start ) {
}

/* --------------------------------------------------------------------*/

void token::set_value ( std::string const & s ) {
  _svalue = s;
}

/* --------------------------------------------------------------------*/

void token::set_value ( int i ) {
  _ivalue = i;
}

/* --------------------------------------------------------------------*/

void token::set_type ( type::code t ) {
  _type = t;
}

/* --------------------------------------------------------------------*/

void token::set_kind ( kind::code k ) {
  _kind = k;
}

/* --------------------------------------------------------------------*/

void token::set_size ( int n ) {
  _size = n;
}

/* --------------------------------------------------------------------*/

void token::set_level ( int l ) {
  _level = l;
}

/* --------------------------------------------------------------------*/

void token::set_displacement ( int d ) {
  _displacement = d;
}

/* --------------------------------------------------------------------*/

void token::set_start ( int l ) {
  _start = l;
}

/* --------------------------------------------------------------------*/

token::operator token_code () const {
  return _code;
}

/* --------------------------------------------------------------------*/

void token::value ( string & s ) const {
  s = _svalue;
}

/* --------------------------------------------------------------------*/

void token::value ( int & i ) const {
  i = _ivalue;
}

/* --------------------------------------------------------------------*/

type::code token::type () const {
  return _type;
}

/* --------------------------------------------------------------------*/

kind::code token::kind () const {
  return _kind;
}

/* --------------------------------------------------------------------*/

int token::size () const {
  return _size;
}

/* --------------------------------------------------------------------*/
  
int token::level () const {
  return _level;
}

/* --------------------------------------------------------------------*/

int token::displacement () const {
  return _displacement;
}

/* --------------------------------------------------------------------*/

int token::start () const {
  return _start;
}

/* --------------------------------------------------------------------*/

const char* token::name ( token_code c ) {
  assert ( c >= FIRST_TOKEN && c <= LAST_TOKEN );
  return token_names[c];
}

/* --------------------------------------------------------------------*/

const char* token::friendly_name ( token_code c ) {
  assert ( c >= FIRST_TOKEN && c <= LAST_TOKEN );
  return friendly_names[c];
}
