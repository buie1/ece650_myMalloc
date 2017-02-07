CC=gcc
CFLAGS=-O0 -fPIC -g
MALLOC_VERSION=FF
WDIR=/Users/jonbuie/Desktop/college_Folder/Duke/sp17/ece650/homeworkDocs/hw1_myMalloc

all: mymalloc_test

mymalloc_test: mymalloc_test.c
	$(CC) $(CFLAGS) -I$(WDIR) -L$(WDIR) -D$(MALLOC_VERSION) -Wl,-rpath,$(WDIR) -o $@ mymalloc_test.c -lmymalloc

clean:
	rm -f *~ *.o mymalloc_test

clobber:
	rm -f *~ *.o
