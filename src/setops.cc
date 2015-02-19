/*----------------------------------------------------------------------
  File    : setops.cc
  Contents: 
  Author  : Ben Burnett
  History : 21.02.2007 file created
----------------------------------------------------------------------*/

#if HAVE_CONFIG_H
#include <config.h>
#endif 

#include "setops.h"
#include <algorithm>
#include <cstdarg>
#include <iterator>

/*----------------------------------------------------------------------
  Namespace Inclusions
----------------------------------------------------------------------*/

using std::set;

/*----------------------------------------------------------------------
  Main Methods
----------------------------------------------------------------------*/

token_set operator + (token_set const & a, token_set const & b) {
  token_set c;
  set_union (a.begin (), a.end (), b.begin (), b.end (), 
	      inserter (c, c.begin ()));
  return c;
}

/* --------------------------------------------------------------------*/

token_set make_set_impl (int n, ...) {
  token_code i;
  va_list ap;
  token_set r;
  va_start (ap, n);
  while (n--) {
    i = static_cast<token_code> (va_arg (ap, int));
    r.insert (i);
  }
  va_end (ap);
  return r;
}

/* --------------------------------------------------------------------*/

token_set make_set (int a) {
  return make_set_impl (1, a); }
token_set make_set (int a, int b) {
  return make_set_impl (2, a, b); }
token_set make_set (int a, int b, int c) {
  return make_set_impl (3, a, b, c); }
token_set make_set (int a, int b, int c, int d) {
  return make_set_impl (4, a, b, c, d); }
token_set make_set (int a, int b, int c, int d, int e) {
  return make_set_impl (5, a, b, c, d, e); }
token_set make_set (int a, int b, int c, int d, int e, int f) {
  return make_set_impl (6, a, b, c, d, e, f); }
token_set make_set (int a, int b, int c, int d, int e, int f, int g) {
  return make_set_impl (7, a, b, c, d, e, f, g); }
token_set make_set (int a, int b, int c, int d, int e, int f, int g, int h) {
  return make_set_impl (8, a, b, c, d, e, f, g, h); }
token_set make_set (int a, int b, int c, int d, int e, int f, int g, int h, 
		     int i) {
  return make_set_impl (9, a, b, c, d, e, f, g, h, i); }
token_set make_set (int a, int b, int c, int d, int e, int f, int g, int h, 
		     int i, int j) {
  return make_set_impl (10, a, b, c, d, e, f, g, h, i, j); }


