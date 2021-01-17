CC = gcc
LD = gcc

CFLAGS = -Wall -std=c99
LFLAGS = -Wall

TESTES = p1\
		 p2

P1OBJS = p1.o
P2OBJS = p2.o

TRABALHO = disparador

MAINOBJS = principal.o

EXECS = $(TESTES) $(TRABALHO)

.c.o:
	$(CC) $(CFLAGS) -c $<

all: $(EXECS)

p1: $(P1OBJS)
	$(LD) $(LFLAGS) -o $@ $(P1OBJS)

p2: $(P2OBJS)
	$(LD) $(LFLAGS) -o $@ $(P2OBJS)
	
disparador: $(MAINOBJS)
	$(LD) $(LFLAGS) -o $@ $(MAINOBJS)

.PHONY: clean

clean:
	rm -f *.o *.core
