CFLAGS:=-g -Wall -Werror -std=gnu99
CFLAGS+= -O2
CFLAGS+= -msse4
LDFLAGS:=-g

all: test

test: test.o mhash.o PMurHash.o

clean:
	$(RM) test *.o

.PHONY: all clean
