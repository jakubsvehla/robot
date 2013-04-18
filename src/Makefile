CC=gcc
LD=gcc
CFLAGS=-Wall -pedantic
LDFLAGS=-Wall -pedantic

BIN=../robot
OBJECTS=main.o server.o client.o robot.o

all: $(BIN)

dep:
	$(CC) -MM *.c -MF Makefile.dep

-include Makefile.dep

$(BIN): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BIN) $(OBJECTS)

.PHONY: all dep clean
