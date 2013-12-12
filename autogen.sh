#! /bin/sh

rm -rf autom4te.cache config
rm -f  config.h
autoreconf -v --force --install -I config
# ./configure

#rm -rf config.cache
#rm -f acconfig.h
#aclocal -I m4
#autoconf
#autoheader
#automake -a
