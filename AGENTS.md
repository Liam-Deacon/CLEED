# Agent / Contributor Guidance (CLEED)

This repository expects changes to remain **small, testable, and reviewable**.
Use this document as the single authoritative guide for how contributions are handled.

## Core commitments

1. **Work in git worktrees.** Every task or issue must live inside its own worktree/branch so that work remains isolated and reviewable.
2. **Always run `pre-commit`.** Before pushing, run the hook suite to format/validate changes alongside the rest of the workflow.
3. **Test the affected surface area.** After pre-commit, execute relevant unit, integration, or system tests for the modified functionality (pre-commit + tests = required).
4. **Prefer root-cause fixes**; avoid shortcuts and keep changes focused on solving the underlying problem.
5. **Keep generated artifacts out of commits** (no `build/`, `CMakeFiles/`, `config.log`, etc.).
6. **Document decisions in the PR.** Reference issues and permalinks so reviewers understand the context.

## Workflow (required)

### 1) Create a worktree

```bash
git fetch origin
git worktree add -b <type>/<short-name> ../cleed-wt/<short-name> origin/master
cd ../cleed-wt/<short-name>
```

Suggested branch prefixes:
- `fix/...` for bugfixes
- `chore/...` for tooling/docs work
- `feat/...` for user-visible features

### 2) Make commits (Conventional Commits)

Stick to **Conventional Commits** that include a meaningful scope:

```
<type>(<scope>): <summary>
```

Examples:
- `fix(build): restore clean CMake configure on macOS`
- `chore(ci): add linux+windows build matrix`
- `docs(manual): convert pdf sources to markdown`

Keep commit bodies concise:
- what changed
- why it changed
- how it was tested
- follow-ups or limitations

### 3) Test (required)

Run `pre-commit` (requires `.pre-commit-config.yaml`) before the rest:

```bash
python3 -m pre_commit run --all-files
```

Then run the targeted tests (unit, integration, system) so that each change touches:
- formatting/validation hooks (pre-commit)
- the functionality implied by the diff (project tests)

Out-of-source builds are preferred:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

For packaging-impacting changes:

```bash
cmake --install build
```

If tests don't exist where necessary, add deterministic coverage when practical.

### 4) Open a PR via `gh` (required)

Ensure the correct GitHub user is active:

```bash
gh auth status
```

Push and open a PR:

```bash
git push -u origin HEAD
gh pr create --fill
```

PR descriptions must cover:
- **Problem statement** + motivation
- **Solution summary**
- **Testing performed** (commands + platforms)
- **Links**: related issues and GitHub **permalinks**
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

Include these in issues:
- environment (OS/toolchain versions)
- reproduction steps
- expected vs actual behavior
- logs/errors
- proposed fix direction (if known)

## Repository hygiene

- Avoid committing build directories (`build/`, `CMakeFiles/`) or configure outputs (`config.status`, `config.log`).
- Prefer adding missing ignores instead of generated content.
- Keep formatting aligned with adjacent code; avoid sweeping refactors in one PR.

## Documentation (reStructuredText)

- The Sphinx sources under `doc/` are `*.rst` (reStructuredText), not Markdown; whitespace/indentation and blank lines are part of the syntax.
- Prefer `.. toctree::` navigation over `.. include::` so pages are addressable and cross-references behave predictably.
- Avoid tabs; use spaces (tabs can create accidental block quotes / unexpected indentation).
- See `skills/restructured-text/SKILL.md` for repo-specific conventions and common pitfalls.

## Suggested escalation path

For large initiatives (QT6 migration, NR removal, packaging automation), split work into:

```
investigation → tests (lock behavior) → refactor/replace → CI verification → packaging/release
```
