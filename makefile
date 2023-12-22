# Easily adaptable makefile
# Note: remove comments (#) to activate some features
#
# author Vitor Carreira
# date 2010-09-26 / updated: 2016-03-15 (Patricio)

# Libraries to include (if any)
LIBS=-lm

# Compiler flags
CFLAGS=-Wall -Wextra -ggdb -std=c11 -pedantic -D_POSIX_C_SOURCE=200809L #-pg

# Linker flags
LDFLAGS=#-pg

# Indentation flags
# IFLAGS=-br -brs -brf -npsl -ce -cli4 -bli4 -nut
IFLAGS=-linux -brs -brf -br

# name of the client executable
CLIENT=fileClient

# Prefix for the client's gengetopt file
CLIENT_OPT=./src/client_opt

# Object files required to build the client executable
CLIENT_OBJS=./src/client.o ./src/debug.o ./src/communication.o $(CLIENT_OPT).o

# Specifies which targets are not files
.PHONY: clean all docs indent debugon

all: $(CLIENT_OPT).h $(CLIENT)

# activate DEBUG, defining the SHOW_DEBUG macro
debugon: CFLAGS += -D SHOW_DEBUG -g
debugon: $(CLIENT)

# activate optimization (-O...)
OPTIMIZE_FLAGS=-O2 # possible values (for gcc): -O2 -O3 -Os -Ofast
optimize: CFLAGS += $(OPTIMIZE_FLAGS)
optimize: LDFLAGS += $(OPTIMIZE_FLAGS)
optimize: $(CLIENT)

$(CLIENT): $(CLIENT_OBJS)
	$(CC) -o $@ $(CLIENT_OBJS) $(LIBS)

# Dependencies
client.o: client.c debug.h common.h $(CLIENT_OPT).h
$(CLIENT_OPT).o: $(CLIENT_OPT).c $(CLIENT_OPT).h

debug.o: ./src/debug.c ./src/debug.h

communication.o: ./src/communication.c ./src/communication.h

# disable warnings from gengetopt generated files
$(CLIENT_OPT).o: $(CLIENT_OPT).c $(CLIENT_OPT).h
	$(CC) -ggdb -std=c11 -pedantic -c $< -o $@


#how to create an object file (.o) from C file (.c)
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f ./src/*.o ./src/core.* ./src/*~ .*~ $(CLIENT) ./src/*.bak

docs: Doxyfile
	doxygen Doxyfile

# entry to create the list of dependencies
depend:
	$(CC) -MM *.c

# entry 'indent' requires the application indent (sudo apt-get install indent)
indent:
	indent $(IFLAGS) *.c *.h

# entry to run the pmccabe utility (computes the "complexity" of the code)
# Requires the application pmccabe (sudo apt-get install pmccabe)
pmccabe:
	pmccabe -v *.c

# entry to run the cppcheck tool
cppcheck:
	cppcheck --enable=all --verbose *.c *.h
