#include "kem_demo.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

OQS_STATUS run_kem_demo(void) {
    OQS_KEM *kem = NULL;
    uint8_t *public_key = NULL;
    uint8_t *secret_key = NULL;
    uint8_t *ciphertext = NULL;
    uint8_t *shared_secret_alice = NULL;
    uint8_t *shared_secret_bob = NULL;
    
    double t_start, t_end;
    double t_keygen, t_encaps, t_decaps;

    printf("Algoritmo: ML-KEM-768\n");

    // 1. Inicializar objeto KEM para ML-KEM-768
    kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_768);
    if (kem == NULL) {
        fprintf(stderr, "ERROR: OQS_KEM_ml_kem_768 nao esta disponível ou falhou ao inicializar.\n");
        return OQS_ERROR;
    }

    // 2. Alocação dinâmica dos buffers conforme os tamanhos da liboqs
    public_key = OQS_MEM_malloc(kem->length_public_key);
    secret_key = OQS_MEM_malloc(kem->length_secret_key);
    ciphertext = OQS_MEM_malloc(kem->length_ciphertext);
    shared_secret_alice = OQS_MEM_malloc(kem->length_shared_secret);
    shared_secret_bob = OQS_MEM_malloc(kem->length_shared_secret);

    if (!public_key || !secret_key || !ciphertext || !shared_secret_alice || !shared_secret_bob) {
        fprintf(stderr, "ERROR: Falha na alocacao de memoria (OQS_MEM_malloc).\n");
        // Força a limpeza caso algum ponteiro tenha sido alocado
        goto cleanup;
    }

    // --- [1] GERAÇÃO DE CHAVES (Bob) ---
    printf("[1] Gerando par de chaves...\n");
    t_start = get_time_ms();
    OQS_STATUS rc = OQS_KEM_keypair(kem, public_key, secret_key);
    t_end = get_time_ms();
    t_keygen = t_end - t_start;

    if (rc != OQS_SUCCESS) {
        fprintf(stderr, "ERROR: OQS_KEM_keypair falhou!\n");
        goto cleanup;
    }
    
    // Registro de Métricas e Evidências Truncadas
    printf("Chave publica: %zu bytes\n", kem->length_public_key);
    printf("Chave privada: %zu bytes\n", kem->length_secret_key);
    printf("Tempo KeyGen: %.4f ms\n", t_keygen);
    print_hex_truncated("Prefix PK", public_key, kem->length_public_key, 8);
    printf("\n");

    // --- [2] ENCAPSULAMENTO (Alice) ---
    printf("[2] Encapsulando segredo compartilhado...\n");
    t_start = get_time_ms();
    rc = OQS_KEM_encaps(kem, ciphertext, shared_secret_alice, public_key);
    t_end = get_time_ms();
    t_encaps = t_end - t_start;

    if (rc != OQS_SUCCESS) {
        fprintf(stderr, "ERROR: OQS_KEM_encaps falhou!\n");
        goto cleanup;
    }

    printf("Ciphertext KEM: %zu bytes\n", kem->length_ciphertext);
    printf("Segredo Alice: %zu bytes\n", kem->length_shared_secret);
    printf("Tempo Encaps: %.4f ms\n", t_encaps);
    print_hex_truncated("Prefix Ciphertext", ciphertext, kem->length_ciphertext, 8);
    print_hex_truncated("Prefix Segredo Alice", shared_secret_alice, kem->length_shared_secret, 4);
    printf("\n");

    // --- [3] DECAPSULAMENTO (Bob) ---
    printf("[3] Decapsulando segredo compartilhado...\n");
    t_start = get_time_ms();
    rc = OQS_KEM_decaps(kem, shared_secret_bob, ciphertext, secret_key);
    t_end = get_time_ms();
    t_decaps = t_end - t_start;

    if (rc != OQS_SUCCESS) {
        fprintf(stderr, "ERROR: OQS_KEM_decaps falhou!\n");
        goto cleanup;
    }

    printf("Segredo Bob: %zu bytes\n", kem->length_shared_secret);
    printf("Tempo Decaps: %.4f ms\n", t_decaps);
    print_hex_truncated("Prefix Segredo Bob", shared_secret_bob, kem->length_shared_secret, 4);
    printf("\n");

    // --- [4] VERIFICAÇÃO ---
    printf("[4] Verificacao do segredo compartilhado...\n");
    if (memcmp(shared_secret_alice, shared_secret_bob, kem->length_shared_secret) == 0) {
        printf("Segredos iguais: SIM\n\n");
        printf("Resultado final:\n");
        printf("Acordo de chaves ML-KEM executado com sucesso.\n");
    } else {
        printf("Segredos iguais: NAO\n\n");
        printf("Resultado final:\n");
        fprintf(stderr, "CRITICAL ERROR: Os segredos derivados nao coincidem!\n");
        rc = OQS_ERROR;
    }

cleanup:
    // Limpeza segura da memória sensível (chaves e segredos) e liberação do objeto KEM
    if (kem != NULL) {
        OQS_MEM_secure_free(secret_key, kem->length_secret_key);
        OQS_MEM_secure_free(shared_secret_alice, kem->length_shared_secret);
        OQS_MEM_secure_free(shared_secret_bob, kem->length_shared_secret);
    }
    OQS_MEM_insecure_free(public_key);
    OQS_MEM_insecure_free(ciphertext);
    OQS_KEM_free(kem);

    return rc;
}
