#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oqs/oqs.h>
#include "kem_demo.h"

static void print_usage(const char *program_name) {
    printf("Uso: %s [--algorithm ML-KEM-768] [--benchmark RODADAS]\n", program_name);
    printf("\n");
    printf("Algoritmos esperados: ML-KEM-512, ML-KEM-768, ML-KEM-1024.\n");
    printf("Sem argumentos, executa uma demonstracao unica com ML-KEM-768.\n");
}

static const char *normalize_algorithm(const char *value) {
    if (value == NULL || strcmp(value, "ML-KEM-768") == 0 || strcmp(value, "768") == 0) {
        return OQS_KEM_alg_ml_kem_768;
    }

    if (strcmp(value, "ML-KEM-512") == 0 || strcmp(value, "512") == 0) {
        return OQS_KEM_alg_ml_kem_512;
    }

    if (strcmp(value, "ML-KEM-1024") == 0 || strcmp(value, "1024") == 0) {
        return OQS_KEM_alg_ml_kem_1024;
    }

    return value;
}

static int parse_rounds(const char *value, size_t *rounds) {
    char *endptr = NULL;
    unsigned long parsed = strtoul(value, &endptr, 10);

    if (value == endptr || *endptr != '\0' || parsed == 0) {
        return 0;
    }

    *rounds = (size_t)parsed;
    return 1;
}

int main(int argc, char **argv) {
    const char *algorithm = MLKEM_DEFAULT_ALGORITHM;
    size_t benchmark_rounds = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        }

        if (strcmp(argv[i], "--algorithm") == 0 || strcmp(argv[i], "-a") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "ERROR: --algorithm requer um valor.\n");
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }

            algorithm = normalize_algorithm(argv[++i]);
            continue;
        }

        if (strcmp(argv[i], "--benchmark") == 0 || strcmp(argv[i], "-b") == 0) {
            if (i + 1 >= argc || !parse_rounds(argv[i + 1], &benchmark_rounds)) {
                fprintf(stderr, "ERROR: --benchmark requer um numero positivo de rodadas.\n");
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }

            i++;
            continue;
        }

        fprintf(stderr, "ERROR: argumento desconhecido: %s\n", argv[i]);
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    OQS_init();

    if (benchmark_rounds == 0) {
        printf("==================================================\n");
        printf("   DEMONSTRACAO: ACORDO DE CHAVES POS-QUANTICO    \n");
        printf("==================================================\n\n");
    }

    OQS_STATUS status;
    if (benchmark_rounds > 0) {
        status = run_kem_benchmark(algorithm, benchmark_rounds);
    } else {
        status = run_kem_demo_for_algorithm(algorithm, 1, NULL);
    }

    OQS_destroy();

    if (status == OQS_SUCCESS) {
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
