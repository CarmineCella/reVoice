#!/usr/bin/env bash
set -euo pipefail

if [[ $# -eq 0 ]]; then
  echo 'Usage: verify-macos-universal.sh bundle-or-binary [...]' >&2
  exit 1
fi
verify() {
  xcrun lipo "$1" -verify_arch arm64 x86_64
  printf 'Verified arm64 + x86_64: %s\n' "$1"
}
for target in "$@"; do
  if [[ -f "$target" ]]; then
    verify "$target"
  elif [[ -d "$target" ]]; then
    count=0
    while IFS= read -r -d '' binary; do
      verify "$binary"
      count=$((count+1))
    done < <(find "$target" -type f -path '*/Contents/MacOS/*' -print0)
    if [[ "$count" -eq 0 ]]; then
      echo "No bundle executables found in $target" >&2
      exit 1
    fi
  else
    echo "Missing build product: $target" >&2
    exit 1
  fi
done
