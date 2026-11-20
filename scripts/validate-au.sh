#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 2 || ! -d "$1/Contents" || ! -d "$2/Contents" ]]; then
  echo 'Usage: validate-au.sh path/to/reVoice.component "path/to/reVoice MIDI.component"' >&2
  exit 1
fi
component_paths=("$(cd "$1" && pwd -P)" "$(cd "$2" && pwd -P)")
component_directory="$HOME/Library/Audio/Plug-Ins/Components"
validation_directories=("$component_directory/reVoice-validation.component" "$component_directory/reVoice-MIDI-validation.component")
for directory in "${validation_directories[@]}"; do
  if [[ -e "$directory" || -L "$directory" ]]; then
    echo "Validation path already exists; leaving it untouched: $directory" >&2
    exit 1
  fi
done
mkdir -p "$component_directory"
created_count=0
marker="${component_paths[0]}:${component_paths[1]}:$$"
validation_tmp="$(mktemp -d "${TMPDIR:-/tmp}/revoice-auval.XXXXXX")"
owner_file="$validation_tmp/owner"
# Keep logs outside Components. Rewriting a file there on every retry keeps
# Apple's directory watcher busy and can indefinitely postpone its rescan.
validation_log="$validation_tmp/auval.log"
printf '%s\n' "$marker" > "$owner_file"
cleanup() {
  if [[ -f "$owner_file" && "$(cat "$owner_file")" == "$marker" ]]; then
    for ((index=0; index<created_count; index++)); do
      if [[ -e "${validation_directories[index]}" || -L "${validation_directories[index]}" ]]; then
        rm -r -- "${validation_directories[index]}"
      fi
    done
    rm -- "$owner_file"
    rm -f -- "$validation_log"
    rmdir "$validation_tmp"
  fi
}
trap cleanup EXIT
for index in 0 1; do
  mkdir "${validation_directories[index]}"
  created_count=$((created_count+1))
  /usr/bin/ditto "${component_paths[index]}" "${validation_directories[index]}"
  /usr/bin/codesign --verify --deep --strict "${validation_directories[index]}"
  touch "${validation_directories[index]}"
done
touch "$component_directory"
validate() {
  local status=0
  for attempt in {1..60}; do
    if /usr/bin/auval -v "$1" "$2" CmCe -strict > "$validation_log" 2>&1; then
      cat "$validation_log"
      return 0
    else
      status=$?
    fi
    # Registration follows filesystem notifications asynchronously. Retry only
    # discovery, never hide a validation failure of a discovered component.
    if [[ "$(cat "$validation_log")" != *"didn't find the component"* ]]; then
      cat "$validation_log" >&2
      return "$status"
    fi
    sleep 1
  done
  cat "$validation_log" >&2
  if [[ "${GITHUB_ACTIONS:-false}" == true ]]; then
    /usr/bin/log show --last 2m --style compact --predicate 'process == "AudioComponentRegistrar"' | tail -n 80 >&2 || true
  fi
  return "$status"
}
validate aufx rVce
validate aumf rVcM
