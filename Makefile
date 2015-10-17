CFLAGS=-g -Wall -O3
LIBS= -lm -ljson-c -lcurl
SOURCES = timelog_read.c timelog.c

all: timelog.c timelog_read 
	gcc $(CFLAGS) $(LIBS) timelog.c -o timelog

timelog_read: timelog_read.c
	gcc $(CFLAGS) $(LIBS) timelog_read.c -o timelog_read


clean:
	rm *o timelog
