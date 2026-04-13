#!/bin/bash

# Master comparison script: runs all individual comparison scripts
# listed in the project (tokens, AST, TAC, RTL, ASM, etc.).

# Do NOT use 'set -e' here; we want to run
# all comparison scripts even if some fail,
# and then report a full summary at the end.

SCRIPTS=(
  "compare_asm.sh"
  "compare_ast_l5.sh"
  "compare_ast.sh"
  "compare_l1to3_ast.sh"
  "compare_l1to3_tac.sh"
  "compare_l1to3_tokens.sh"
  "compare_rtl_l5.sh"
  "compare_rtl.sh"
  "compare_tac_l4.sh"
  "compare_tac.sh"
)

PASS_SCRIPTS=()
FAIL_SCRIPTS=()
SKIP_SCRIPTS=()

for script in "${SCRIPTS[@]}"; do
  if [[ -x "$script" ]]; then
    echo "============================================="
    echo "Running $script..."
    echo "============================================="

    ./$script
    exit_code=$?

    if [[ $exit_code -eq 0 ]]; then
      PASS_SCRIPTS+=("$script")
    else
      FAIL_SCRIPTS+=("$script (exit $exit_code)")
    fi

  elif [[ -f "$script" ]]; then
    echo "[WARN] $script exists but is not executable; skipping."
    SKIP_SCRIPTS+=("$script (not executable)")
  else
    echo "[WARN] $script not found; skipping."
    SKIP_SCRIPTS+=("$script (missing)")
  fi

  echo ""
  echo "---------------------------------------------"
  echo ""
done

echo ""
echo "============= COMPARISON SCRIPT SUMMARY ============="

if [[ ${#PASS_SCRIPTS[@]} -gt 0 ]]; then
  echo "PASSED (exit code 0):"
  for s in "${PASS_SCRIPTS[@]}"; do
    echo "  - $s"
  done
  echo ""
fi

if [[ ${#FAIL_SCRIPTS[@]} -gt 0 ]]; then
  echo "FAILED (non-zero exit):"
  for s in "${FAIL_SCRIPTS[@]}"; do
    echo "  - $s"
  done
  echo ""
fi

if [[ ${#SKIP_SCRIPTS[@]} -gt 0 ]]; then
  echo "SKIPPED:"
  for s in "${SKIP_SCRIPTS[@]}"; do
    echo "  - $s"
  done
  echo ""
fi

total=${#SCRIPTS[@]}
pass=${#PASS_SCRIPTS[@]}
fail=${#FAIL_SCRIPTS[@]}
skip=${#SKIP_SCRIPTS[@]}

echo "Total scripts: $total  |  Passed: $pass  Failed: $fail  Skipped: $skip"
echo "======================================================"

if [[ $fail -eq 0 ]]; then
  exit 0
else
  exit 1
fi
