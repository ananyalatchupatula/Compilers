#!/bin/bash

MY=./old-compiler
REF=./new-compiler

MY_OUT=/tmp/my_tac
REF_OUT=/tmp/ref_tac

rm -rf "$MY_OUT" "$REF_OUT"
mkdir -p "$MY_OUT" "$REF_OUT"

echo "Running and comparing TAC outputs for Level-1 to Level-3..."
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
        $MY --show-tac "$f" > /dev/null
        mv "$f.tac" "$MY_OUT/${base}.tac"

        # run reference
        $REF --show-tac "$f" > /dev/null
        mv "$f.tac" "$REF_OUT/${base}.tac"

        if diff -Bw "$MY_OUT/${base}.tac" "$REF_OUT/${base}.tac" > /dev/null; then
            echo "  $base : OK"
        else
            echo "  $base : DIFFER"
            diff -Bw "$MY_OUT/${base}.tac" "$REF_OUT/${base}.tac"
            echo "  ----------------------------------------"
        fi
    done

    echo "-----------------------------------------------------------"
done