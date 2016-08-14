CFLAGS:=-g -Wall -Werror -std=gnu99
CFLAGS+= -O2
CFLAGS+= -msse4
LDFLAGS:=-g

all: test

test: test.c PMurHash.c

clean:
	$(RM) test

.PHONY: all clean
