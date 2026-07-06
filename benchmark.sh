#!/usr/bin/env bash

set -euo pipefail

ROUNDS="${1:-30}"
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUTPUT_DIR="$PROJECT_DIR/evidencias/benchmarks"
OUTPUT_FILE="$OUTPUT_DIR/benchmark_mlkem.csv"

mkdir -p "$OUTPUT_DIR"

make -C "$PROJECT_DIR"
"$PROJECT_DIR/kem_experiment" --benchmark "$ROUNDS" > "$OUTPUT_FILE"

printf 'Benchmark salvo em %s\n' "$OUTPUT_FILE"
