CC = cc
CFLAGS= -Wall -Werror -Wextra
TARGET = ls
SOURCES = ls.c ls.h helpers.c helpers.h print.c print.h

debug: $(SOURCES)  
	$(CC) $(CFLAGS) $(SOURCES) -g -o $(TARGET)
$(TARGET): $(SOURCES) 
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)
