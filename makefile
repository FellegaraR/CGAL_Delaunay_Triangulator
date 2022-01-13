CC	=	g++
FLAGS	=	-std=c++14	-O3

SOURCES	=	sources/string_management.h	sources/string_management.cpp	\
	sources/io.h	sources/io.cpp	\
	sources/main_tri.cpp
	
all: del_tri
	
del_tri:
	$(CC)	$(FLAGS)	-o	dtri	$(SOURCES)	-DCGAL_HEADER_ONLY	-lgmp
	
clean:
	rm	dtri
