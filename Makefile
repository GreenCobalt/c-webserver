CC=gcc
CFLAGS=-I.

IDIR = include
ODIR = obj

_DEPS = server.h strfunc.h
_OBJS = run.o server.o strfunc.o

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

obj/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJS)
	$(CC) -o server $(OBJS) -I.

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o server