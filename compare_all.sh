#!/bin/bash

MY=./compiler
REF=./A3-sclp
TESTDIR=example-programs/L3/L3-testcases

MY_OUT=/tmp/my_ast
REF_OUT=/tmp/ref_ast

rm -rf $MY_OUT $REF_OUT
mkdir -p $MY_OUT $REF_OUT

echo "Running and comparing tokens..."
echo "--------------------------------"

for f in $TESTDIR/*.c; do
    base=$(basename "$f" .c)

    #run my compiler
    $MY --show-ast "$f" > /dev/null
    mv "$f.ast" "$MY_OUT/$base.ast"

    #run reference
    $REF --show-ast "$f" > /dev/null
    mv "$f.ast" "$REF_OUT/$base.ast"

    if diff -b -w "$MY_OUT/$base.ast" "$REF_OUT/$base.ast" > /dev/null; then
        echo "$base : OK"
    else
        echo "$base : DIFFER"
        diff -b -w "$MY_OUT/$base.ast" "$REF_OUT/$base.ast"
        echo "----------------------------------------"
    fi
done