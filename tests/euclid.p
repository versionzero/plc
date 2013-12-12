
begin
   integer X, Y, x, y;
   proc euclid begin      
      do ~( x = y ) -> do x > y -> x := x - y;
                       [] y > x -> y := y - x; od;                  
      od;
   end;
   read X, Y;
   x, y := X, Y;
   call euclid;
   write x, y;
end.
   
