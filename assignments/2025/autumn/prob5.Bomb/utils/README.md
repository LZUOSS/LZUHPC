# Utility Scripts

Helper scripts for the Bomb Project.

## benchmark_perlin.py

Benchmark script for perlin module. Called automatically by `make perlin-bench`.

**Usage:**
```bash
python3 utils/benchmark_perlin.py <size> <seed> <baseline_bin> <submit_bin>
```

## benchmark_splat.py

Benchmark script for splat module. Called automatically by `make splat-bench`.

**Usage:**
```bash
python3 utils/benchmark_splat.py <size> <seed> <baseline_bin> <submit_bin> [heightmap]
```

## track_performance.py

Track and compare optimization progress over time.

**Usage:**
```bash
# Save current performance
python3 utils/track_performance.py save S "optimized loops"

# View history
python3 utils/track_performance.py show

# Compare first vs latest
python3 utils/track_performance.py compare
```

**Features:**
- Records speedup for each optimization
- Shows optimization history
- Compares progress over time
- Stores data in `.performance_log.json`

## aliases.sh

Command aliases for faster typing.

**Usage:**
```bash
source utils/aliases.sh
```

**Available aliases:**
- `bd` = `make dev`
- `bq` = `make quick`
- `bc` = `make check`
- `bb` = `make benchmark`
- `bt` = `make test`
- `bv` = `make validate`
- `br` = `make run`
- `bp` = `make perlin-bench`
- `bs` = `make splat-bench`
- `bh` = `make help`
- `bclean` = `make clean`

**Functions:**
- `bsize M` = `make dev SIZE=M`
- `bbench L` = `make benchmark SIZE=L`

## Notes

- These scripts are called automatically by Makefile targets
- You typically don't need to run them directly
- See main README.md for usage examples
