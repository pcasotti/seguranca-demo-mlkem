#include "utils.h"
#include <stdio.h>
#include <time.h>

double get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
}

void print_hex_truncated(const char *label, const uint8_t *data, size_t len, size_t max_bytes) {
    printf("%s (%zu bytes): ", label, len);
    size_t to_print = (len < max_bytes) ? len : max_bytes;
    for (size_t i = 0; i < to_print; i++) {
        printf("%02X", data[i]);
    }
    if (len > max_bytes) {
        printf("...");
    }
    printf("\n");
}
