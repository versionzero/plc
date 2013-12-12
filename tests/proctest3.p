
begin

   integer i;
   Boolean b;
   const c = 10;
     
   proc f begin
      proc g begin
	 write 7;
      end;
      call b;
      call g;
      write 6;
   end;
      
   proc h begin
      write 9;
      call f;
   end;

   proc l begin
      write 10;
      call i;   $ error! not a proc
   end;

   call c;
   call h;
   call g; $ error! not in the correct scope (so, undefined)
   call ;
   write 5;

end.
