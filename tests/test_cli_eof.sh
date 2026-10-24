#!/usr/bin/env bash
# Pruebas de regresión de la CLI: entrada cerrada (EOF), índices inválidos y
# una búsqueda completa. Uso: test_cli_eof.sh <binario> <fichero.txt>
set -euo pipefail

BIN="${1:?falta el binario}"
DATA="${2:?falta el fichero de datos}"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

fail() {
    echo "FAILED: $1" >&2
    exit 1
}

run_eof() {
    local name="$1"
    local input="$2"
    printf '%b' "$input" | timeout 20 "$BIN" >"$TMP/out" 2>&1 ||
        fail "$name terminó con error"
}

run_eof "EOF en el tipo de trie" ""
run_eof "EOF en el autocompletado" "1\n"
run_eof "EOF en el menú de ficheros" "1\n1\n"
run_eof "EOF durante la búsqueda" "1\n1\n0\n${DATA}\ns\n"

printf '1\n1\n1abc\n999\n0\n%s\nx\n' "$DATA" | timeout 20 "$BIN" \
    >"$TMP/invalid" 2>&1 || fail "índice inválido terminó con error"
grep -q "Invalid input. Please enter a valid number." "$TMP/invalid" ||
    fail "no se informó del índice no numérico"
grep -q "Error: index not available" "$TMP/invalid" ||
    fail "no se informó del índice fuera de rango"

printf '1\n1\n0\n%s\ns\nalice\nx\n' "$DATA" | timeout 20 "$BIN" \
    >"$TMP/normal" 2>&1 || fail "búsqueda con trie normal terminó con error"
grep -q "Total words found with the prefix 'alice'" "$TMP/normal" ||
    fail "el trie normal no encontró el prefijo alice"

printf '2\n2\n0\n%s\ns\nalice\nx\n' "$DATA" | timeout 20 "$BIN" \
    >"$TMP/radix" 2>&1 || fail "búsqueda con radix trie terminó con error"
grep -q "Total words found with the prefix 'alice'" "$TMP/radix" ||
    fail "el radix trie no encontró el prefijo alice"

echo "OK: CLI"
