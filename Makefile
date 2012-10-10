CC=g++
CFLAGS=-c -Wall -g
LDLIBS=-ltrace -lsqlite3 -lwandevent -largtable2
all: ospf

ospf: ospf_scan.o ospf_functions.o sql_functions.o daemons.o logger.o
	$(CC) -o ospf_monitor ospf_functions.o ospf_scan.o sql_functions.o daemons.o logger.o $(LDLIBS)
ospf_scan.o: ospf_scan.cpp ospf.h 591.h
	$(CC) $(CFLAGS) ospf_scan.cpp
ospf_functions.o: ospf_functions.cpp ospf.h 591.h
	$(CC) $(CFLAGS) ospf_functions.cpp
sql_functions.o: sql_functions.cpp ospf.h 591.h sql.h
	$(CC) $(CFLAGS) sql_functions.cpp
daemons.o: daemons.c daemons.h
	$(CC) $(CFLAGS) daemons.c
logger.o: logger.c logger.h
	$(CC) $(CFLAGS) logger.c
clean:
	rm -rf *o
	rm -rf *~
