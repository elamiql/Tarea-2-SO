# Tarea 2 - Sistemas Operativos

### Integrantes:
- Joseph Matamala
- Juan Pablo Núñez
- Agustín Salgado
- Ignacio Silva

---

## Descripcion

Implementacion de dos partes:
- **Parte I**: Barrera reutilizable con monitores (`pthread_mutex_t` y `pthread_cond_t`)
- **Parte II**: Simulador de memoria virtual con algoritmo de reemplazo Clock (Second Chance)

---

## Requisitos

- GCC (Compilador de C)
- Entorno Linux/Unix/WSL
- Biblioteca pthread
- Make (opcional)

### Instalacion en WSL (Windows)

```bash
wsl sudo apt-get update
wsl sudo apt-get install -y build-essential
```

---

## Compilacion

### Compilar todo
```bash
make all
# o en WSL:
wsl make all
```

### Compilar por partes
```bash
make barrera    # Solo Parte I
make simulador  # Solo Parte II
```

### Compilacion manual
```bash
# Parte I
gcc -Wall -Wextra -pthread -g -o barrera src/main.c src/barrera.c

# Parte II
gcc -Wall -Wextra -pthread -g -o simulador src/simulador.c src/reloj.c src/utils.c -lm
```

---

## Uso

### Parte I: Barrera Reutilizable

```bash
./barrera [N] [E]
```

**Parametros:**
- N: Numero de hebras (default: 5)
- E: Numero de etapas (default: 4)

**Ejemplos:**
```bash
./barrera           # 5 hebras, 4 etapas
./barrera 3 2       # 3 hebras, 2 etapas
wsl ./barrera 10 5  # En WSL
```

**Verificacion:**
- Todas las hebras deben esperar en cada etapa antes de continuar
- Ninguna hebra avanza a la etapa X+1 hasta que todas completen la etapa X

---

### Parte II: Simulador de Memoria Virtual

```bash
./simulador <num_marcos> <bits_pagina> [archivo_trazas]
```

**Parametros:**
- num_marcos: Numero de marcos fisicos (ej: 4, 8, 16)
- bits_pagina: Bits de offset (10=1KB, 12=4KB, 16=64KB)
- archivo_trazas: (Opcional) Archivo con direcciones virtuales

**Ejemplos:**
```bash
# Archivos de trazas (trace1.txt con 8 bits)
./simulador 8 8 traces/trace1.txt     # 8 marcos, paginas 256B
./simulador 16 8 traces/trace1.txt    # 16 marcos, paginas 256B
./simulador 32 8 traces/trace1.txt    # 32 marcos, paginas 256B

# Archivos de trazas (trace2.txt con 12 bits)
./simulador 8 12 traces/trace2.txt    # 8 marcos, paginas 4KB
./simulador 16 12 traces/trace2.txt   # 16 marcos, paginas 4KB
./simulador 32 12 traces/trace2.txt   # 32 marcos, paginas 4KB

# Usando Makefile o modo interactivo
make run-simulador                    # Ejecuta con trace1.txt
wsl ./simulador 4 12                  # Modo interactivo en WSL
```

**Archivos de trazas incluidos:**
- `traces/trace1.txt` - Tamaño marco: 8 bits / 256B
- `traces/trace2.txt` - Tamaño marco: 12 bits / 4KB

**Formato de trazas:**
- Hexadecimal: `0x1000`
- Decimal: `4096`
- Octal: `010000`
- Comentarios: lineas con `#`

---

## Limpieza

```bash
make clean
```

---

## Estructura del Proyecto

```
Tarea-2-SO/
├── Makefile
├── README.md
├── traces/
│   ├── trace1.txt          (8 bits / 256B)
│   └── trace2.txt          (12 bits / 4KB)
└── src/
    ├── barrera.c, barrera.h    (Parte I)
    ├── main.c                  (Parte I)
    ├── simulador.c             (Parte II)
    ├── reloj.c, reloj.h        (Parte II)
    ├── sim.h                   (Parte II)
    └── utils.c, utils.h        (Parte II)
```

---

## Componentes Principales

### Parte I: Barrera Reutilizable

**Estructura:**
```c
typedef struct {
    int count;              // Hebras que llegaron
    int N;                  // Total de hebras
    int etapa;              // Etapa actual
    pthread_mutex_t mutex;  // Exclusion mutua
    pthread_cond_t cond;    // Variable condicion
} Barrera;
```

**Funciones:**
1. `barrera_init()` - Inicializa para N hebras
2. `barrera_wait()` - Bloquea hasta que todas lleguen
3. `barrera_destroy()` - Libera recursos

**Mecanismo de reutilizacion:**
- Cada hebra captura la etapa actual antes de esperar
- La ultima hebra incrementa la etapa y despierta a todas con `pthread_cond_broadcast()`

---

### Parte II: Simulador de Memoria Virtual

**Estructura:**
```c
typedef struct {
    int pagina;   // Numero de pagina virtual
    int usado;    // Bit de referencia (Clock)
} Marco;
```

**Variables globales:**
- `Marco *marcos` - Array de marcos fisicos
- `int Nmarcos` - Numero total de marcos
- `int puntero` - Puntero circular (Clock)
- Estadisticas: hits, faults, referencias

**Funciones principales:**

1. **`buscar_pagina(nvp)`** - Busca pagina en memoria
2. **`asignar_marco(nvp)`** - Asigna marco libre o reemplaza
3. **`reloj_reemplazo(nvp)`** - Algoritmo Clock:
   - Recorre marcos circularmente
   - Si usado=1: resetea a 0 (segunda oportunidad)
   - Si usado=0: selecciona como victima
4. **`traducir(marco, offset, b)`** - Formula: `(marco << b) | offset`
5. **`parse_address(s)`** - Parsea hex/decimal/octal

**Flujo:**
1. Lee parametros
2. Inicializa marcos
3. Por cada direccion virtual:
   - Extrae pagina y offset
   - Busca en memoria (HIT/FAULT)
   - Traduce a direccion fisica
4. Muestra estadisticas