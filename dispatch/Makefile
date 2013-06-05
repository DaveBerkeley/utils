
CC = g++
CFLAGS = -Wall

all: dispatch

clean:
	rm dispatch
	rm *.o

dispatch: main.o messages.o
	$(CC) $(LDFLAGS) -o $@ $^

messages.o: messages.cpp dispatch.h list.h messages.h
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: main.cpp dispatch.h list.h messages.h
	$(CC) $(CFLAGS) -c -o $@ $<

test: dispatch
	./dispatch

