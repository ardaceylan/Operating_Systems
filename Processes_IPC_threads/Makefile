CC = gcc
CFLAGS = -Wall -Wextra -pthread

all: app myapp

app: app.o rm.o
	$(CC) $(CFLAGS) -o app app.o rm.o

myapp: myapp.o rm.o
	$(CC) $(CFLAGS) -o myapp myapp.o rm.o

app.o: app.c rm.h
	$(CC) $(CFLAGS) -c app.c

myapp.o: myapp.c rm.h
	$(CC) $(CFLAGS) -c myapp.c

rm.o: rm.c rm.h
	$(CC) $(CFLAGS) -c rm.c

clean:
	rm -f app myapp *.o
