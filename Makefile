CC=gcc
CFLAGS=-I.
DEPS=jsonformatter.h

%.o: %.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CLFLAGS)

all: jsonformatter.o
	$(CC) ${CFLAGS} -o jsonformatter jsonformatter.o

clean:
	rm jsonformatter.o
	rm jsonformatter
