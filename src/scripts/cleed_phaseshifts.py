#!/usr/bin/env python3
"""Generate CLEED phase shift files via the phaseshifts package."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
from datetime import datetime, timezone


def sha256sum(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def phaseshifts_version() -> str:
    try:
        import phaseshifts  # type: ignore

        return getattr(phaseshifts, "__version__", "unknown")
    except Exception:
        return "unknown"


def resolve_phsh(binary: str) -> str:
    resolved = shutil.which(binary)
    return resolved or binary


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Generate CLEED phase shift files using phaseshifts.",
    )
    parser.add_argument("-b", "--bulk", required=True, help="Bulk .bul input file")
    parser.add_argument("-i", "--slab", required=True, help="Slab .inp input file")
    parser.add_argument(
        "-o",
        "--output-dir",
        default=os.environ.get("CLEED_PHASE", ""),
        help="Output directory for .phs files (defaults to CLEED_PHASE)",
    )
    parser.add_argument(
        "-l",
        "--lmax",
        type=int,
        default=None,
        help="Maximum angular momentum quantum number",
    )
    parser.add_argument(
        "-f",
        "--format",
        default="CLEED",
        help="Phase shift output format (default: CLEED)",
    )
    parser.add_argument(
        "-t",
        "--tmpdir",
        default=None,
        help="Temporary directory for intermediate files",
    )
    parser.add_argument(
        "--phsh",
        default="phsh.py",
        help="Path to phaseshifts phsh.py executable",
    )
    parser.add_argument(
        "--manifest",
        default=None,
        help="Path to write a JSON manifest (defaults to <output-dir>/phaseshifts-manifest.json)",
    )
    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()

    bulk = Path(args.bulk)
    slab = Path(args.slab)
    if not bulk.exists():
        parser.error(f"Bulk file not found: {bulk}")
    if not slab.exists():
        parser.error(f"Slab file not found: {slab}")

    output_dir = Path(args.output_dir) if args.output_dir else None
    if output_dir is None:
        parser.error("Output directory is required (set CLEED_PHASE or --output-dir)")

    output_dir.mkdir(parents=True, exist_ok=True)
    manifest_path = (
        Path(args.manifest)
        if args.manifest
        else output_dir / "phaseshifts-manifest.json"
    )

    phsh = resolve_phsh(args.phsh)
    if shutil.which(phsh) is None and not Path(phsh).exists():
        parser.error(
            f\"phsh.py not found (searched for '{args.phsh}'); install phaseshifts or pass --phsh\"
        )
    cmd = [phsh, "-g", "-b", str(bulk), "-i", str(slab), "-f", args.format]
    if args.lmax is not None:
        cmd.extend(["-l", str(args.lmax)])
    if args.tmpdir:
        cmd.extend(["-t", args.tmpdir])

    env = os.environ.copy()
    env["CLEED_PHASE"] = str(output_dir)

    try:
        subprocess.run(cmd, check=True, env=env)
    except FileNotFoundError as exc:
        print(f\"Failed to execute {phsh}: {exc}\", file=sys.stderr)
        return 127
    except subprocess.CalledProcessError as exc:
        return exc.returncode

    phs_files = sorted(p.name for p in output_dir.glob("*.phs"))
    manifest = {
        "created_at": datetime.now(timezone.utc).isoformat(),
        "phaseshifts_version": phaseshifts_version(),
        "bulk": {
            "path": str(bulk.resolve()),
            "sha256": sha256sum(bulk),
        },
        "slab": {
            "path": str(slab.resolve()),
            "sha256": sha256sum(slab),
        },
        "command": cmd,
        "cleed_phase": str(output_dir.resolve()),
        "outputs": phs_files,
    }

    with manifest_path.open("w", encoding="utf-8") as handle:
        json.dump(manifest, handle, indent=2, sort_keys=True)
        handle.write("\n")

    print(f"Wrote phase shifts to {output_dir}")
    print(f"Manifest: {manifest_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
