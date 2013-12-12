/*----------------------------------------------------------------------
  File    : ast.h
  Contents: 
  Author  : Ben Burnett
  History : 08.02.2007 file created
----------------------------------------------------------------------*/

#ifndef AST_H
#define AST_H

#include "token.h"

namespace ast {

/*----------------------------------------------------------------------
  Forward Declarations
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Abstract Syntax Tree node
----------------------------------------------------------------------*/

class ast {

  friend class parser;          /* all derived class will now be friendly 
				   with the parser ... that is, they'll
				   show it their protected members */

protected:
 
  int        _column,           /* node's column number */
             _line;             /* node's line number */
  
public:

  ast ();
  
  int line () const;
  int column () const;

};

/*----------------------------------------------------------------------
  Expressions
----------------------------------------------------------------------*/

class expression : public ast {

protected:
  
  token      _op;
  type::code _type;
  
public:

  expression ( token const & = token (), type::code = type::universal );
  
  type::code type () const;
  void set_type ( type::code );

};

/*--------------------------------------------------------------------*/

class identifier : public expression {

private:
  
  int _offset;
  
public:

  identifier ( token const &, type::code, int );
  /* -- might be able to get type from the token */

  operator int () const;

};

/*--------------------------------------------------------------------*/

class constant : public expression {

public:
  
  constant ( token const &, type::code = type::universal );

  static constant true$;
  static constant false$;

  void set_value ( token const & );

};

/*----------------------------------------------------------------------
  Operators
----------------------------------------------------------------------*/

class op : public expression {

public:

  op ( token const &, type::code );

};

/*--------------------------------------------------------------------*/

class binary : public op {

public:

  binary ();

};

/*--------------------------------------------------------------------*/

class unary : public op {

public:

  unary ();

};

/*--------------------------------------------------------------------*/

class access : public op {

private:

  identifier _array;
  expression _index;

public:

  access ( identifier, expression, type::code );

  identifier const & get_array () const;
  expression const & get_index () const;

};

/*----------------------------------------------------------------------
  Logical operators
----------------------------------------------------------------------*/

class logical : public expression {

public:

  logical ();

};

/*--------------------------------------------------------------------*/

class land : public logical {

public:

  land ();

};

/*--------------------------------------------------------------------*/

class lor : public logical {

public:

  lor ();

};

/*--------------------------------------------------------------------*/

class lnot : public logical {

public:

  lnot ();

};

/*--------------------------------------------------------------------*/

class relational : public logical {

public:

  relational ();

};

/*----------------------------------------------------------------------
  Statements

  Statement = EmptyStatement | ReadStatement | WriteStatement 
            | Assignmentstatement | ProcedureStatement | IfStatement 
	    | DoStatement .
----------------------------------------------------------------------*/

class statement : public ast {

public:

  statement ();

};

/*--------------------------------------------------------------------*/

class empty : public statement {

public:

  empty ();

};

/*--------------------------------------------------------------------*/

class if_statement : public statement {

public:

  if_statement ();

};

/*--------------------------------------------------------------------*/

class do_statement : public statement {

public:

  do_statement ();

};

/*--------------------------------------------------------------------*/

class read : public statement {

public:

  read ();

};

/*--------------------------------------------------------------------*/

class write : public statement {

public:

  write ();

};

/*--------------------------------------------------------------------*/

class assignment : public statement {

public:

  assignment ();

};

/*--------------------------------------------------------------------*/

class procedure : public statement {

public:

  procedure ();

};

}

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
