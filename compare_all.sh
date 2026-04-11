#!/bin/bash

MY=./old-compiler
REF=./new-compiler
TESTDIR=example-programs/Level-1-test-cases
MY_OUT=/tmp/my_tac
REF_OUT=/tmp/ref_tac

rm -rf $MY_OUT $REF_OUT
mkdir -p $MY_OUT $REF_OUT

echo "Running and comparing tokens..."
echo "--------------------------------"

for f in $TESTDIR/*.c; do
    base=$(basename "$f" .c)

    #run my compiler
    $MY --show-tac "$f" > /dev/null
    mv "$f.tac" "$MY_OUT/$base.tac"

    #run reference
    $REF --show-tac "$f" > /dev/null
    mv "$f.tac" "$REF_OUT/$base.tac"

    if diff -Bw "$MY_OUT/$base.tac" "$REF_OUT/$base.tac" > /dev/null; then
        echo "$base : OK"
    else
        echo "$base : DIFFER"
        diff -Bw "$MY_OUT/$base.tac" "$REF_OUT/$base.tac"
        echo "----------------------------------------"
    fi
done