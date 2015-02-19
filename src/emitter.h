/*----------------------------------------------------------------------
  File    : emitter.h
  Contents: 
  Author  : Ben Burnett
  History : 06.04.2007 file created
----------------------------------------------------------------------*/

#ifndef EMITTER_H
#define EMITTER_H

#include <string>

/*----------------------------------------------------------------------
  Forward Declarations
----------------------------------------------------------------------*/

class label;

/*----------------------------------------------------------------------
  Emitter interface
----------------------------------------------------------------------*/

struct emitter_interface {

protected:
  
  virtual void emit (std::string const&) = 0;  
  virtual void emit (std::string const&, int) = 0;  
  virtual void emit (std::string const&, int , int) = 0;  

public:

  virtual ~emitter_interface ();

  void add (); 
  void and$ (); 
  void arrow (int); 
  void assign (int); 
  void bar (int); 
  void call (int, int); 
  void constant (int);
  void divide (); 
  void end_procedure (); 
  void end_program (); 
  void equal (); 
  void fi (int);
  void greater ();
  void index (int, int);
  void less (); 
  void minus (); 
  void modulo (); 
  void multiply (); 
  void not$ ();
  void or$ (); 
  void procedure (int, int); 
  void program (int, int); 
  void read (int); 
  void subtract (); 
  void value ();
  void variable (int, int); 
  void write (int);

  void define_address (int);
  void define_argument (int, int);

};

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
