PROG 2 1
DEFADDR 3
PROC 4 5
DEFARG 4 1
DEFADDR 5
VARIABLE 1 14
VARIABLE 0 3
CONSTANT 1
CONSTANT 10
ASSIGN 2
DEFADDR 7
DEFADDR 6
VARIABLE 1 14
VALUE
VARIABLE 0 3
VALUE
LESS
ARROW 8
DEFADDR 9
VARIABLE 1 3
VARIABLE 1 14
VALUE
INDEX 10 10
VALUE
VARIABLE 1 13
VALUE
EQUAL
ARROW 11
VARIABLE 0 3
VARIABLE 1 14
VALUE
ASSIGN 1
BAR 10
DEFADDR 11
VARIABLE 1 3
VARIABLE 1 14
VALUE
INDEX 10 11
VALUE
VARIABLE 1 13
VALUE
EQUAL
NOT
ARROW 12
VARIABLE 1 14
VARIABLE 1 14
VALUE
CONSTANT 1
ADD
ASSIGN 1
BAR 10
DEFADDR 12
FI 12
DEFADDR 10
BAR 7
DEFADDR 8
VARIABLE 1 15
VARIABLE 1 3
VARIABLE 1 14
VALUE
INDEX 10 14
VALUE
VARIABLE 1 13
VALUE
EQUAL
ASSIGN 1
ENDPROC
DEFARG 2 13
DEFADDR 1
VARIABLE 0 14
CONSTANT 1
ASSIGN 1
DEFADDR 14
DEFADDR 13
VARIABLE 0 14
VALUE
CONSTANT 10
GREATER
NOT
ARROW 15
VARIABLE 0 3
VARIABLE 0 14
VALUE
INDEX 10 18
READ 1
VARIABLE 0 14
VARIABLE 0 14
VALUE
CONSTANT 1
ADD
ASSIGN 1
BAR 14
DEFADDR 15
VARIABLE 0 13
READ 1
DEFADDR 17
DEFADDR 16
VARIABLE 0 13
VALUE
CONSTANT 0
EQUAL
NOT
ARROW 18
CALL 0 3
DEFADDR 19
VARIABLE 0 15
VALUE
ARROW 21
VARIABLE 0 13
VALUE
VARIABLE 0 14
VALUE
WRITE 2
BAR 20
DEFADDR 21
VARIABLE 0 15
VALUE
NOT
ARROW 22
VARIABLE 0 13
VALUE
WRITE 1
BAR 20
DEFADDR 22
FI 25
DEFADDR 20
VARIABLE 0 13
READ 1
BAR 17
DEFADDR 18
ENDPROG
