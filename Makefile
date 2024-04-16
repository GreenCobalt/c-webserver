CC=gcc
CFLAGS=-I.
LDFLAGS=

IDIR = include
ODIR = obj

_DEPS = server.h strfunc.h defs.h
_OBJS = run.o server.o strfunc.o defs.o

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

obj/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJS)
	$(CC) -o server $(OBJS) -I. $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o server