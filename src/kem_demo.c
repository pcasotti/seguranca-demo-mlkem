#include "kem_demo.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void cleanup_buffers(OQS_KEM *kem,
                            uint8_t *public_key,
                            uint8_t *secret_key,
                            uint8_t *ciphertext,
                            uint8_t *shared_secret_alice,
                            uint8_t *shared_secret_bob) {
    if (kem != NULL) {
        if (secret_key != NULL) {
            OQS_MEM_secure_free(secret_key, kem->length_secret_key);
        }
        if (shared_secret_alice != NULL) {
            OQS_MEM_secure_free(shared_secret_alice, kem->length_shared_secret);
        }
        if (shared_secret_bob != NULL) {
            OQS_MEM_secure_free(shared_secret_bob, kem->length_shared_secret);
        }
    }

    if (public_key != NULL) {
        OQS_MEM_insecure_free(public_key);
    }
    if (ciphertext != NULL) {
        OQS_MEM_insecure_free(ciphertext);
    }
    if (kem != NULL) {
        OQS_KEM_free(kem);
    }
}

static void fill_result(KEMDemoResult *result,
                        const OQS_KEM *kem,
                        double t_keygen,
                        double t_encaps,
                        double t_decaps,
                        int secrets_match) {
    if (result == NULL || kem == NULL) {
        return;
    }

    memset(result, 0, sizeof(*result));
    snprintf(result->algorithm, sizeof(result->algorithm), "%s", kem->method_name);
    result->public_key_bytes = kem->length_public_key;
    result->secret_key_bytes = kem->length_secret_key;
    result->ciphertext_bytes = kem->length_ciphertext;
    result->shared_secret_bytes = kem->length_shared_secret;
    result->keygen_ms = t_keygen;
    result->encaps_ms = t_encaps;
    result->decaps_ms = t_decaps;
    result->secrets_match = secrets_match;
}

OQS_STATUS run_kem_demo_for_algorithm(const char *algorithm, int verbose, KEMDemoResult *result) {
    OQS_KEM *kem = NULL;
    uint8_t *public_key = NULL;
    uint8_t *secret_key = NULL;
    uint8_t *ciphertext = NULL;
    uint8_t *shared_secret_alice = NULL;
    uint8_t *shared_secret_bob = NULL;

    double t_start, t_end;
    double t_keygen = 0.0;
    double t_encaps = 0.0;
    double t_decaps = 0.0;
    OQS_STATUS rc = OQS_ERROR;
    int secrets_match = 0;

    if (algorithm == NULL) {
        algorithm = MLKEM_DEFAULT_ALGORITHM;
    }

    kem = OQS_KEM_new(algorithm);
    if (kem == NULL) {
        fprintf(stderr, "ERROR: algoritmo KEM indisponivel na liboqs: %s\n", algorithm);
        return OQS_ERROR;
    }

    if (verbose) {
        printf("Algoritmo: %s\n\n", kem->method_name);
    }

    public_key = OQS_MEM_malloc(kem->length_public_key);
    secret_key = OQS_MEM_malloc(kem->length_secret_key);
    ciphertext = OQS_MEM_malloc(kem->length_ciphertext);
    shared_secret_alice = OQS_MEM_malloc(kem->length_shared_secret);
    shared_secret_bob = OQS_MEM_malloc(kem->length_shared_secret);

    if (!public_key || !secret_key || !ciphertext || !shared_secret_alice || !shared_secret_bob) {
        fprintf(stderr, "ERROR: Falha na alocacao de memoria (OQS_MEM_malloc).\n");
        goto cleanup;
    }

    if (verbose) {
        printf("[1] Bob gera o par de chaves...\n");
    }

    t_start = get_time_ms();
    rc = OQS_KEM_keypair(kem, public_key, secret_key);
    t_end = get_time_ms();
    t_keygen = t_end - t_start;

    if (rc != OQS_SUCCESS) {
        fprintf(stderr, "ERROR: OQS_KEM_keypair falhou!\n");
        goto cleanup;
    }

    if (verbose) {
        printf("Chave publica: %zu bytes\n", kem->length_public_key);
        printf("Chave privada: %zu bytes\n", kem->length_secret_key);
        printf("Tempo KeyGen: %.4f ms\n", t_keygen);
        print_hex_truncated("Prefixo PK", public_key, kem->length_public_key, 8);
        printf("\n");
    }

    if (verbose) {
        printf("[2] Alice encapsula o segredo compartilhado...\n");
    }

    t_start = get_time_ms();
    rc = OQS_KEM_encaps(kem, ciphertext, shared_secret_alice, public_key);
    t_end = get_time_ms();
    t_encaps = t_end - t_start;

    if (rc != OQS_SUCCESS) {
        fprintf(stderr, "ERROR: OQS_KEM_encaps falhou!\n");
        goto cleanup;
    }

    if (verbose) {
        printf("Ciphertext KEM: %zu bytes\n", kem->length_ciphertext);
        printf("Segredo Alice: %zu bytes\n", kem->length_shared_secret);
        printf("Tempo Encaps: %.4f ms\n", t_encaps);
        print_hex_truncated("Prefixo Ciphertext", ciphertext, kem->length_ciphertext, 8);
        print_hex_truncated("Prefixo Segredo Alice", shared_secret_alice, kem->length_shared_secret, 4);
        printf("\n");
    }

    if (verbose) {
        printf("[3] Bob decapsula o segredo compartilhado...\n");
    }

    t_start = get_time_ms();
    rc = OQS_KEM_decaps(kem, shared_secret_bob, ciphertext, secret_key);
    t_end = get_time_ms();
    t_decaps = t_end - t_start;

    if (rc != OQS_SUCCESS) {
        fprintf(stderr, "ERROR: OQS_KEM_decaps falhou!\n");
        goto cleanup;
    }

    if (verbose) {
        printf("Segredo Bob: %zu bytes\n", kem->length_shared_secret);
        printf("Tempo Decaps: %.4f ms\n", t_decaps);
        print_hex_truncated("Prefixo Segredo Bob", shared_secret_bob, kem->length_shared_secret, 4);
        printf("\n");
    }

    if (verbose) {
        printf("[4] Verificacao do segredo compartilhado...\n");
    }

    secrets_match = memcmp(shared_secret_alice, shared_secret_bob, kem->length_shared_secret) == 0;
    if (secrets_match) {
        rc = OQS_SUCCESS;
    } else {
        rc = OQS_ERROR;
    }

    if (verbose && secrets_match) {
        printf("Segredos iguais: SIM\n\n");
        printf("Resultado final:\n");
        printf("Acordo de chaves ML-KEM executado com sucesso.\n");
    } else if (verbose) {
        printf("Segredos iguais: NAO\n\n");
        printf("Resultado final:\n");
        fprintf(stderr, "CRITICAL ERROR: Os segredos derivados nao coincidem!\n");
    }

    fill_result(result, kem, t_keygen, t_encaps, t_decaps, secrets_match);

cleanup:
    cleanup_buffers(kem, public_key, secret_key, ciphertext, shared_secret_alice, shared_secret_bob);

    return rc;
}

OQS_STATUS run_kem_demo(void) {
    return run_kem_demo_for_algorithm(MLKEM_DEFAULT_ALGORITHM, 1, NULL);
}

OQS_STATUS run_kem_benchmark(const char *algorithm, size_t rounds) {
    if (rounds == 0) {
        fprintf(stderr, "ERROR: numero de rodadas deve ser maior que zero.\n");
        return OQS_ERROR;
    }

    printf("algorithm,round,keygen_ms,encaps_ms,decaps_ms,public_key_bytes,secret_key_bytes,ciphertext_bytes,shared_secret_bytes,secrets_match\n");

    for (size_t i = 1; i <= rounds; i++) {
        KEMDemoResult result;
        OQS_STATUS rc = run_kem_demo_for_algorithm(algorithm, 0, &result);

        if (rc != OQS_SUCCESS) {
            return rc;
        }

        printf("%s,%zu,%.6f,%.6f,%.6f,%zu,%zu,%zu,%zu,%d\n",
               result.algorithm,
               i,
               result.keygen_ms,
               result.encaps_ms,
               result.decaps_ms,
               result.public_key_bytes,
               result.secret_key_bytes,
               result.ciphertext_bytes,
               result.shared_secret_bytes,
               result.secrets_match);
    }

    return OQS_SUCCESS;
}
