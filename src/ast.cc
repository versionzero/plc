/*----------------------------------------------------------------------
  File    : ast.cc
  Contents: 
  Author  : Ben Burnett
  History : 08.02.2007 file created
----------------------------------------------------------------------*/

#if HAVE_CONFIG_H
#include <config.h>
#endif 

#include "ast.h"

/*----------------------------------------------------------------------
  Namespace Inclusions
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Friend Functions/Operators
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Exceptions
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Main Methods
----------------------------------------------------------------------*/

ast::ast () {
}

/*--------------------------------------------------------------------*/

int ast::line () const {
  return _line;
}
 
/*--------------------------------------------------------------------*/

int ast::column () const {
  return _column;
}

/*--------------------------------------------------------------------*/


