#!/usr/bin/env bash

set -euo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUTPUT_DIR="$PROJECT_DIR/evidencias/execucoes"
OUTPUT_FILE="$OUTPUT_DIR/demo_output.txt"

mkdir -p "$OUTPUT_DIR"

make -C "$PROJECT_DIR"
"$PROJECT_DIR/kem_experiment" > "$OUTPUT_FILE"

printf 'Execucao salva em %s\n' "$OUTPUT_FILE"
