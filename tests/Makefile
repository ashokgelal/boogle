
CC=gcc
CFLAGS=-g -Wall
TESTS=test-booga test-sigterm test-write test-read
all: $(TESTS)
	
test-booga: test-booga.o
	$(CC) $(CFLAGS) -o $@ test-booga.o

test-sigterm: test-sigterm.o
	$(CC) $(CFLAGS) -o $@ test-sigterm.o

test-write: test-write.o
	$(CC) $(CFLAGS) -o $@ test-write.o

test-read: test-read.o
	$(CC) $(CFLAGS) -o $@ test-read.o

clean:
	/bin/rm -f *.o $(TESTS) a.out 

