$ find reduced form of quotient
begin
        integer M, N, G;

        $ find GCD of M, N, returned in G
        proc GCD
        begin
                integer m, n, rem;

                m, n := M, N;
                do ~( n = 0 ) ->
                        rem := m \ n;
                        m := n;
                        n := rem;
                od;
                G := m;
        end;

        $ get numerator and denominator
        read M, N;
        call GCD;
        $ if quotient is reducible, print reduced form
        if ~( G = 1 ) ->
                write M / G, N / G; []
        G = 1 ->
                skip;
        fi;
end.
