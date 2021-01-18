CC = gcc
LD = gcc

CFLAGS = -Wall -std=c99
LFLAGS = -Wall


TRABALHO = disparador

MAINOBJS = principal.o

EXECS = $(TRABALHO)

.c.o:
	$(CC) $(CFLAGS) -c $<

all: $(EXECS)

disparador: $(MAINOBJS)
	$(LD) $(LFLAGS) -o $@ $(MAINOBJS)
	@echo "Arquivo compilado: disparador"

.PHONY: clean clean-all

clean:
	rm -f *.o *.core

clean-all:
	rm -f *.o *.core $(EXECS)
