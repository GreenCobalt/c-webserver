# targets: 
# linux 	= x86_64-linux-gnu-gcc
# windows 	= x86_64-w64-mingw32-gcc

CFLAGS=-I. -O3 -W -Wall -Werror
LDFLAGS=-static

IDIR = include
ODIR = obj

_DEPS = server.h str.h http_def.h file.h http_error.h date.h config.h mime.h debug.h
_OBJS = main.o server.o str.o http_def.o file.o http_error.o date.o config.o mime.o

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

ifeq ($(CC), x86_64-w64-mingw32-gcc)
	LDFLAGS += -lws2_32
endif

obj/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJS)
	$(CC) -o server $(OBJS) -I. $(LDFLAGS)

linux:
	CC=x86_64-linux-gnu-gcc make server

windows:
	CC=x86_64-w64-mingw32-gcc make server

.PHONY: clean
clean:
	rm -rf $(ODIR)/*.o server server.exe a.out