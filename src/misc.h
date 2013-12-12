/*----------------------------------------------------------------------
  File    : misc.h
  Contents: 
  Author  : Ben Burnett
  History : 15.01.2007 file created
----------------------------------------------------------------------*/

#ifndef MISC_H
#define MISC_H

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/

/* --- returns a count of the number of items in a fixed array */
#define count_of(x) (sizeof(x)/sizeof(x[0]))

/* --- maximum length for and identifier */
#define MAX_NAME_LEN  80

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
