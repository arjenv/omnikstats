#
# Makefile to build omnikstats
# 
OBJECTS=   omnikstats.o \
           omnikfunctions.o\
           omniksearch.o\
           omnikgetstats.o\
           omnikcsv.c\
           omnikpvoutput.o\
           omnikfillstruct.o\
           omnikreadconfig.o

CC=gcc

CFLAGS= -Wall

LDFLAGS=

LDLIBS=-lm -L/usr/lib/i386-linux-gnu -lcurl

omnikstats.o: omnikstats.c
	$(CC) $(CFLAGS) -c omnikstats.c

omnikfunctions.o: omnikfunctions.c
	$(CC) $(CFLAGS) -c omnikfunctions.c

omniksearch.o: omniksearch.c
	$(CC) $(CFLAGS) -c omniksearch.c

omnikgetstats.o: omnikgetstats.c
	$(CC) $(CFLAGS) -c omnikgetstats.c

omnikcsv.o: omnikcsv.c
	$(CC) $(CFLAGS) -c omnikcsv.c

omnikpvoutput.o: omnikpvoutput.c
	$(CC) $(CFLAGS) -c omnikpvoutput.c

omnikfillstruct.o: omnikfillstruct.c
	$(CC) $(CFLAGS) -c omnikfillstruct.c

omnikreadconfig.o: omnikreadconfig.c
	$(CC) $(CFLAGS) -c omnikreadconfig.c

clean:
	rm -f *.o
	rm -f omnikstats

all: $(OBJECTS)
	$(CC) $(CFLAGS) -o omnikstats $(OBJECTS) $(LDFLAGS) $(LDLIBS)

