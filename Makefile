CC = cc
CFLAGS= -Wall -Werror -Wextra
TARGET = ls

debug: $(TARGET).c $(TARGET).h 
	$(CC) $(CFLAGS) $(TARGET).c $(TARGET).h -g -o $(TARGET)
$(TARGET): $(TARGET).c $(TARGET).h
	$(CC) $(CFLAGS) $(TARGET).c $(TARGET).h -o $(TARGET)
