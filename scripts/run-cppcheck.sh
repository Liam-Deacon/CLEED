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

cppcheck \
  --force \
  --enable=warning,performance,portability,style,information \
  --std=c99 \
  --language=c \
  --suppress=missingIncludeSystem \
  --template='[{severity}] {file}:{line}: {message}' \
  "$@"
