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
  "WORD",
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
  "NUMBER",
  "INDEX"
};

/* --------------------------------------------------------------------*/

static const char *friendly_names[] = {
  "unknown",
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
  "word",
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
  "number",
  "index"
};

/* --------------------------------------------------------------------*/

const token eof_token ( END_OF_FILE );
const token token::null ( NONE );

/*----------------------------------------------------------------------
  Helper Functions
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Main Methods
----------------------------------------------------------------------*/

token::token ( token_code c, string const & s ) 
  : _code ( c ), _value ( s ), _kind ( kind::undefined ), 
    _type ( type::universal ) {
}

/* --------------------------------------------------------------------*/

token::token ( token_code c, kind::code k, type::code t, string const & s ) 
  : _code ( c ), _value ( s ), _kind ( k ), _type ( t ) {
}

/* --------------------------------------------------------------------*/

void token::set_value ( std::string const & s ) {
  _value = s;
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

token::operator token_code () const {
  return _code;
}

/* --------------------------------------------------------------------*/

std::string token::value () const {
  return _value;
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

const char* token::name ( token_code c ) {
  assert ( c >= FIRST_TOKEN && c <= LAST_TOKEN );
  return token_names[c];
}

/* --------------------------------------------------------------------*/

#include <iostream>
using std::cout;

const char* token::friendly_name ( token_code c ) {
  assert ( c >= FIRST_TOKEN && c <= LAST_TOKEN );
  return friendly_names[c];
}
