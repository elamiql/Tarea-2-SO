#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Parsea una direccion de memoria desde string
// Soporta hexadecimal (0x), octal (0) y decimal
unsigned long parse_address(char *s) {
    if (s == NULL) {
        return 0;
    }
    
    // Saltar espacios al inicio
    while (isspace(*s)) {
        s++;
    }
    
    // Convertir usando strtoul (auto-detecta formato)
    char *endptr;
    unsigned long addr = strtoul(s, &endptr, 0);
    
    return addr;
}
