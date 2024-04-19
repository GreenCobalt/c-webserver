CC=gcc
CFLAGS=-I. -O3
LDFLAGS=-lconfuse -lmagic

IDIR = include
ODIR = obj

_DEPS = server.h strfunc.h defs.h file.h
_OBJS = httpd.o server.o strfunc.o defs.o file.o

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

obj/%.o: src/%.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJS)
	$(CC) -o server $(OBJS) -I. $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o server