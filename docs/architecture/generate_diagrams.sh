#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUTPUT_DIR="$SCRIPT_DIR"

for src in "$SCRIPT_DIR"/*.puml; do
    base="$(basename "$src" .puml)"
    for fmt in svg png; do
        out="$OUTPUT_DIR/${base}.${fmt}"
        http_code=$(curl -s -w "%{http_code}" -X POST \
            --data-binary "@$src" \
            -H "Content-Type: text/plain" \
            -o "$out" \
            "https://kroki.io/plantuml/${fmt}")
        if [[ "$http_code" == "200" ]]; then
            echo "Generated ${fmt^^}: ${out}"
        else
            rm -f "$out"
            echo "Failed to render ${fmt^^} for ${src} (HTTP ${http_code})" >&2
            exit 1
        fi
    done
done
