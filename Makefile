CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
TARGET = text
SRC = text_editor_main.c

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)

clean:
	rm -f $(TARGET)