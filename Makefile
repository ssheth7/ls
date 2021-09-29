CC = cc
CFLAGS= -Wall -Werror -Wextra
TARGET = ls
SOURCES = ls.c ls_helpers.c ls_helpers.h
debug: $(SOURCES)  
	$(CC) $(CFLAGS) $(SOURCES) -g -o $(TARGET)
$(TARGET): $(TARGET).c 
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)
