#!/bin/bash

MY=./old-compiler
REF=./new-compiler

MY_OUT=/tmp/my_toks
REF_OUT=/tmp/ref_toks

rm -rf "$MY_OUT" "$REF_OUT"
mkdir -p "$MY_OUT" "$REF_OUT"

echo "Running and comparing toks outputs for Level-1 to Level-3..."
echo "-----------------------------------------------------------"

for level in 1 2 3; do
    TESTDIR="example-programs/Level-${level}-test-cases"

    if [ ! -d "$TESTDIR" ]; then
        echo "Skipping Level-$level (directory $TESTDIR not found)"
        echo "-----------------------------------------------------------"
        continue
    fi

    echo "Level-$level:"

    for f in "$TESTDIR"/*.c; do
        base=$(basename "$f" .c)

        # run my compiler
        $MY --show-tokens "$f" > /dev/null
        mv "$f.toks" "$MY_OUT/${base}.toks"

        # run reference
        $REF --show-tokens "$f" > /dev/null
        mv "$f.toks" "$REF_OUT/${base}.toks"

        if diff -Bw "$MY_OUT/${base}.toks" "$REF_OUT/${base}.toks" > /dev/null; then
            echo "  $base : OK"
        else
            echo "  $base : DIFFER"
            diff -Bw "$MY_OUT/${base}.toks" "$REF_OUT/${base}.toks"
            echo "  ----------------------------------------"
        fi
    done

    echo "-----------------------------------------------------------"
done