CFLAGS=-g -Wall -O3
LIBS= -lm -ljson-c -lcurl
SOURCES = timelog_read.c firebase_test.c
PROGRAMS = timelog_read firebase_test

all: $(PROGRAMS)

timelog_read: timelog_read.c
	gcc $(CFLAGS) $(LIBS) timelog_read.c -o timelog_read

firebase_test: firebase_test.c
	gcc $(CFLAGS) $(LIBS) firebase_test.c -o firebase_test


clean:
	rm $(PROGRAMS)
