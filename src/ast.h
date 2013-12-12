/*----------------------------------------------------------------------
  File    : ast.h
  Contents: 
  Author  : Ben Burnett
  History : 08.02.2007 file created
----------------------------------------------------------------------*/

#ifndef AST_H
#define AST_H

/*----------------------------------------------------------------------
  Forward Declarations
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Abstract Syntax Tree node
----------------------------------------------------------------------*/

class ast {

  int _column,                  /* node's column number */
      _line;                    /* node's line number */
  
public:

  ast ();
  
  int line () const;
  int column () const;

};

/*----------------------------------------------------------------------
  Expressions
----------------------------------------------------------------------*/

class expression : public ast {

public:

  expression ();

};

/*--------------------------------------------------------------------*/

class identifier : public expression {

public:

  identifier ();

};

/*--------------------------------------------------------------------*/

class constant : public expression {

public:

  constant ();

};

/*----------------------------------------------------------------------
  Operators
----------------------------------------------------------------------*/

class op : public expression {

public:

  op ();

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

public:

  access ();

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

#endif

/*----------------------------------------------------------------------
  Emacs Configuration
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
