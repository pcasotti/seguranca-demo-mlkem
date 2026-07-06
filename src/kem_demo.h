#ifndef KEM_DEMO_H
#define KEM_DEMO_H

#include <stddef.h>
#include <oqs/oqs.h>

#define MLKEM_DEFAULT_ALGORITHM OQS_KEM_alg_ml_kem_768

typedef struct {
    char algorithm[64];
    size_t public_key_bytes;
    size_t secret_key_bytes;
    size_t ciphertext_bytes;
    size_t shared_secret_bytes;
    double keygen_ms;
    double encaps_ms;
    double decaps_ms;
    int secrets_match;
} KEMDemoResult;

// Executa o fluxo experimental completo do ML-KEM-768.
OQS_STATUS run_kem_demo(void);

// Executa o fluxo para um algoritmo KEM especifico.
OQS_STATUS run_kem_demo_for_algorithm(const char *algorithm, int verbose, KEMDemoResult *result);

// Executa multiplas rodadas e imprime os resultados em CSV.
OQS_STATUS run_kem_benchmark(const char *algorithm, size_t rounds);

#endif // KEM_DEMO_H
