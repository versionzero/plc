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

  NONE = -1,
  END_OF_FILE,
  
  BEGIN, 
  END, 
  PROC, 
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
  EQUAL,                        // =
  GREATER_THAN,                 // >
  LESS_THAN,                    // <
  MULTIPLY,                     // *
  DIVIDE,                       // /
  MODULO,                       // '\'
  GUARD_SEPARATOR,              // []  
  
  ASSIGN,                       // :=  
  LAST_ASSIGN = ASSIGN,

  GUARD_POINT,                  // ->
  LAST_BINARY_OP = GUARD_POINT,
  LAST_OP = GUARD_POINT,
  
  RIGHT_PAREN,                  // )
  RIGHT_BRACKET,                // ]

  COMMENT,                      // $
  
  ARRAY, 
  BOOLEAN, 
  CONST,
  INTEGER 

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

  static std::string token_name ( token_code );
  
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
