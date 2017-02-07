CC=gcc
CFLAGS=-O0 -fPIC -g
WDIR=/winhomes/jab165/ece650/ece650_myMalloc

all: thread_test

thread_test: thread_test.c
	$(CC) $(CFLAGS) -I$(WDIR) -L$(WDIR) -Wl,-rpath=$(WDIR) -o $@ thread_test.c -lmymalloc -lpthread

clean:
	rm -f *~ *.o thread_test

clobber:
	rm -f *~ *.o
