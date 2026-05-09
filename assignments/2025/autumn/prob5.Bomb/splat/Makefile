MAKEFLAGS += --no-print-directory # 为了美观

CXX := g++

CXXFLAGS       := -O3 -march=native -std=c++17 -I../framework -Wall -fno-tree-vectorize -fno-tree-slp-vectorize
CXXFLAGS_DEBUG := -O3 -march=native -g -std=c++17 -I../framework -Wall -fno-tree-vectorize -fno-tree-slp-vectorize

TARGET := run_splat
TARGET_TEST := run_splat_debug
BASELINE_TARGET := run_splat_baseline
SUBMISSION_SRC := submit_splat.cpp
BASELINE_SRC := baseline_splat.cpp

TASKSET_BIN := $(shell command -v taskset 2>/dev/null)
PIN_TEST_CMD := $(if $(TASKSET_BIN),taskset -c 0 ./$(TARGET_TEST),./$(TARGET_TEST))

HARNESS_SRC := harness_splat.cpp

SIZE := M
SEED := 42
HEIGHTMAP ?=

.PHONY: all baseline run test perf clean visualize toplev raw_output raw_output_baseline benchmark

all: $(TARGET) $(BASELINE_TARGET)

baseline: $(BASELINE_TARGET)

$(TARGET): $(HARNESS_SRC) $(SUBMISSION_SRC) ../framework/timer.h ../framework/hash.h ../framework/prng.h ../framework/types.h
	$(CXX) $(CXXFLAGS) $(HARNESS_SRC) -o $(TARGET)

$(TARGET_TEST): $(HARNESS_SRC) $(SUBMISSION_SRC) ../framework/timer.h ../framework/hash.h ../framework/prng.h ../framework/types.h
	$(CXX) $(CXXFLAGS_DEBUG) $(HARNESS_SRC) -o $(TARGET_TEST)

$(BASELINE_TARGET): $(HARNESS_SRC) $(BASELINE_SRC) ../framework/timer.h ../framework/hash.h ../framework/prng.h ../framework/types.h
	$(CXX) $(CXXFLAGS) -DSUBMISSION_FILE=\"$(BASELINE_SRC)\" $(HARNESS_SRC) -o $(BASELINE_TARGET)

SIM_HEIGHTMAP_ARG := $(if $(strip $(HEIGHTMAP)),--input-heightmap "$(HEIGHTMAP)",)
OCCLUSION_SUBMIT_FILE ?= occlusion_submit.raw
OCCLUSION_BASELINE_FILE ?= occlusion_baseline.raw

run: $(TARGET)
	@echo "\033[32m--- Running with size $(SIZE) and seed $(SEED) ---\033[0m"
	@python3 gen_splat.py --size $(SIZE) --seed $(SEED) | ./$(TARGET) $(SIM_HEIGHTMAP_ARG)

visualize: $(TARGET_TEST)
	@echo "\033[32m--- Generating visualization for size S (outputs: damage_S.ppm, occlusion_S.ppm) ---\033[0m"
	@python3 gen_splat.py --size S --seed 42 | ./$(TARGET_TEST) --visualize

raw_output: $(TARGET_TEST)
	@echo "\033[32m--- Generating submission occlusion map raw (size $(SIZE), seed $(SEED)) ---\033[0m"
	@python3 gen_splat.py --size $(SIZE) --seed $(SEED) | ./$(TARGET_TEST) $(SIM_HEIGHTMAP_ARG) --output-occlusion-raw $(OCCLUSION_SUBMIT_FILE)

raw_output_baseline: $(BASELINE_TARGET)
	@echo "\033[32m--- Generating baseline occlusion map raw (size $(SIZE), seed $(SEED)) ---\033[0m"
	@python3 gen_splat.py --size $(SIZE) --seed $(SEED) | ./$(BASELINE_TARGET) $(SIM_HEIGHTMAP_ARG) --output-occlusion-raw $(OCCLUSION_BASELINE_FILE)

test: $(TARGET_TEST)
	@echo "\033[32m--- Running comprehensive tests (S, M, L) with seed 42 ---\033[0m"
ifeq ($(TASKSET_BIN),)
	@echo "\033[33m[warning] 'taskset' not found; running tests without CPU pinning.\033[0m"
endif
	@echo "\033[34m[Size S]\033[0m"
	@python3 gen_splat.py --size S --seed 42 | $(PIN_TEST_CMD) $(SIM_HEIGHTMAP_ARG)
	@echo "\n\033[34m[Size M]\033[0m"
	@python3 gen_splat.py --size M --seed 42 | $(PIN_TEST_CMD) $(SIM_HEIGHTMAP_ARG)
	@echo "\n\033[34m[Size L]\033[0m"
	@python3 gen_splat.py --size L --seed 42 | $(PIN_TEST_CMD) $(SIM_HEIGHTMAP_ARG)

perf: $(TARGET_TEST)
	@echo "\033[32m--- Running with perf stat (size $(SIZE), seed $(SEED)) ---\033[0m"
	@perf stat -e cycles,instructions,branches,branch-misses,cache-references,cache-misses,L1-dcache-load-misses,LLC-load-misses,dTLB-load-misses,stalled-cycles-frontend,stalled-cycles-backend \
	 bash -c "python3 gen_splat.py --size $(SIZE) --seed $(SEED) | ./$(TARGET_TEST) $(SIM_HEIGHTMAP_ARG)"

RUN_WRAPPER := ./run.sh

$(RUN_WRAPPER):
	@printf '%s\n' '#!/usr/bin/env bash' \
'set -euo pipefail' \
'python3 gen_splat.py --size $(SIZE) --seed $(SEED) | exec ./$(TARGET_TEST) $(SIM_HEIGHTMAP_ARG)' > $(RUN_WRAPPER)
	@chmod +x $(RUN_WRAPPER)

#TODO: toplev位置需要修改, 并且run-sample需要修改
toplev: $(TARGET_TEST) $(RUN_WRAPPER)
	@echo "\033[32m--- Running with toplev stat (size $(SIZE), seed $(SEED)) ---\033[0m"
	@python3 ../../pmu-tools/toplev.py -l2 --run-sample -- "$(RUN_WRAPPER)"

BENCH_SIZE ?= $(SIZE)
BENCH_SEED ?= $(SEED)
BENCH_HEIGHTMAP ?= $(HEIGHTMAP)

benchmark: $(BASELINE_TARGET) $(TARGET)
	@echo "\033[32m--- Benchmarking Splat submission vs. baseline (size $(BENCH_SIZE), seed $(BENCH_SEED)) ---\033[0m"
	@python3 ../utils/benchmark_splat.py $(BENCH_SIZE) $(BENCH_SEED) $(BASELINE_TARGET) $(TARGET) $(BENCH_HEIGHTMAP)

clean:
	@echo "\033[32m--- Cleaning up in Splat  ---\033[0m"
	@rm -f $(TARGET) $(TARGET_TEST) $(BASELINE_TARGET) *.o *.ppm *.raw perf.data* run.sh
