CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
TARGET_BARRERA = barrera
TARGET_SIMULADOR = simulador

all: $(TARGET_BARRERA) $(TARGET_SIMULADOR)

# Parte I: Barrera Reutilizable
$(TARGET_BARRERA): src/main.c src/barrera.c src/barrera.h
	@echo "Compilando $(TARGET_BARRERA)..."
	$(CC) $(CFLAGS) -o $(TARGET_BARRERA) src/main.c src/barrera.c
	@echo "✓ Barrera compilada correctamente"

# Parte II: Simulador de Memoria Virtual
$(TARGET_SIMULADOR): src/simulador.c src/reloj.c src/utils.c src/sim.h src/reloj.h src/utils.h
	@echo "Compilando $(TARGET_SIMULADOR)..."
	$(CC) $(CFLAGS) -o $(TARGET_SIMULADOR) src/simulador.c src/reloj.c src/utils.c -lm
	@echo "✓ Simulador compilado correctamente"

# Ejecutar barrera (Parte I)
run-barrera: $(TARGET_BARRERA)
	./$(TARGET_BARRERA)

# Ejecutar simulador (Parte II)
run-simulador: $(TARGET_SIMULADOR)
	./$(TARGET_SIMULADOR) 8 8 traces/trace1.txt

# Ejecutar simulador con trace2
run-simulador2: $(TARGET_SIMULADOR)
	./$(TARGET_SIMULADOR) 8 12 traces/trace2.txt

# Ejecutar simulador interactivo
run-sim-interactive: $(TARGET_SIMULADOR)
	./$(TARGET_SIMULADOR) 4 12

# Atajos compatibles con versión anterior
run: run-barrera
main: $(TARGET_BARRERA)

clean:
	rm -f $(TARGET_BARRERA) $(TARGET_SIMULADOR) *.o

.PHONY: all clean run run-barrera run-simulador run-simulador2 run-sim-interactive main
