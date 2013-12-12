/*----------------------------------------------------------------------
  File    : symboltbl.cc
  Contents: 
  Author  : Ben Burnett
  History : 10.01.2007 file created
            09.02.2007 revamped the internal structure; it now holds
	               a pointer to the current symbol table (current
		       in terms of scope) and provides a compleate
		       interface to an STL map.
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

symboltbl::symboltbl () 
  : _owned ( new environment () ) {    
  /* --- insert all the reserved words in to the symbol table */
  for ( unsigned int i = 0; i < count_of ( keywords ); ++i ) {
    insert ( keywords[i].word, token ( keywords[i].code, 
				       keywords[i].word ) );
  }
  _curr = _owned;
}

/* --------------------------------------------------------------------*/

symboltbl::~symboltbl () {
  pop ();
}

/* --------------------------------------------------------------------*/

void symboltbl::copy ( symboltbl const & other ) {
  _owned->prev = other._owned->prev;  
  _owned->symbols = other._owned->symbols;
  _curr = _owned;
}

/* --------------------------------------------------------------------*/

symboltbl::symboltbl ( symboltbl const & other )
  : _owned ( new environment () ), _curr ( 0 ) {
  copy ( other );  
}

/* --------------------------------------------------------------------*/

symboltbl& symboltbl::operator = ( symboltbl const & other ) {
  copy ( other );
  return *this;
}

/* --------------------------------------------------------------------*/

symboltbl::iterator symboltbl::begin () {
  return _curr->symbols.begin ();
}

/* --------------------------------------------------------------------*/

symboltbl::const_iterator symboltbl::begin () const {
  return _curr->symbols.begin ();
}

/* --------------------------------------------------------------------*/

symboltbl::iterator symboltbl::end () {
  return _curr->symbols.end ();
}

/* --------------------------------------------------------------------*/

symboltbl::const_iterator symboltbl::end () const {
  return _curr->symbols.end ();
}

/* --------------------------------------------------------------------*/

bool symboltbl::insert ( symboltbl::key_type const & k,
			 symboltbl::mapped_type const & m ) {
  return ( _owned->symbols.insert ( make_pair ( k, m ) ).second );
}

/* --------------------------------------------------------------------*/

symboltbl::iterator symboltbl::find ( symboltbl::key_type const & k ) {
  /* --- look at this scope and all 'parent' ones */
  symboltbl::iterator it;
  for ( _curr = _owned; 0 != _curr; _curr = _curr->prev ) {
    it = _curr->symbols.find ( k );
    if ( it != _curr->symbols.end () ) {
      return it;
    }
  }
  /* --- if we didn't find anything, then set it to the end() iterator */
  _curr = _owned;
  return _owned->symbols.end ();
}

/* --------------------------------------------------------------------*/

symboltbl::const_iterator 
symboltbl::find ( symboltbl::key_type const & k ) const {
  /* --- look at this scope and all 'parent' ones */
  symboltbl::const_iterator it;
  for ( _curr = _owned; 0 != _curr; _curr = _curr->prev ) {
    it = _curr->symbols.find ( k );
    if ( it != _curr->symbols.end () ) {
      return it;
    }
  }
  /* --- if we didn't find anything, then set it to the top level end()
     iterator (actually, it's the botton, since we are a tree and the
     top is a leaf node). */
  _curr = _owned;
  return _owned->symbols.end ();
}

/* --------------------------------------------------------------------*/

void symboltbl::push () {
  _owned = new environment ( _owned );
  _curr = _owned;
}

/* --------------------------------------------------------------------*/

void symboltbl::pop () {
  if ( _owned ) {
    environment *tmp = _owned;
    _owned = _owned->prev;
    delete tmp;
    _curr = _owned;
  }
}
