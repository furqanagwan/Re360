"""Run clang-tidy on the lines a change adds or modifies.

Reads a unified diff (`git diff -U0 BASE...HEAD`) on stdin and runs clang-tidy
once per changed file that is in the compilation database, limited to the
changed lines. Any diagnostic fails the run, so new code follows the naming and
bug checks in .clang-tidy while untouched code is left alone. ReGPU's renderer
(src/regpu, apart from its plugin glue) keeps its own style (ADR-018) and is
left out, as it is from clang-format.

LLVM's clang-tidy-diff.py does the same, but names files with forward slashes in
its line filter; clang-tidy reports Windows paths with backslashes, so the filter
never matches there. This script passes each file's native absolute path.
"""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

HUNK = re.compile(r"^@@ -\d+(?:,\d+)? \+(\d+)(?:,(\d+))? @@")
NEW_FILE = re.compile(r"^\+\+\+ (?:b/)?(.+?)\s*$")


def changed_lines(diff: str) -> dict[str, list[list[int]]]:
    """Map each file in a -U0 diff to the [first, last] line ranges it adds."""
    lines: dict[str, list[list[int]]] = {}
    current: str | None = None
    for line in diff.splitlines():
        new_file = NEW_FILE.match(line)
        if new_file:
            name = new_file.group(1)
            current = None if name == "/dev/null" else name
            continue
        hunk = HUNK.match(line)
        if hunk and current:
            start = int(hunk.group(1))
            count = int(hunk.group(2)) if hunk.group(2) is not None else 1
            if count:
                lines.setdefault(current, []).append([start, start + count - 1])
    return lines


def compiled_files(build_path: Path) -> set[Path]:
    with open(build_path / "compile_commands.json", encoding="utf-8") as db:
        return {Path(entry["file"]).resolve() for entry in json.load(db) if "file" in entry}


def tidy_command(clang_tidy: str, build_path: Path, path: Path, ranges: list[list[int]]) -> list[str]:
    line_filter = json.dumps([{"name": str(path), "lines": ranges}], separators=(",", ":"))
    return [
        clang_tidy,
        f"-p={build_path}",
        f"-line-filter={line_filter}",
        "-warnings-as-errors=*",
        "--quiet",
        str(path),
    ]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--build", required=True, type=Path, help="folder with compile_commands.json")
    parser.add_argument("--clang-tidy", default="clang-tidy")
    parser.add_argument("--include", default=r"(src|include|tests/unit|tests/gpu)/.*\.(cpp|h)$",
                        help="regex the changed paths must match")
    parser.add_argument("--exclude", default=r"src/regpu/(?!plugin/)",
                        help="regex of changed paths that keep their own style")
    parser.add_argument("-j", "--jobs", type=int, default=4)
    args = parser.parse_args()

    build_path = args.build.resolve()
    in_database = compiled_files(build_path)
    include = re.compile(args.include)
    exclude = re.compile(args.exclude) if args.exclude else None
    commands = []
    for name, ranges in changed_lines(sys.stdin.read()).items():
        path = Path(name).resolve()
        if include.match(name) and not (exclude and exclude.match(name)) and path in in_database:
            commands.append(tidy_command(args.clang_tidy, build_path, path, ranges))
    if not commands:
        print("No changed lines in compiled files.")
        return 0

    def run(command: list[str]) -> tuple[int, str]:
        result = subprocess.run(command, capture_output=True, text=True)
        return result.returncode, result.stdout + result.stderr

    failed = 0
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        for code, output in pool.map(run, commands):
            if code:
                failed += 1
                print(output)
    print(f"clang-tidy: {len(commands)} file(s) checked, {failed} with findings on changed lines.")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
