# An admittedly primitive Makefile
# To compile, type "make" or "make all"
# To remove files, type "make clean"

CC = gcc
CFLAGS = -Wall
LIB = -lpthread -ldl -luuid
OBJS = wserver.o wclient.o request.o io_helper.o msg.o list.o
SQ = sqlite3

.SUFFIXES: .c .o 

all: wserver wclient spin.cgi spin.o

wserver: wserver.o request.o io_helper.o sqlite3.o msg.o list.o
	$(CC) $(CFLAGS) -o wserver wserver.o request.o io_helper.o msg.o sqlite3.o list.o $(LIB)

wclient: wclient.o io_helper.o
	$(CC) $(CFLAGS) -o wclient wclient.o io_helper.o

spin.cgi: spin.c
	$(CC) $(CFLAGS) -o spin.cgi spin.c

sqlite3.o: $(SQ)/sqlite3.c
	$(CC) $(CFLAGS) -DSQLITE_THREADSAFE=0 -c $(SQ)/sqlite3.c

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f $(OBJS) wserver wclient spin.cgi
