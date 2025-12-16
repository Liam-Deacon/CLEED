# Static analysis (Codacy + local tooling)

This project uses Codacy for static analysis checks on pull requests. Locally, we run a `cppcheck` pre-commit hook so issues can be caught before pushing.

## What Codacy is checking

Codacy reports issues from multiple analyzers (including Cppcheck). The current PR feedback has included:

- **Security/robustness**: e.g. CWE-126 warnings when code assumes inputs are always NUL-terminated.
- **Maintainability**: function size and cyclomatic complexity thresholds (examples seen in PR feedback):
  - **Max function length**: 50 lines
  - **Max cyclomatic complexity**: 8

These thresholds are enforced by Codacy and should be treated as the “source of truth”.

### Scope

Codacy exclusions are configured in `.codacy.yml`. We exclude `tests/**` from Codacy analysis to avoid test-only false positives/noise, while still keeping local `pre-commit`/Cppcheck coverage on tests.

## Local Cppcheck (pre-commit)

We run Cppcheck locally via pre-commit to give fast feedback.

Configuration lives in:

- `.pre-commit-config.yaml`

The current hook is intentionally configured to:

- Run on `src/latt` and `tests`
- Enable common categories (`warning`, `performance`, `portability`, `style`, `information`)
- Suppress `missingIncludeSystem` (Codacy/Cppcheck can run without system headers)
- **Not** fail the commit on findings (`--error-exitcode=0`) so we can iteratively reduce the baseline

Run locally:

```bash
python3 -m pre_commit run --all-files
```

If you want to run Cppcheck manually with the same intent:

```bash
cppcheck --enable=warning,performance,portability,style,information \
  --suppress=missingIncludeSystem \
  src/latt tests
```

## Notes / best practices

- Prefer bounded operations (`strnlen`, bounded `sscanf` formats, etc.) when parsing external input.
- Keep parsing logic table-driven and split into small helpers to stay under Codacy complexity/LOC thresholds.
- When changing algorithms, add tests that lock behaviour to protect against regressions.
