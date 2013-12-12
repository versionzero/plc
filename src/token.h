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
  WORD,

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
  INDEX,

  LAST_TOKEN
};

/*----------------------------------------------------------------------
  Kind Codes
----------------------------------------------------------------------*/

namespace kind { 
  enum code { 
    constant,  
    variable, 
    array,
    procedure,
    undefined
   }; 
}; 

/*----------------------------------------------------------------------
  Type Codes
----------------------------------------------------------------------*/

namespace type {
  enum code {
    universal,
    boolean = BOOLEAN,
    integer = INTEGER
  };
};

/*----------------------------------------------------------------------
  Main Class
----------------------------------------------------------------------*/

class token {  
  
 private:
  
  token_code  _code;  
  std::string _value;
  kind::code  _kind;
  type::code  _type;

 public:

  token ( token_code, std::string const & );  
  token ( token_code = NONE, 
	  kind::code = kind::undefined,
	  type::code = type::universal, 	  
	  std::string const & = std::string () );  

  void set_value ( std::string const & );
  void set_type ( type::code );
  void set_kind ( kind::code );
  
  operator token_code () const;   
  std::string value () const;
  type::code type () const;
  kind::code kind () const;

  static const char* name ( token_code );
  static const char* friendly_name ( token_code );
  static const token null;
  
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
