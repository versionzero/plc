/*----------------------------------------------------------------------
  File    : symboltbl.h
  Contents: 
  Author  : Ben Burnett
  History : 10.01.2007 file created
----------------------------------------------------------------------*/

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "token.h"
#include <list>
#include <map>
#include <string>

/*----------------------------------------------------------------------
  Forward Declarations
----------------------------------------------------------------------*/

class token;

/*----------------------------------------------------------------------
  Main Class - note we don't have all those proper things like copy 
  constructors and operators, this is because we will never make 
  more than one of these.  Of course we could enforce this strictly
  by making it a singleton of some sort ... but I'm just not too 
  concerned about that right now
----------------------------------------------------------------------*/
 
class symboltbl {

private:
  
  typedef std::map<std::string, token> map_type;  
  typedef std::list<map_type>          list_type;

  list_type                   _contents;  /* list of all the symbols */
  mutable list_type::iterator _current;   /* current scope  */  
  
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
  
  iterator begin ();
  const_iterator begin () const;
  iterator end ();
  const_iterator end () const;

  std::pair<iterator, bool> insert (key_type const &, mapped_type const &);  

  iterator find (key_type const &); 
  iterator find_top (key_type const &);   
  /* notice there is no const version of the above, we will implement it 
     when -- and if -- it become nessessary (which I hope it doesn't) */

  void push ();                 /* create a new scope (or block) */
  void pop ();                  /* close the current scope */  
  int  level () const;          /* current # of open scopes */

  void push_storage (int);
  void pop_storage (int);

};

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
