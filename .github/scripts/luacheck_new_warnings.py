#!/usr/bin/env python3
"""List the luacheck warnings that the current commit adds to the Lua files it changes.

Usage: python3 .github/scripts/luacheck_new_warnings.py BASE [--luacheck CMD]

Run from the repository root with the head commit checked out. BASE is the commit to
compare with (in a pull request job, HEAD^1 of the merge commit). luacheck runs over the
whole tree twice, on HEAD and on BASE, both with HEAD's .luacheckrc: with
allow_defined_top a global defined at the top of one file counts as defined in every
file of the same run, so both sides need the same set of files.

A warning is new when a changed file has more of it (same code and message) than the
file had at BASE. Line numbers are left out of the comparison, so moved code is not
reported. Warnings on lines the commit adds are listed first.

This only reports. It exits 0 whatever it finds, and 1 only if it cannot run.
"""

import argparse
import collections
import io
import os
import re
import shlex
import subprocess
import sys
import tarfile
import tempfile

# Codes that usually mean a real bug get annotations in the pull request;
# the rest are listed in the log and the job summary only.
ANNOTATE = ("E", "W111", "W112", "W113", "W121", "W122", "W143")
# LuaCATS holds editor type stubs, not game code. UI_bc1 is EUI as bc1 ships it and is
# not edited in this repository (changes go into a copy under "(3a) VP - EUI
# Compatibility Files"). Both stay in the luacheck run, because they define globals.
SKIP = ("LuaCATS/", "UI_bc1/")
MAX_ANNOTATIONS = 10  # GitHub shows at most 10 warning annotations per step

LINE = re.compile(r"^(.*?):(\d+):(\d+): \(([EW]\d+)\) (.*)$")


def git(*args):
    return subprocess.run(("git",) + args, check=True, stdout=subprocess.PIPE).stdout


def changed_files(base):
    """{path at HEAD: path at BASE or None for a new file} for changed .lua files."""
    out = git("diff", "-z", "--name-status", "-M", base, "HEAD", "--", "*.lua")
    parts = out.decode("utf-8").split("\0")
    result, i = {}, 0
    while i < len(parts) and parts[i]:
        status = parts[i][0]
        if status in "RC":
            result[parts[i + 2]] = parts[i + 1]
            i += 3
        else:
            if status != "D":
                result[parts[i + 1]] = None if status == "A" else parts[i + 1]
            i += 2
    return {f: old for f, old in result.items() if not f.startswith(SKIP)}


def added_lines(base, path, old):
    """Line numbers at HEAD that the commit adds or changes in path."""
    spec = [old, path] if old and old != path else [path]
    out = git("diff", "-U0", "-M", base, "HEAD", "--", *spec).decode("utf-8", "replace")
    lines = set()
    for m in re.finditer(r"^@@ -\S+ \+(\d+)(?:,(\d+))? @@", out, re.M):
        start, count = int(m.group(1)), int(m.group(2) or 1)
        lines.update(range(start, start + count))
    return lines


def extract_base(base, dest):
    data = git("archive", "--format=tar", base, "--", "*.lua")
    with tarfile.open(fileobj=io.BytesIO(data)) as tar:
        if hasattr(tarfile, "data_filter"):
            tar.extractall(dest, filter="data")
        else:
            tar.extractall(dest)
    with open(".luacheckrc", "rb") as src, open(os.path.join(dest, ".luacheckrc"), "wb") as dst:
        dst.write(src.read())


def start_luacheck(cmd, cwd):
    args = shlex.split(cmd) + [".", "--formatter", "plain", "--codes", "--no-color"]
    return subprocess.Popen(args, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)


def finish_luacheck(proc, side):
    out, err = proc.communicate()
    if proc.returncode > 2:  # 0 clean, 1 warnings, 2 errors; 3 and 4 mean luacheck failed
        sys.stderr.write(err.decode("utf-8", "replace"))
        raise SystemExit(f"luacheck failed on {side} (exit code {proc.returncode})")
    warnings = collections.defaultdict(list)
    for line in out.decode("utf-8", "replace").splitlines():
        m = LINE.match(line)
        if m:
            path, ln, col, code, msg = m.groups()
            path = path.replace("\\", "/")
            path = path[2:] if path.startswith("./") else path
            key = (path, code, re.sub(r"line \d+", "line N", msg))
            warnings[key].append((int(ln), int(col), msg))
    return warnings


def escape(value):
    return value.replace("%", "%25").replace("\r", "%0D").replace("\n", "%0A") \
                .replace(":", "%3A").replace(",", "%2C")


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("base", help="commit to compare with, e.g. HEAD^1")
    ap.add_argument("--luacheck", default=os.environ.get("LUACHECK", "luacheck"),
                    help="luacheck command (default: luacheck, or $LUACHECK)")
    opts = ap.parse_args()

    base = git("rev-parse", "--verify", opts.base + "^{commit}").decode().strip()
    changed = changed_files(base)
    if not changed:
        print("No Lua files changed outside LuaCATS/ and UI_bc1/.")
        return 0

    with tempfile.TemporaryDirectory() as tmp:
        extract_base(base, tmp)
        head_proc = start_luacheck(opts.luacheck, ".")
        base_proc = start_luacheck(opts.luacheck, tmp)
        head = finish_luacheck(head_proc, "this commit")
        before = finish_luacheck(base_proc, "the base")

    new, fixed, partial = [], 0, {}
    for path, old in sorted(changed.items()):
        touched = added_lines(base, path, old)
        keys = {k for k in head if k[0] == path}
        if old:
            keys |= {(path, c, m) for (p, c, m) in before if p == old}
        # After a syntax error (E011) luacheck reports nothing else for the file, so that
        # side's counts mean nothing. Broken here: list the errors, and other warnings only
        # on changed lines. Broken at the base: list every bug-like warning, and the rest
        # only on changed lines.
        broken_head = any(k[1] == "E011" for k in head if k[0] == path)
        broken_base = any(k[1] == "E011" for k in before if k[0] == old)
        if broken_head or broken_base:
            partial[path] = "this change" if broken_head else "the base"
        for key in sorted(keys):
            count_head = len(head.get(key, ()))
            count_base = len(before.get((old, key[1], key[2]), ())) if old else 0
            if (broken_head or broken_base) and not (broken_head and key[1].startswith("E")):
                keep_all = broken_base and not broken_head and key[1].startswith(ANNOTATE)
                for ln, col, msg in head.get(key, ()):
                    if keep_all or ln in touched:
                        new.append((path, ln, col, key[1], msg))
            elif count_head < count_base:
                fixed += count_base - count_head
            elif count_head > count_base:
                # Prefer the warnings on lines this commit wrote, then the last ones.
                found = sorted(head[key], key=lambda w: (w[0] in touched, w[0]))
                for ln, col, msg in found[count_base - count_head:]:
                    new.append((path, ln, col, key[1], msg))
    new.sort(key=lambda w: (w[0], w[1], w[2]))
    for path, side in partial.items():
        print(f"{path}: does not parse at {side}, so its warnings cannot be compared"
              + ("." if side == "this change" else
                 "; listing its bug-like warnings and the warnings on changed lines."))

    annotated = 0
    for path, ln, col, code, msg in new:
        print(f"{path}:{ln}:{col}: ({code}) {msg}")
        if code.startswith(ANNOTATE) and annotated < MAX_ANNOTATIONS:
            annotated += 1
            print(f"::warning file={escape(path)},line={ln},col={col},"
                  f"title=luacheck {code}::{escape(msg)}")
    if annotated == MAX_ANNOTATIONS:
        more = sum(1 for w in new if w[3].startswith(ANNOTATE)) - annotated
        if more:
            print(f"{more} more warning(s) of the annotated codes are in the log and the job summary.")
    print(f"{len(new)} new luacheck warning(s), {fixed} fixed, "
          f"in {len(changed)} changed Lua file(s).")

    summary = os.environ.get("GITHUB_STEP_SUMMARY")
    if summary:
        with open(summary, "a", encoding="utf-8") as s:
            s.write("### luacheck: warnings this pull request adds (report only)\n\n")
            s.write(f"{len(new)} new, {fixed} fixed, in {len(changed)} changed Lua file(s).\n\n")
            if new:
                s.write("| File | Line | Code | Message |\n|---|---|---|---|\n")
                for path, ln, col, code, msg in new:
                    msg = msg.replace("|", "\\|")
                    s.write(f"| `{path}` | {ln} | {code} | {msg} |\n")
                s.write("\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
