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
  Type Declarations
----------------------------------------------------------------------*/
 
class symboltbl {

 private:

  std::map<std::string, token> _table;

 public:  
  
  symboltbl ();

  bool insert ( std::string const &, token const & );
  bool exists ( std::string const & ) const;
  token const & get ( std::string const & ) const;

};

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
