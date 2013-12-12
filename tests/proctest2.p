begin
	proc f
	begin
		proc g
		begin
			write 7;
		end;

		call g;
		write 6;
	end;

	proc h
	begin
		write 9;
		call f;
	end;

	call h;
	write 5;
end.
