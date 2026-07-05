#include <stdio.h>
#include <stdlib.h>
#include <oqs/oqs.h>
#include "kem_demo.h"

int main(void) {
    // Inicialização global da liboqs
    OQS_init();

    printf("==================================================\n");
    printf("   DEMONSTRACAO: ACORDO DE CHAVES POS-QUANTICO    \n");
    printf("==================================================\n\n");

    OQS_STATUS status = run_kem_demo();

    // Finalização global dos recursos da liboqs
    OQS_destroy();

    if (status == OQS_SUCCESS) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
