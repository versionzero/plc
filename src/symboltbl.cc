/*----------------------------------------------------------------------
  File    : symboltbl.cc
  Contents: 
  Author  : Ben Burnett
  History : 10.01.2007 file created
----------------------------------------------------------------------*/

#if HAVE_CONFIG_H
#include <config.h>
#endif 

#include "symboltbl.h"
#include "misc.h"

/*----------------------------------------------------------------------
  Namespace Inclusions
----------------------------------------------------------------------*/

using std::map;
using std::string;
using std::make_pair;

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/

/* --- all the reserved words in PL */
static struct {
  char *word;
  token_code code;
} keywords[] = { 
  { "begin", BEGIN },
  { "end", END },
  { "const", CONST },
  { "array", ARRAY },
  { "integer", INTEGER },
  { "Boolean", BOOLEAN },
  { "proc", PROC },
  { "skip", SKIP },
  { "read", READ },
  { "write", WRITE },
  { "call", CALL },
  { "if", IF },
  { "do", DO },
  { "fi", FI },
  { "od", OD },
  { "false", FALSE },
  { "true", TRUE }
};

/*----------------------------------------------------------------------
  Friend Functions/Operators
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Main Methods
----------------------------------------------------------------------*/

symboltbl::symboltbl () {  
  /* --- insert all the reserved words in to the symbol table */
  for ( unsigned int i = 0; i < count_of ( keywords ); ++i ) {
    insert ( keywords[i].word, token ( keywords[i].code, 
				       keywords[i].word ) );
  }
}

/* --------------------------------------------------------------------*/

bool symboltbl::insert ( string const & s, token const & t ) {
  return ( _table.insert ( make_pair ( s, t ) ).second );
}

/* --------------------------------------------------------------------*/
 
bool symboltbl::exists ( string const & s ) const {
  return ( _table.find ( s ) != _table.end () );
}

/* --------------------------------------------------------------------*/

token const & symboltbl::get ( std::string const & s ) const {
  return (*_table.find ( s )).second;
}
