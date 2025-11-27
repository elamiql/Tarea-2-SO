CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
TARGET = main

all: $(TARGET)

$(TARGET): src/main.c src/barrera.c src/barrera.h
	@echo "Compilando $(TARGET)..."
	$(CC) $(CFLAGS) -o $(TARGET) src/main.c src/barrera.c
	@echo "Compilado correctamente"

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o
