begin
   const num = 10;  $ odd that negative constants aren not in the language
   integer x, y;    $ this is another comment!
   proc max         $ can have unrecognizable symbols in here, like: '#'
   begin #
      if x > y ->
	 write x;
      [] ~(x > y) ->
         write y;
      fi;
   end;
end.
