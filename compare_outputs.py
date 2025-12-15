#!/usr/bin/env python3
"""Compare program outputs in `public/outNN.txt` to expected `public/tNN.txt`.

Usage:
  python tools/compare_outputs.py [--folder public] [--start 1] [--end 14] [--csv report.csv]

Prints a table with: index, out file, expected file, exact match (bool), normalized match (bool).
If files differ, shows a short snippet of program output and expected output.
"""

from pathlib import Path
import argparse
import csv
import textwrap
import sys

TRUNC = 300


def read_text(path: Path) -> str:
    if not path.exists():
        return None
    return path.read_text(encoding="utf-8", errors="replace")


def normalize_text(s: str):
    # normalize by stripping trailing whitespace on each line
    return "\n".join(line.rstrip() for line in s.splitlines())


def short_snippet(s: str):
    if s is None:
        return "<missing>"
    s = s.replace('\r\n', '\n').replace('\r', '\n')
    s = s.strip('\n')
    if len(s) <= TRUNC:
        return s
    return s[:TRUNC] + '...'


def compare_folder(folder: Path, start: int, end: int):
    rows = []
    for i in range(start, end + 1):
        out_file = folder / f"out{i:02d}.txt"
        t_file = folder / f"t{i:02d}.txt"
        out_txt = read_text(out_file)
        t_txt = read_text(t_file)
        exact = (out_txt == t_txt)
        normalized = False
        if out_txt is None or t_txt is None:
            normalized = False
        else:
            normalized = normalize_text(out_txt) == normalize_text(t_txt)
        rows.append({
            "index": i,
            "out_file": str(out_file),
            "t_file": str(t_file),
            "exact": exact,
            "normalized": normalized,
            "out_snip": short_snippet(out_txt) if not exact else "",
            "t_snip": short_snippet(t_txt) if not exact else "",
        })
    return rows


def print_table(rows):
    # header
    print(f{"Index":<6} {"Exact":<7} {"Normalized":<11} {"Out file":<40} {"T file"})
    print("-" * 100)
    for r in rows:
        print(f"{r['index']:<6} {str(r['exact']):<7} {str(r['normalized']):<11} {r['out_file']:<40} {r['t_file']}")
        if not r['exact']:
            print("  Program output:\n" + textwrap.indent(r['out_snip'], "    "))
            print("  Expected output:\n" + textwrap.indent(r['t_snip'], "    "))
            print("" )


def write_csv(path: Path, rows):
    with path.open("w", newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        writer.writerow(["index", "out_file", "t_file", "exact", "normalized", "out_snip", "t_snip"])
        for r in rows:
            writer.writerow([r['index'], r['out_file'], r['t_file'], r['exact'], r['normalized'], r['out_snip'], r['t_snip']])


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("--folder", default="public", help="Folder containing outNN.txt and tNN.txt")
    parser.add_argument("--start", type=int, default=1)
    parser.add_argument("--end", type=int, default=14)
    parser.add_argument("--csv", help="Write results to CSV file")
    args = parser.parse_args(argv)

    folder = Path(args.folder)
    if not folder.exists():
        print(f"Folder not found: {folder}")
        return 2

    rows = compare_folder(folder, args.start, args.end)
    print_table(rows)
    if args.csv:
        write_csv(Path(args.csv), rows)
        print(f"Wrote CSV to {args.csv}")
    # return non-zero if any exact mismatches
    any_mismatch = any(not r['exact'] for r in rows)
    return 1 if any_mismatch else 0


if __name__ == '__main__':
    raise SystemExit(main(sys.argv[1:]))
