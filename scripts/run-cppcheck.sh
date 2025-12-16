#!/usr/bin/env bash
set -euo pipefail

if ! command -v cppcheck >/dev/null 2>&1; then
  echo "cppcheck is required for the static analysis pre-commit hook but was not found." >&2
  echo "Install cppcheck (e.g. brew install cppcheck or apt install cppcheck) before committing." >&2
  exit 1
fi

 # Suppress the missing system include warnings as Codacy already flags them in other contexts.
 # The warning set mirrors the Codacy profile we run on CI.
if [ "$#" -eq 0 ]; then
  echo "cppcheck hook called without matching files; skipping." >&2
  exit 0
fi

files_c=()
files_cpp=()
for file in "$@"; do
  case "$file" in
    *.c|*.h)
      files_c+=("$file")
      ;;
    *.cc|*.cpp)
      files_cpp+=("$file")
      ;;
    *)
      ;;
  esac
done

run_cppcheck() {
  local std="$1"
  local lang="$2"
  shift 2
  cppcheck \
    --force \
    --enable=warning,performance,portability,style,information \
    --std="$std" \
    --language="$lang" \
    --suppress=missingIncludeSystem \
    --template='[{severity}] {file}:{line}: {message}' \
    "$@"
}

if [ "${#files_c[@]}" -ne 0 ]; then
  run_cppcheck c99 c "${files_c[@]}"
fi

if [ "${#files_cpp[@]}" -ne 0 ]; then
  run_cppcheck c++11 c++ "${files_cpp[@]}"
fi
