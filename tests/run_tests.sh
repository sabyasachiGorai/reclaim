#!/usr/bin/env bash
# Usage: tests/run_tests.sh path/to/reclaim
# Builds a throwaway folder and checks duplicate detection and cleanup.
set -u
BIN="$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
DIR="$(mktemp -d)"
trap 'rm -rf "$DIR"' EXIT
FAIL=0

check() { # check <description> <command...>
    local desc="$1"; shift
    if "$@"; then echo "PASS: $desc"; else echo "FAIL: $desc"; FAIL=1; fi
}

mkdir -p "$DIR/data/sub"
printf 'same content' > "$DIR/data/a.txt"
printf 'same content' > "$DIR/data/sub/renamed.txt"   # identical, different name
printf 'AAAA' > "$DIR/data/x.txt"
printf 'BBBB' > "$DIR/data/y.txt"                      # same size, different content
ln -s "$DIR/data/a.txt" "$DIR/data/link.txt"          # symlink must be ignored

OUT="$("$BIN" "$DIR/data")"
DUPES="$(sed -n '/^Group 1/,$p' <<<"$OUT")"   # duplicate section only

check "identical files are one group"      grep -q "Group 1: 2 copies" <<<"$OUT"
check "exactly one group found"            grep -q "Duplicate groups: 1" <<<"$OUT"
check "same size, different content apart" bash -c '! grep -q -e x.txt -e y.txt <<<"$0"' "$DUPES"
check "symlink is ignored"                 bash -c '! grep -q link.txt <<<"$0"' "$OUT"
check "wasted space is reported"           grep -q "reclaimed: 12 B" <<<"$OUT"

# --clean: garbage input is rejected, then choice 1 keeps one copy and deletes the other
printf 'garbage\n99\n1\n' | "$BIN" "$DIR/data" --clean >/dev/null
KEPT=$(ls "$DIR/data/a.txt" "$DIR/data/sub/renamed.txt" 2>/dev/null | wc -l)
check "--clean keeps exactly one copy"     test "$KEPT" -eq 1
check "--clean leaves other files alone"   test -f "$DIR/data/x.txt" -a -f "$DIR/data/y.txt"

exit $FAIL
