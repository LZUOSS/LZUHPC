#!/bin/bash
# Bomb Project - Command Aliases
# Usage: source utils/aliases.sh

# Quick commands
alias bd='make dev'           # bomb dev
alias bq='make quick'         # bomb quick
alias bc='make check'         # bomb check
alias bb='make benchmark'     # bomb benchmark
alias bt='make test'          # bomb test
alias bv='make validate'      # bomb validate
alias br='make run'           # bomb run

# Single module commands
alias bp='make perlin-bench'  # bomb perlin
alias bs='make splat-bench'   # bomb splat

# Utility commands
alias bh='make help'          # bomb help
alias bclean='make clean'     # bomb clean

# Functions with parameters
bsize() {
    # Usage: bsize M  (test with size M)
    make dev SIZE=$1
}

bbench() {
    # Usage: bbench M  (benchmark with size M)
    make benchmark SIZE=$1
}

echo "✓ Bomb Project aliases loaded!"
echo ""
echo "Quick commands:"
echo "  bd      = make dev"
echo "  bq      = make quick"
echo "  bc      = make check"
echo "  bb      = make benchmark"
echo ""
echo "With parameters:"
echo "  bsize M  = make dev SIZE=M"
echo "  bbench L = make benchmark SIZE=L"
echo ""
echo "View all: bh"
