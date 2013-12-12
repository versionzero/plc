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

const token eof_token ( END_OF_FILE );

/*----------------------------------------------------------------------
  Helper Functions
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Main Methods
----------------------------------------------------------------------*/

token::token ( token_code c, string const & s ) 
  : _code ( c ), _value ( s ) {
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
