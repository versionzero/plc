/*----------------------------------------------------------------------
  File    : setops.h
  Contents: set of useful set operations for building first, follow
             and stop sets
  Author  : Ben Burnett
  History : 18.02.2007 file created
            21.02.2007 added make_set function to build sets on the 
	               fly.
----------------------------------------------------------------------*/

#ifndef SET_OPERATIONS_H
#define SET_OPERATIONS_H

#include "token.h"
#include <set>

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/

typedef std::set<token_code> token_set;

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

/* -- enable the creating unions of two set by adding them */
token_set operator + (token_set const & a, token_set const & b);

/* --------------------------------------------------------------------*/

/* -- set builder implementation: all other set builder functions
   call this on internally.  It takes a variable number of arguments
   of type T and returns a set */ 
token_set make_set_impl (int n, ...);

/* -- build sets of varible length */
token_set make_set (int a);
token_set make_set (int a, int b);
token_set make_set (int a, int b, int c);
token_set make_set (int a, int b, int c, int d);
token_set make_set (int a, int b, int c, int d, int e);
token_set make_set (int a, int b, int c, int d, int e, int f);
token_set make_set (int a, int b, int c, int d, int e, int f, int g);
token_set make_set (int a, int b, int c, int d, int e, int f, int g, int h);
token_set make_set (int a, int b, int c, int d, int e, int f, int g, int h, 
		     int i);
token_set make_set (int a, int b, int c, int d, int e, int f, int g, int h, 
		     int i, int j);

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
