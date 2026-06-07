#!/bin/bash

set -euo pipefail

status=true
mkdir -p .itest

run_case()
{
    local name="$1"
    local input=".itest/${name}.input.csv"
    local expected=".itest/${name}.expected.csv"
    local actual=".itest/${name}.actual.csv"

    ./gauss "$input" > "$actual"

    if cmp -s "$expected" "$actual"; then
        echo "Integration test '${name}' passed"
    else
        echo "Integration test '${name}' failed"
        diff -u "$expected" "$actual" || true
        status=false
    fi
}

cat > .itest/small2x2.input.csv <<'CSV'
A0,A1,B
2,1,5
1,3,7
CSV

cat > .itest/small2x2.expected.csv <<'CSV'
x
1.600000
1.800000
CSV

run_case small2x2

cat > .itest/classic3x3.input.csv <<'CSV'
A0,A1,A2,B
2,1,-1,8
-3,-1,2,-11
-2,1,2,-3
CSV

cat > .itest/classic3x3.expected.csv <<'CSV'
x
2.000000
3.000000
-1.000000
CSV

run_case classic3x3

$status
