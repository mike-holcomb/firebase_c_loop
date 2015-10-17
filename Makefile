CFLAGS=-c -Wall

all: timelog.c
	gcc $(CFLAGS) timelog.c -o timelog

clean:
	rm *o timelog
