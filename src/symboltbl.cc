/*----------------------------------------------------------------------
  File    : symboltbl.cc
  Contents: 
  Author  : Ben Burnett
  History : 10.01.2007 file created
            09.02.2007 revamped the internal structure; it now holds
	               a pointer to the current symbol table (current
		       in terms of scope) and provides a compleate
		       interface to an STL map.
	    19.03.2007 revamped AGAIN, now we use a list of maps, 
	               don't really know why I didn't do this in the 
		       first place!
----------------------------------------------------------------------*/

#if HAVE_CONFIG_H
#include <config.h>
#endif 

#include "symboltbl.h"
#include "misc.h"


#include<iostream>
using namespace std;

/*----------------------------------------------------------------------
  Namespace Inclusions
----------------------------------------------------------------------*/

using std::map;
using std::make_pair;
using std::pair;
using std::string;

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/

/* --- all the reserved words in PL programming language */
static struct {
  const char *word;
  token_code code;
} keywords[] = { 
  { "begin",   BEGIN },
  { "end",     END },
  { "const",   CONST },
  { "array",   ARRAY },
  { "integer", INTEGER },
  { "Boolean", BOOLEAN },
  { "proc",    PROC },
  { "skip",    SKIP },
  { "read",    READ },
  { "write",   WRITE },
  { "call",    CALL },
  { "if",      IF },
  { "do",      DO },
  { "fi",      FI },
  { "od",      OD },
  { "false",   FALSE },
  { "true",    TRUE }
};

/*----------------------------------------------------------------------
  Friend Functions/Operators
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Main Methods
----------------------------------------------------------------------*/

symboltbl::symboltbl () : _current ( _contents.begin () ) {
  push ();
  /* --- insert all the reserved words in to the symbol table */  
  for ( unsigned int i = 0; i < count_of ( keywords ); ++i ) {
    _current->insert ( make_pair ( keywords[i].word, 
				   token ( keywords[i].code, 
					   keywords[i].word ) ) );
  }  
}

/* --------------------------------------------------------------------*/

symboltbl::iterator symboltbl::begin () {
  return _current->begin ();
}

/* --------------------------------------------------------------------*/

symboltbl::const_iterator symboltbl::begin () const {
  return _current->begin ();
}

/* --------------------------------------------------------------------*/

symboltbl::iterator symboltbl::end () {
  return _current->end ();
}

/* --------------------------------------------------------------------*/

symboltbl::const_iterator symboltbl::end () const {
  return _current->end ();
}

/* --------------------------------------------------------------------*/

pair<symboltbl::iterator, bool> 
symboltbl::insert ( symboltbl::key_type const & k,
		    symboltbl::mapped_type const & m ) {  
  _current = _contents.begin ();  /* force to the current block */
  return _current->insert ( make_pair ( k, m ) );
}

/* --------------------------------------------------------------------*/

symboltbl::iterator symboltbl::find ( symboltbl::key_type const & k ) {
  /* --- look at this scope and all previous blocks */
  list_type::iterator it = _contents.begin ();
  for ( ; it != _contents.end (); ++it ) {
    map_type::iterator jt = it->find ( k );
    if ( jt != it->end () ) {
      _current = it;  /* so that users of our container can make an
			 assertion that jt != _current->end()  */
      return jt;
    }
  }  
  /* --- if we didn't find anything, then set it to the top-level 
     end() iterator. */
  _current = _contents.begin ();
  return _current->end ();
}

/* --------------------------------------------------------------------*/

symboltbl::iterator symboltbl::find_top ( symboltbl::key_type const & k ) {
  _current = _contents.begin ();  
  return _current->find ( k );
}

/* --------------------------------------------------------------------*/

void symboltbl::push () {
  _contents.push_front ( map_type () );
  _current = _contents.begin ();
}

/* --------------------------------------------------------------------*/

void symboltbl::pop () {
  if ( _contents.size () > 1 ) {
    _contents.pop_front ();
    _current = _contents.begin ();
  }
}

/* --------------------------------------------------------------------*/

int symboltbl::level () const {
  /* --- keywords are in the bottom level on their own and are not 
     really in their own scope, per say, so substract that from the
     scopre level */
  return _contents.size () - 1; 
}

/* --------------------------------------------------------------------*/

void symboltbl::push_storage ( int ) {  
}

/* --------------------------------------------------------------------*/

void symboltbl::pop_storage ( int ) {
}
