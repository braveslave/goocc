CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

goocc: $(OBJS)
	$(CC) -o goocc $(OBJS) $(LDFLAGS)

$(OBJS): goocc.h

test: goocc
	./test.sh

clean:
	rm -f goocc *.o *~ tmp*

.PHONY: test clean