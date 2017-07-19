## Introduction

The Project Language Compiler (or PLC, as we shall refer to it from here on 
in) is a basic "Project Language" compiler designed to meet the requirements
for CPSC4600 at the University of Lethbridge.

The current version of the compiler is written with out the aid of scanner or
parser generating tools; however, this may change in future versions, as the
project develops.

### Scanning Phase (now outdated)

The current version of the compiler does not do much, other than scan an input 
stream, and output tokens as it seems them.  The output takes the following 
form:
 
    <filename.p>:<line#>[<column#>]: <TOKEN ID>: <token value

For instance, the following code:

    begin
       const num = 10;
       integer x, y;
       proc max
       $ this is a comment!
       begin
          if x > y -> write x. [] ~(x > y) -> write y.
          fi
       end
    end. 

generates:

    ...
    plc:2[0]: BEGIN: begin
    plc:3[3]: CONST: const
    plc:3[2]: IDENTIFIER: num
    plc:3[9]: EQUAL: =
    plc:3[10]: INTEGER: 10
    plc:3[9]: SEMICOLON: ;
    plc:4[3]: INTEGER: integer
    plc:4[2]: IDENTIFIER: x
    plc:4[8]: COMMA: ,
    plc:4[5]: IDENTIFIER: y
    plc:4[7]: SEMICOLON: ;
    plc:5[3]: PROC: proc
    plc:5[1]: IDENTIFIER: max
    ...

as it's output.  Yup, that's about as special as it gets for now.

### Parsing Phase (outdated)

The compiler now includes a parser module.  This module uses the previously
created scanner module to syntactically analyze the input.  In this module
we attempt to validate the syntactic structure of the code, and to output
location information when a violation of the language is encountered.

Like the scanner phase, we have some phase specific output.  Unlike the 
previous phase, the project can be configured to suppress this debug output.
Disabled by default, the debug information can be enabled by suppling 
--enable-debug=true as a parameter to the configure script.  What this option
does is allow the code to spit out parse tree information.  So, while there
is no actual parse tree maintained in the application itself, it can be 
observed during runtime as the parser descends into the parse tree.

### Type Checking Phase (outdated)

After a great deal of sweat, tears and lost hair the compiler now has type
checking.  There were a number of changes that took place.  First, the 
scanner was simplified and all references to the symbol table were removed
from it.  This responsibility was moved in to the parser to increase its
flexibility.  Additional error handling was added to handle type errors, as 
well as the new semantic errors, such as duplicate symbol declarations within
the same scope.

First, lets look at some type definitions:

     1  begin
     2  integer x;
     3  integer array q[10];
     4  integer y, z;
     5  integer array r[ 10 ];
     6  integer u, v, w;
     7  integer w;
     8  Boolean b;
     9  const c = 10;   $ ok!
    10  const d = true; $ ok!
    11  const e = d;    $ ok!
    12  const f = v;    $ v is non-constant
    13  const g = f;    $ ok! - but only silently ok
    14  const h = g;    $ ok!
    15  const i = ggg;  $ ggg not defined
    16  const j = j;    $ this *should* be an error
    17  const x = 10;   $ already an integer
    18  end.

will produce the following lovely output:

    plc 0.3 - PL language compiler [debug build] (ben.burnett@gmail.com)
    ...
    vartest2.p:11: error: non-constant `v' used in a constant expression
    vartest2.p:14: error: undefined symbol `ggg'
    vartest2.p:14: error: expected `identifier' before `ggg' token
    vartest2.p:15: error: undefined symbol `j'
    vartest2.p:15: error: expected `identifier' before `j' token
    vartest2.p:16: error: duplicate symbol `x'
    ...

Now for procedure definitions:

     1
     2  begin
     3
     4     integer i;
     5     Boolean b;
     6     const c = 10;
     7       
     8     proc f begin
     9        proc g begin
    10           write 7;
    11        end;
    12        call b;
    13        call g;
    14        write 6;
    15     end;
    16        
    17     proc h begin
    18        write 9;
    19        call f;
    20     end;
    21
    22     proc l begin
    23        write 10;
    24        call i;   $ error! not a proc
    25     end;
    26
    27     call c;
    28     call h;
    29     call g; $ error! not in the correct scope
    30     write 5;
    31
    32  end.

will produce the following output:

    plc 0.3 - PL language compiler [debug build] (ben.burnett@gmail.com)
    ...
    proctest3.p:24: error: `i' is not a procedure
    proctest3.p:27: error: `c' is not a procedure
    proctest3.p:29: error: undefined symbol `g'
    proctest3.p:29: error: expected `identifier' before `g' token
    ...

We also catch mismatched assignment statement.  By this we mean that if one
side of the assignment statement contains more items than the other, then the
user will be alerted to this fact.

     1
     2  begin
     3     integer x, y, z;
     4     Boolean b, c, d;
     5     x, y := 1, 2, 3; $ error! rhs is heavy
     6     x, y, z := 1, 2; $ error! lhs is heavy
     7     b, c, d := true, false, 1; $ error! type error
     8     write x;
     9  end.

will produce the following output:

    plc 0.3 - PL language compiler [debug build] (ben.burnett@gmail.com)
    ...
    unbalanced.p:5: error: unbalanced assignment statement; rhs is heavy
    unbalanced.p:6: error: unbalanced assignment statement; lhs is heavy
    unbalanced.p:7: error: type mismatch

See the design section for further information.

## How to build the PLC 

To build the project, simply run:

    # ./configure
    # make -j
   
This will configure and build PLC.  Optionally, one can add the 
--enable-debug=true flag to the ./configure line: this will enable trace
information about the parsing state. The -j flag for make compiles multiple 
files in parallel (helpful if you have distcc, a dual-core or a dual-processor
machine). Like many GNU projects, you can installthe compiler locally, using 
the following:

    # make install

Alternatively, you can run it with no installation:

    # cd src
    # ./plc ../tests/everything.p

The above will run the PLC using everything.p as the input source file. Also 
in the tests directory are other test files.  One is called unknown.p, which 
has a syntax error in it.  This file demonstrates the error handling built in 
to the PLC.

### Code Generation Phase

Very little has changed in this phase with regards to the overall design.  The 
main changes have taken place in the parser itself and take the form of minor
code modifications.  The modifications simply output assembler code.  This 
assembler code is then consumed by the assembler that we were given and is
in turn converted in to machine code which is the final product of our 
compiler.

The biggest hurdle of this phase was integrating other peoples code into my own.
There is always the temptation to re-write or re-factor other peoples work to
fit your own style or desired functionality.  However, I stopped myself, and 
worked with what we were given.  The result is a fairly nice system anyway.
I used a stringstream as an intermediary stream, so the assembler my compiler
generates is output into an in/out stringstream, which is then used as an
input stream for the assembler.  The eliminated the need to clean up any
temporary files, etc.  I also made the output file optional, so the machine 
code can be spit out on stdout, or redirected to a file, if a filename is
supplied.  To see the options, simply run the program with no options.

## Design

The design of the PLC is derived from many sources.  The initial skeleton code
was based on a previous project for Bioinformatics.  This provided the code
for file-handling and command-line processing, as well as some error handling 
methods.  However, by the end of this project, most of this initial code will
have been dispensed with.  It was only used to speed up the process of codeing.

### Scanner Phase

The error handling code, for instance, does not scale well, and will be 
replaced with a new system.  This new system will use C++ exceptions and
will allow a great deal of the error handling to be moved out of the logic
of the compiler.  This will simplify any future changes in the scanner as 
well as the compiler itself.  Also, it should improve readability, by removing
all the current checks.

The second phase of the project, once we start adding the parser, will bring 
a new top level control systems as well.  Currently, the symbol table, as well
as the scanner itself, is "owned" by the main function.  In the future, there
will be a top level compiler object, which will take ownership of the symbol
table, the scanner, as well as the parser.  This will be useful, as well, once
the code generation phase is upon us; then, it will also own a code generation
object.

### Parser Phase

In this phase we have enhanced and, in some cases, simplified  the error 
handling.  This means that not only is the previous error information 
emitted, but information to help determine the type of error is also emitted.
Thus, for example, when we expect to see a semicolon (';') but instead receive
a comma (',') in the input, we can tell the user that a comma was seen in place
of a semicolon, on line n.  This will help the user in correcting their code,
versus only being told there is some syntax error on line n.

### Type and Scope Checking Phase

The biggest issue with this phase was making the decisions between design
and efficiency.  Because of previous design decisions, some of the addition
that were required would either require additional searches (in the symbol
table), or the addition of "spaghetti" code to remember information temporarily.
I chose the former because the actual efficiency can be improved by changing 
the data structure.  Currently I use a red-black tree for the symbol table; 
however, a hash table would allow constant time access, making the above 
searching problem a non-issue.  Therefore, for the benefit of myself, and 
anyone who decides to read this code, I've opted for the simpler re-search 
approach.  In the future I may change the data structure to get rid of 
the O(log n) search time.

## Structure

### Classes

Scanner Phase:

* scanner	       actual scanning mechanisms
* scanner_error	       used to represent errors specific to scanning 
** unknown_symbol      an error that can be encountered while scanning;
   		       namely, when we encounter a symbol we do not 
		       recognize
* symboltbl            symbol table used by the compiler
* token		       holds all the information about a single token

Parser Phase:

* parser	       new parser module to help with syntactic analysis
* setops 	       set operation helpers: most of this is syntactic
  		       sugar, meaning that it supplies, for instance, a
		       a plus ('+') operator to return a set union and 
		       a set of functions to make sets in-place from a set
		       of parameters (not necessarily unique)
* scanner	       actual scanning mechanisms
* symboltbl            symbol table used by the compiler
* token		       holds all the information about a single token

### Files

Scanner Phase:

* src/error.{cc,h}     these files contain all the error handling and recovery
  		       code.  
* src/misc.{cc,h}      code that does not fit nicely in to any other place
  		       is kept here
* src/plc.{cc,h}       these contain the compiler's main entry point, as well
  		       as any globaly accessible structures
* src/scanner.{cc,h}   in here is all the code for scanning an input stream
  		       and converting character streams in to token streams
* src/symboltbl.{cc,h} an implementation of a symbol table
* src/token.{cc,h}     contains all the code related to tokens, all their
  		       unique IDs, as well as string representations of the
		       tokens themselves.

Parser Phase:

* src/error.{cc,h}     these files contain all the error handling and recovery
  		       code.  
* src/misc.h           code that does not fit nicely in to any other place
  		       is kept here
* src/parser.{cc,h}    these files contain the parser module
* src/plc.{cc,h}       these contain the compiler's main entry point, as well
  		       as any globaly accessible structures
* src/scanner.{cc,h}   in here is all the code for scanning an input stream
  		       and converting character streams in to token streams
* src/setops.{cc,h}    set operations and enhancement
* src/symboltbl.{cc,h} an implementation of a symbol table
* src/token.{cc,h}     contains all the code related to tokens, all their
  		       unique IDs, as well as string representations of the
		       tokens themselves.
