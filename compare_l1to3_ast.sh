#!/bin/bash

MY=./old-compiler
REF=./new-compiler

MY_OUT=/tmp/my_ast
REF_OUT=/tmp/ref_ast

rm -rf "$MY_OUT" "$REF_OUT"
mkdir -p "$MY_OUT" "$REF_OUT"

echo "Running and comparing ast outputs for Level-1 to Level-3..."
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
        $MY --show-ast "$f" > /dev/null
        mv "$f.ast" "$MY_OUT/${base}.ast"

        # run reference
        $REF --show-ast "$f" > /dev/null
        mv "$f.ast" "$REF_OUT/${base}.ast"

        if diff -Bw "$MY_OUT/${base}.ast" "$REF_OUT/${base}.ast" > /dev/null; then
            echo "  $base : OK"
        else
            echo "  $base : DIFFER"
            diff -Bw "$MY_OUT/${base}.ast" "$REF_OUT/${base}.ast"
            echo "  ----------------------------------------"
        fi
    done

    echo "-----------------------------------------------------------"
done