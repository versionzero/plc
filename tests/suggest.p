
begin
   
   proc alphabetic begin
   end;
   
   proc alphanumeric begin
   end;
   
   call alphabetic;
   call alphanumeric;
   call alphanumumbic; $ error! but should suggest: `alphanumeric'

end.
