/*----------------------------------------------------------------------
  File    : symboltbl.h
  Contents: 
  Author  : Ben Burnett
  History : 10.01.2007 file created
----------------------------------------------------------------------*/

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "token.h"
#include <map>
#include <string>

/*----------------------------------------------------------------------
  Forward Declarations
----------------------------------------------------------------------*/

class token;

/*----------------------------------------------------------------------
  Main Class
----------------------------------------------------------------------*/
 
class symboltbl {

  
private:
  
  typedef std::map<std::string, token> map_type;
  
  struct environment {
    environment ( environment *p = 0 ) : prev ( p ) {}
    environment *prev;
    map_type    symbols;
  };
  
  environment* _owned;
  mutable environment* _curr;
  
  void copy ( symboltbl const & );
  
public:  

  typedef map_type::key_type           key_type;
  typedef map_type::mapped_type        mapped_type;
  typedef map_type::value_type         value_type;
  typedef map_type::reference          reference;
  typedef map_type::const_reference    const_reference;
  typedef map_type::iterator           iterator;
  typedef map_type::size_type          size_type;
  typedef map_type::const_iterator     const_iterator;
    
  symboltbl ();
  virtual ~symboltbl ();
  symboltbl ( symboltbl const & );
  
  symboltbl & operator = ( symboltbl const & );

  iterator begin ();
  const_iterator begin () const;
  iterator end ();
  const_iterator end () const;

  bool insert ( key_type const &, mapped_type const & );  

  iterator find ( key_type const & );
  const_iterator find ( key_type const & ) const;

  void push ();                 /* --- create a new scope */
  void pop ();                  /* --- close the current scope */

};

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
