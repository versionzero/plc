$$
$ File    : rc4.p
$ Contents: PL implementation of the RC4 stream generator
$ Author  : Ben Burnett
$ History : 15.02.2007 file created
$           18.02.2007 made the key repeat in T propoerly
$$

$$
$ RC4 - RC4 is a stream cipher designed by Ron Rivest (1987).
$
$ Source: Computer Science 3730: Cryptography [Howard Cheng]:
$  http://www.cs.uleth.ca/~cheng/courses/cs3730/slides/symmetric.pdf
$
$ Psudo-code :
$
$ for (i = 0; i < 256; i++)
$   S[i]=i;
$ j = 0;
$ for (i = 0; i < 256; i++)
$   j = (j + S[i] + T[i]) % 256;
$   swap(S[i], S[j]);
$ endfor
$ i = j = 0;
$ while (true) 
$   i = (i + 1) % 256;
$   j = (j + S[i]) % 256;
$   swap(S[i], S[j]);
$   t = (S[i] + S[j]) % 256;
$   print S[t]; // next byte of key stream
$ endwhile
$
$$

begin

   const max_length = 256;
   integer array S, T, K [256];
   integer i, j, t, key_length, tmp;

   $ Read the key from standard input
   proc read_key
   begin
      $ the first number tells the generator how many integers are in the key
      read key_length;
      i := 0;
      do ~ ( i = key_length ) ->
         read K[i];
         i := i + 1;
      od;
   end;

   $ Initialize the permutation stream
   proc init
   begin      
      i := 0;
      $ Also, if the key is shorter than 256, then it is repeated as many times
      $ as necessary until the buffer is full.
      do ~ ( i = 255 ) ->
         S[i] := i;
         T[i] := K[i \ key_length];
         i := i + 1;
      od;
      i := 0;
      do ~ ( i = 255 ) ->
         j    := (j + S[i] + T[i]) \ 256;      
         tmp  := S[i];
         S[i] := S[j];
         S[j] := tmp;
      od;
   end;

   $ To generate the next byte in the key stream, we choose the next byte
   $ according to the scheme below. NOTE: This routine will not terminate,
   $ so just Ctrl+C to end the program.
   proc print_key_stream
   begin
      i, j := 0, 0;
      do true ->
         i    := ( i + 1 ) \ 256;
         j    := ( j + S[i] ) \ 256;
         tmp  := S[i];
         S[i] := S[j];
         S[j] := tmp;
         t    := ( S[i] + S[j] ) \ 256;
         write S[t];
      od;
   end;

   $ Run RC4
   call read_key;
   call init;
   call print_key_stream;

end.

$$
$ EOF
$$
