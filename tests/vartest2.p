begin
integer x;
integer array q[10];
integer y, z;
integer array r[ 10 ];
integer u, v, w;
integer w;
Boolean b;
const c	= 10;   $ ok!
const d	= true; $ ok!
const e	= d;    $ ok!
const f	= v;    $ v is non-constant
const g	= f;    $ ok! - but only silently ok
const h	= g;    $ ok!
const i	= ggg;  $ ggg not defined
const j	= j;    $ this *should* be an error
const x	= 10;   $ already an integer
end.	
