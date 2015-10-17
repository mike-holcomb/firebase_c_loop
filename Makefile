CFLAGS=-g -Wall -O3
LIBS= -lm -ljson-c -lcurl

all: timelog.c
	gcc $(CFLAGS) $(LIBS) timelog.c -o timelog

clean:
	rm *o timelog
