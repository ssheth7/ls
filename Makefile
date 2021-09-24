CC = cc
CFLAGS= -Wall -Werror -Wextra
TARGET = ls

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c $(TARGET).h -o $(TARGET)
