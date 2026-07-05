#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>

// Retorna o tempo atual em milissegundos
double get_time_ms(void);

// Imprime um array de bytes em formato hexadecimal truncado para fins didáticos
void print_hex_truncated(const char *label, const uint8_t *data, size_t len, size_t max_bytes);

#endif // UTILS_H
