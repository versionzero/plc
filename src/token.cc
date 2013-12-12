/*----------------------------------------------------------------------
  File    : context.cc
  Contents: 
  Author  : Ben Burnett
  History : 10.01.2007 file created
----------------------------------------------------------------------*/

#if HAVE_CONFIG_H
#include <config.h>
#endif 

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
  "END_OF_FILE",
  "BEGIN", 
  "END", 
  "PROC", 
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
  "GUARD_SEPARATOR",              
  "ASSIGN",                       
  "GUARD_POINT",                  
  "RIGHT_PAREN",                  
  "RIGHT_BRACKET",                
  "COMMENT",                      
  "ARRAY", 
  "BOOLEAN", 
  "CONST",
  "INTEGER" 
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

std::string token::token_name ( token_code c ) {
  return string ( token_names[c] );
}
