bin_PROGRAMS = plc
plc_SOURCES = ast.cc ast.h compiler.cc compiler.h error.cc error.h plc.cc \
	plc.h scanner.cc scanner.h token.cc token.h setops.cc setops.h \
	symboltbl.cc symboltbl.h misc.h parser.cc parser.h 
CLEANFILES = *~ *\#

# if we are *not* in debug build, let all the source files know
if NDEBUG
plc_CXXFLAGS =-DNDEBUG $(AM_CXXFLAGS)
endif

# remove symbol table in release mode
release: all
	strip $(bin_PROGRAMS)

