
begin
   integer x, y, z;
   integer array a[100];
   a[true] := 10; $ error! integer expression expected
   read y, z;
   x := y * z / 5;
   write x;
end.
