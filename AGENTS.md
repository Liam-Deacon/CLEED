# Agent / Contributor Guidance (CLEED)

This repository is maintained with the expectation that changes are **small, testable, and reviewable**.
Use this document as the default “how we work” baseline for both humans and coding agents.

## Golden rules

1. **Work in git worktrees.** One branch/worktree per task/issue.
2. **Always test** the smallest relevant surface area before opening a PR.
3. **Run `pre-commit`** (format/validate) when configured, alongside unit/integration tests.
4. **Prefer root-cause fixes** over workarounds; keep changes minimal and focused.
5. **Avoid committing generated/build artifacts** (build dirs, `CMakeFiles/`, `config.log`, etc.).
6. **Document decisions** in PR descriptions and link to issues and permalinks.

## Workflow (required)

### 1) Create a worktree

```bash
git fetch origin
git worktree add -b <type>/<short-name> ../cleed-wt/<short-name> origin/master
cd ../cleed-wt/<short-name>
```

Suggested branch prefixes:
- `fix/...` for bugfixes
- `chore/...` for maintenance tooling/docs
- `feat/...` for new user-facing features

### 2) Make commits (Conventional Commits)

Use **Conventional Commits** with a scope where it adds clarity:

```
<type>(<scope>): <summary>
```

Examples:
- `fix(build): restore clean CMake configure on macOS`
- `chore(ci): add linux+windows build matrix`
- `docs(manual): convert pdf sources to markdown`

Keep commit bodies **digestible**:
- what changed
- why
- how it was tested
- notable follow-ups / known limitations

### 3) Test (required)

Run `pre-commit` checks where available (requires `.pre-commit-config.yaml`):

```bash
python3 -m pre_commit run --all-files
```

Prefer out-of-source builds:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

If a change affects install/packaging:

```bash
cmake --install build
```

If tests don’t exist for a modified area, add them **when feasible** and keep them deterministic.

### 4) Open a PR via `gh` (required)

Before using `gh`, ensure the correct account:

```bash
gh auth status
```

Push and open a PR:

```bash
git push -u origin HEAD
gh pr create --fill
```

PR descriptions must include:
- **Problem statement** + motivation
- **Solution summary**
- **Testing performed** (commands + platforms)
- **Links**: related issues, and GitHub **permalinks** for key code references
- **Follow-ups** (if any) as checkboxes

#### Multi-line `gh` bodies (avoid literal `\\n`)

When using the GitHub CLI, prefer passing multi-line content via a file. Shell quoting can easily turn newlines into literal `\\n`, or break on braces/quotes.

Use `--body-file` / `--title` with a here-doc:

```bash
cat <<'EOF' > /tmp/pr-body.md
## Summary
- ...

## Testing
- ...
EOF

gh pr create --title "fix(latt): ..." --body-file /tmp/pr-body.md
gh pr comment 22 --body-file /tmp/pr-body.md
```

For small one-liners, `--body "..."` is fine, but prefer `--body-file` for anything with formatting.

### 5) Keep issues actionable

When filing issues, include:
- environment (OS/toolchain versions)
- reproduction steps
- expected vs actual behavior
- logs/error output
- proposed fix direction (if known)

## Repository hygiene

- Do not commit build directories (`build/`, `CMakeFiles/`) or configure outputs (`config.status`, `config.log`).
- Prefer adding missing ignores over checking in generated content.
- Keep formatting consistent with nearby code; avoid sweeping refactors in a single PR.

## Suggested escalation path

For larger initiatives (Qt6 migration, NR removal, packaging automation), split work into:

```
investigation → tests (lock behavior) → refactor/replace → CI verification → packaging/release
```
