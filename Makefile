CC = cc
CFLAGS= -Wall -Werror -Wextra
TARGET = ls
SOURCES = ls.o helpers.o print.o 

debug: $(SOURCES)  
	$(CC) $(CFLAGS) $(SOURCES) -g -o $(TARGET)
$(TARGET): $(SOURCES) 
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

ls.o: ls.c ls.h
	${CC} -g -c ls.c

print.o: print.c print.h
	${CC} -g -c print.c

helpers.o: helpers.c helpers.h
	$(CC) -g -c helpers.c

clean:
	rm -f ls *.o
