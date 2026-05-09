MAKEFLAGS += --no-print-directory

CXX ?= g++

PERLIN_DIR := perlin
SPLAT_DIR := splat
VALIDATE_DIR := validate

SIZE ?= S
SEED ?= 42
BENCH_HEIGHTMAP ?=
TERRAIN_DATA_FILE := terrain.raw

PERLIN_BASELINE_RAW := $(VALIDATE_DIR)/terrain_baseline.raw
PERLIN_SUBMIT_RAW := $(VALIDATE_DIR)/terrain_submit.raw
SPLAT_BASELINE_RAW := $(VALIDATE_DIR)/occlusion_baseline.raw
SPLAT_SUBMIT_RAW := $(VALIDATE_DIR)/occlusion_submit.raw

VALIDATE_BIN := $(VALIDATE_DIR)/validate
VALIDATE_SRC := $(VALIDATE_DIR)/validate.cpp
VALIDATE_FLAGS ?= -O3 -std=c++17

DIFF_BIN := $(VALIDATE_DIR)/diff_tool
DIFF_SRC := $(VALIDATE_DIR)/diff.cpp

.PHONY: help all perlin splat test run visualize benchmark validate diff clean
.PHONY: dev quick perlin-test splat-test perlin-bench splat-bench check

help:
	@echo "\033[1;36m╔════════════════════════════════════════════════════════════════╗\033[0m"
	@echo "\033[1;36m║           Bomb Project - Performance Optimization             ║\033[0m"
	@echo "\033[1;36m╚════════════════════════════════════════════════════════════════╝\033[0m"
	@echo ""
	@echo "\033[1;33mQuick Commands:\033[0m"
	@echo "  \033[1;32mmake dev\033[0m              Fast dev workflow (build→test S→benchmark)"
	@echo "  \033[1;32mmake quick\033[0m            Quick validation (test S only)"
	@echo "  \033[1;32mmake check\033[0m            Full check (test+validate+benchmark)"
	@echo ""
	@echo "\033[1;33mBasic Commands:\033[0m"
	@echo "  \033[32mmake all\033[0m              Build all modules"
	@echo "  \033[32mmake test\033[0m             Run full tests (S/M/L)"
	@echo "  \033[32mmake benchmark\033[0m        Performance comparison"
	@echo "  \033[32mmake validate\033[0m         Validate output correctness"
	@echo "  \033[32mmake run\033[0m              Run full pipeline"
	@echo ""
	@echo "\033[1;33mSingle Module:\033[0m"
	@echo "  \033[32mmake perlin-test\033[0m      Test perlin only"
	@echo "  \033[32mmake splat-test\033[0m       Test splat only"
	@echo "  \033[32mmake perlin-bench\033[0m     Benchmark perlin only"
	@echo "  \033[32mmake splat-bench\033[0m      Benchmark splat only"
	@echo ""
	@echo "\033[1;33mOther:\033[0m"
	@echo "  \033[32mmake visualize\033[0m        Generate visualization images"
	@echo "  \033[32mmake clean\033[0m            Clean all generated files"
	@echo ""
	@echo "\033[1;33mParameters:\033[0m"
	@echo "  SIZE={S|M|L}          Set test size (default: S)"
	@echo "  SEED=<number>         Set random seed (default: 42)"
	@echo ""
	@echo "\033[1;33mExamples:\033[0m"
	@echo "  make dev              # Fast development test"
	@echo "  make benchmark SIZE=M # Medium size benchmark"
	@echo "  make check            # Pre-submission check"
	@echo ""

all: perlin splat

perlin:
	@$(MAKE) -C $(PERLIN_DIR) all

splat:
	@$(MAKE) -C $(SPLAT_DIR) all

test:
	@echo "\033[32m--- Running perlin test suite ---\033[0m"
	@$(MAKE) -C $(PERLIN_DIR) test
	@echo "\033[32m--- Running splat test suite ---\033[0m"
	@$(MAKE) -C $(SPLAT_DIR) test

run: perlin splat
	@echo "\n\033[32m--- [PHASE 1] Generating terrain via submission ---\033[0m"
	@$(MAKE) -C $(PERLIN_DIR) raw_output RAW_OUTPUT_SIZE=$(SIZE) RAW_OUTPUT_FILE=../$(TERRAIN_DATA_FILE)
	@echo "\n\033[32m--- [PHASE 2] Running splat submission on generated terrain ---\033[0m"
	@$(MAKE) -C $(SPLAT_DIR) run SIZE=$(SIZE) SEED=$(SEED) HEIGHTMAP=../$(TERRAIN_DATA_FILE)

visualize: perlin splat
	@echo "\n\033[32m--- [VISUALIZE] Rendering terrain colour map ---\033[0m"
	@$(MAKE) -C $(PERLIN_DIR) visualize SIZE=$(SIZE) SEED=$(SEED)
	@mv $(PERLIN_DIR)/terrain_S.ppm terrain_color.ppm
	@echo "\n\033[32m--- [VISUALIZE] Rendering splat debug maps (flat terrain) ---\033[0m"
	@$(MAKE) -C $(SPLAT_DIR) visualize
	@mv $(SPLAT_DIR)/occlusion_S.ppm occlusion_flat.ppm

benchmark: perlin splat
	@echo "\033[32m=== Perlin benchmark ===\033[0m"
	@$(MAKE) -C $(PERLIN_DIR) benchmark BENCH_SIZE=$(SIZE) BENCH_SEED=$(SEED)
	@echo
	@echo "\033[32m=== Splat benchmark ===\033[0m"
	@$(MAKE) -C $(SPLAT_DIR) benchmark BENCH_SIZE=$(SIZE) BENCH_SEED=$(SEED) BENCH_HEIGHTMAP=$(BENCH_HEIGHTMAP)

diff: $(DIFF_BIN)
	@echo "\033[32m--- Built PPM diff helper: $@ ---\033[0m"

$(DIFF_BIN): $(DIFF_SRC)
	@echo "\033[32m--- Compiling $(DIFF_BIN) helper ---\033[0m"
	@$(CXX) $(VALIDATE_FLAGS) $< -o $@

validate: $(VALIDATE_BIN)
ifneq ($(strip $(BASELINE)$(OUTPUT)),)
	@echo "\033[32m--- Validating $(OUTPUT) against $(BASELINE) ---\033[0m"
	@$(VALIDATE_BIN) "$(BASELINE)" "$(OUTPUT)"
else
	@comp="$(COMPONENT)"; \
	if [ -z "$$comp" ] || [ "$$comp" = "all" ]; then \
		echo "\033[32m--- Validating perlin (size $(SIZE)) ---\033[0m"; \
		$(MAKE) -C $(PERLIN_DIR) raw_output_baseline RAW_OUTPUT_SIZE=$(SIZE) RAW_BASELINE_FILE=../$(PERLIN_BASELINE_RAW); \
		$(MAKE) -C $(PERLIN_DIR) raw_output RAW_OUTPUT_SIZE=$(SIZE) RAW_OUTPUT_FILE=../$(PERLIN_SUBMIT_RAW); \
		$(VALIDATE_BIN) $(PERLIN_BASELINE_RAW) $(PERLIN_SUBMIT_RAW); \
		echo "\n\033[32m--- Validating splat (size $(SIZE), seed $(SEED)) ---\033[0m"; \
		$(MAKE) -C $(SPLAT_DIR) raw_output_baseline SIZE=$(SIZE) SEED=$(SEED) HEIGHTMAP=../$(PERLIN_SUBMIT_RAW) OCCLUSION_BASELINE_FILE=../$(SPLAT_BASELINE_RAW); \
		$(MAKE) -C $(SPLAT_DIR) raw_output SIZE=$(SIZE) SEED=$(SEED) HEIGHTMAP=../$(PERLIN_SUBMIT_RAW) OCCLUSION_SUBMIT_FILE=../$(SPLAT_SUBMIT_RAW); \
		$(VALIDATE_BIN) $(SPLAT_BASELINE_RAW) $(SPLAT_SUBMIT_RAW); \
	elif [ "$$comp" = "perlin" ]; then \
		echo "\033[32m--- Validating perlin (size $(SIZE)) ---\033[0m"; \
		$(MAKE) -C $(PERLIN_DIR) raw_output_baseline RAW_OUTPUT_SIZE=$(SIZE) RAW_BASELINE_FILE=../$(PERLIN_BASELINE_RAW); \
		$(MAKE) -C $(PERLIN_DIR) raw_output RAW_OUTPUT_SIZE=$(SIZE) RAW_OUTPUT_FILE=../$(PERLIN_SUBMIT_RAW); \
		$(VALIDATE_BIN) $(PERLIN_BASELINE_RAW) $(PERLIN_SUBMIT_RAW); \
	elif [ "$$comp" = "splat" ]; then \
		echo "\033[32m--- Preparing terrain for splat validation ---\033[0m"; \
		$(MAKE) -C $(PERLIN_DIR) raw_output RAW_OUTPUT_SIZE=$(SIZE) RAW_OUTPUT_FILE=../$(PERLIN_SUBMIT_RAW); \
		echo "\033[32m--- Validating splat (size $(SIZE), seed $(SEED)) ---\033[0m"; \
		$(MAKE) -C $(SPLAT_DIR) raw_output_baseline SIZE=$(SIZE) SEED=$(SEED) HEIGHTMAP=../$(PERLIN_SUBMIT_RAW) OCCLUSION_BASELINE_FILE=../$(SPLAT_BASELINE_RAW); \
		$(MAKE) -C $(SPLAT_DIR) raw_output SIZE=$(SIZE) SEED=$(SEED) HEIGHTMAP=../$(PERLIN_SUBMIT_RAW) OCCLUSION_SUBMIT_FILE=../$(SPLAT_SUBMIT_RAW); \
		$(VALIDATE_BIN) $(SPLAT_BASELINE_RAW) $(SPLAT_SUBMIT_RAW); \
	else \
		echo "\033[31mUnknown COMPONENT='$(COMPONENT)'. Expected all|perlin|splat.\033[0m"; \
		exit 1; \
	fi
endif

$(VALIDATE_BIN): $(VALIDATE_SRC)
	@echo "\033[32m--- Building validator ---\033[0m"
	@$(CXX) $(VALIDATE_FLAGS) $< -o $@

# Fast development workflow
dev: all
	@echo "\033[1;36m╔════════════════════════════════════════════════════════════════╗\033[0m"
	@echo "\033[1;36m║                  Fast Development Workflow                     ║\033[0m"
	@echo "\033[1;36m╚════════════════════════════════════════════════════════════════╝\033[0m"
	@echo ""
	@echo "\033[1;33m[1/3] Running quick tests (SIZE=S)...\033[0m"
	@$(MAKE) quick
	@echo ""
	@echo "\033[1;33m[2/3] Performance benchmark...\033[0m"
	@$(MAKE) benchmark SIZE=S
	@echo ""
	@echo "\033[1;32m✓ Development test complete!\033[0m"

# Quick validation (S size only)
quick: all
	@echo "\033[32m--- Quick test: Perlin (S) ---\033[0m"
	@$(MAKE) -C $(PERLIN_DIR) run SIZE=S SEED=42 2>&1 | grep -E "(hash=|time_sec=)" || true
	@echo "\033[32m--- Quick test: Splat (S) ---\033[0m"
	@$(MAKE) -C $(SPLAT_DIR) run SIZE=S SEED=42 2>&1 | grep -E "(hash=|time_sec=)" || true

# Full check (pre-submission)
check: all
	@echo "\033[1;36m╔════════════════════════════════════════════════════════════════╗\033[0m"
	@echo "\033[1;36m║                    Full Check Workflow                         ║\033[0m"
	@echo "\033[1;36m╚════════════════════════════════════════════════════════════════╝\033[0m"
	@echo ""
	@echo "\033[1;33m[1/3] Running full tests...\033[0m"
	@$(MAKE) test
	@echo ""
	@echo "\033[1;33m[2/3] Validating correctness...\033[0m"
	@$(MAKE) validate SIZE=S
	@echo ""
	@echo "\033[1;33m[3/3] Performance benchmark...\033[0m"
	@$(MAKE) benchmark SIZE=M
	@echo ""
	@echo "\033[1;32m╔════════════════════════════════════════════════════════════════╗\033[0m"
	@echo "\033[1;32m║  ✓ All checks passed! Ready for submission                    ║\033[0m"
	@echo "\033[1;32m╚════════════════════════════════════════════════════════════════╝\033[0m"

# Single module tests
perlin-test: perlin
	@echo "\033[32m--- Testing Perlin module ---\033[0m"
	@$(MAKE) -C $(PERLIN_DIR) test

splat-test: splat
	@echo "\033[32m--- Testing Splat module ---\033[0m"
	@$(MAKE) -C $(SPLAT_DIR) test

# Single module benchmarks
perlin-bench: perlin
	@echo "\033[32m=== Perlin benchmark ===\033[0m"
	@$(MAKE) -C $(PERLIN_DIR) benchmark BENCH_SIZE=$(SIZE) BENCH_SEED=$(SEED)

splat-bench: splat
	@echo "\033[32m=== Splat benchmark ===\033[0m"
	@$(MAKE) -C $(SPLAT_DIR) benchmark BENCH_SIZE=$(SIZE) BENCH_SEED=$(SEED) BENCH_HEIGHTMAP=$(BENCH_HEIGHTMAP)

clean:
	@echo "\033[32m--- Cleaning workspace ---\033[0m"
	@$(MAKE) -C $(PERLIN_DIR) clean
	@$(MAKE) -C $(SPLAT_DIR) clean
	@rm -f $(TERRAIN_DATA_FILE) terrain_color.ppm occlusion_flat.ppm
	@rm -f $(VALIDATE_BIN) $(DIFF_BIN)
	@rm -f $(PERLIN_BASELINE_RAW) $(PERLIN_SUBMIT_RAW) $(SPLAT_BASELINE_RAW) $(SPLAT_SUBMIT_RAW)
