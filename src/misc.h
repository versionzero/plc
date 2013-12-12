/*----------------------------------------------------------------------
  File    : misc.h
  Contents: 
  Author  : Ben Burnett
  History : 15.01.2007 file created
----------------------------------------------------------------------*/

#ifndef MISC_H
#define MISC_H

#include <utility>

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/

/* --- returns a count of the number of items in a fixed array */
#define count_of(x) (sizeof(x)/sizeof(x[0]))

/* --- maximum length for and identifier */
#define MAX_NAME_LEN  80

/* --------------------------------------------------------------------*/

template<typename T1, typename T2>
struct tie {
  T1 &_t1; T2 &_t2;
  tie ( T1 & t1, T2 & t2 ) : _t1 ( t1 ), _t2 ( t2 ) {}
  std::pair<T1, T2> const & operator = ( std::pair<T1, T2> const & bow ) { 
    _t1 = bow.first; _t2 = bow.second; }
};

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
