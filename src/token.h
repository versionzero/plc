/*----------------------------------------------------------------------
  File    : token.h
  Contents: 
  Author  : Ben Burnett
  History : 10.01.2007 file created
----------------------------------------------------------------------*/

#ifndef TOKEN_H
#define TOKEN_H

#include <string>

/*----------------------------------------------------------------------
  Token Codes
----------------------------------------------------------------------*/

enum token_code {

  FIRST_TOKEN = -1,
  
  NONE = FIRST_TOKEN,
  UNKNOWN,
  END_OF_FILE,
  
  BEGIN, 
  END, 
  SKIP, 
  READ, 
  WRITE, 
  CALL, 
  IF,
  DO, 
  FI, 
  OD, 

  SEMICOLON,                    // ;
  COMMA,                        // ,
  PERIOD,                       // .

  FALSE, 
  TRUE,
  IDENTIFIER,

  LEFT_PAREN,                   // (
  LEFT_BRACKET,                 // [

  FIRST_OP,  
  LOGICAL_NOT = FIRST_OP,       // ~
  
  FIRST_BINARY_OP,  
  PLUS = FIRST_BINARY_OP,       // +
  MINUS,                        // -
  LOGICAL_OR,                   // |
  LOGICAL_AND,                  // &
  EQUAL,                        // =   (25)
  GREATER_THAN,                 // >
  LESS_THAN,                    // <
  MULTIPLY,                     // *
  DIVIDE,                       // /
  MODULO,                       // '\'
  ASSIGN,                       // :=    

  GUARD_SEPARATOR,              // []    
  GUARD_POINT,                  // ->
    
  RIGHT_PAREN,                  // )
  RIGHT_BRACKET,                // ]

  COMMENT,                      // $
  
  ARRAY, 
  BOOLEAN, 
  CONST,
  INTEGER,
  PROC, 
  NUMBER,

  LAST_TOKEN
};

/*----------------------------------------------------------------------
  Main Class
----------------------------------------------------------------------*/

class token {  
  
 private:
  
  token_code  _code;  
  std::string _value;

 public:

  token ( token_code = NONE, std::string const & = std::string () );  
  operator token_code () const; 
  std::string value () const;

  static const char* name ( token_code );
  static const char* friendly_name ( token_code );
  
};

/* --------------------------------------------------------------------*/

extern const token eof_token;

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
