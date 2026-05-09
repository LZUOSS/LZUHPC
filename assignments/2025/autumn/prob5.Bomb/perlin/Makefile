MAKEFLAGS += --no-print-directory # 为了美观

CXX := g++
CXXFLAGS       := -O2 -std=c++17 -I../framework -Wall -fno-tree-vectorize -fno-tree-slp-vectorize
CXXFLAGS_DEBUG := -O2 -std=c++17 -g -I../framework -Wall -fno-tree-vectorize -fno-tree-slp-vectorize

TARGET := run_perlin
TARGET_TEST := run_perlin_debug
BASELINE_TARGET := run_perlin_baseline
SUBMISSION_SRC := submit_perlin.cpp
BASELINE_SRC := baseline_perlin.cpp

TASKSET_BIN := $(shell command -v taskset 2>/dev/null)
PIN_TEST_CMD := $(if $(TASKSET_BIN),taskset -c 0 ./$(TARGET_TEST),./$(TARGET_TEST))

HARNESS_SRC := harness_perlin.cpp

#TODO: 重复修改SEED获得准确性
SIZE := L
SEED := 42

.PHONY: all baseline run test perf clean visualize toplev raw raw_output raw_output_baseline benchmark

all: $(TARGET) $(BASELINE_TARGET)

baseline: $(BASELINE_TARGET)

$(TARGET): $(HARNESS_SRC) $(SUBMISSION_SRC) ../framework/timer.h ../framework/hash.h ../framework/prng.h ../framework/types.h
	$(CXX) $(CXXFLAGS) $(HARNESS_SRC) -o $(TARGET)

$(TARGET_TEST): $(HARNESS_SRC) $(SUBMISSION_SRC) ../framework/timer.h ../framework/hash.h ../framework/prng.h ../framework/types.h
	$(CXX) $(CXXFLAGS_DEBUG) $(HARNESS_SRC) -o $(TARGET_TEST)

$(BASELINE_TARGET): $(HARNESS_SRC) $(BASELINE_SRC) ../framework/timer.h ../framework/hash.h ../framework/prng.h ../framework/types.h
	$(CXX) $(CXXFLAGS) -DSUBMISSION_FILE=\"$(BASELINE_SRC)\" $(HARNESS_SRC) -o $(BASELINE_TARGET)

run: $(TARGET)
	@echo "\033[32m--- Running with size $(SIZE) and seed $(SEED) ---\033[0m"
	@python3 gen_perlin.py --size $(SIZE) --seed $(SEED) | ./$(TARGET)

visualize: $(TARGET_TEST)
	@echo "\033[32m--- Generating visualization for size S (output: terrain_S.ppm) ---\033[0m"
	@python3 gen_perlin.py --size S --seed 42 | ./$(TARGET_TEST) --visualize terrain_S.ppm

RAW_OUTPUT_SIZE ?= L
RAW_OUTPUT_FILE ?= terrain.raw
raw_output: $(TARGET_TEST)
	@echo "\033[32m--- Generating raw binary terrain data (size $(RAW_OUTPUT_SIZE), output: $(RAW_OUTPUT_FILE)) ---\033[0m"
	@python3 gen_perlin.py --size $(RAW_OUTPUT_SIZE) --seed 42 | ./$(TARGET_TEST) --output-raw $(RAW_OUTPUT_FILE)

raw: raw_output

RAW_BASELINE_FILE ?= terrain_baseline.raw
raw_output_baseline: $(BASELINE_TARGET)
	@echo "\033[32m--- Generating baseline raw terrain (size $(RAW_OUTPUT_SIZE), output: $(RAW_BASELINE_FILE)) ---\033[0m"
	@python3 gen_perlin.py --size $(RAW_OUTPUT_SIZE) --seed 42 | ./$(BASELINE_TARGET) --output-raw $(RAW_BASELINE_FILE)

#TODO: 添加重复测试
#OJ: OJ上taskset需要修改
test: $(TARGET_TEST)
	@echo "\n\033[32m--- Running comprehensive tests (S, M, L) with seed 42 ---\033[0m"
ifeq ($(TASKSET_BIN),)
	@echo "\033[33m[warning] 'taskset' not found; running tests without CPU pinning.\033[0m"
endif
	@echo "\n\033[34m[Size S]\033[0m"
	@python3 gen_perlin.py --size S --seed 42 | $(PIN_TEST_CMD)
	@echo "\n\033[34m[Size M]\033[0m"
	@python3 gen_perlin.py --size M --seed 42 | $(PIN_TEST_CMD)
	@echo "\n\033[34m[Size L]\033[0m"
	@python3 gen_perlin.py --size L --seed 42 | $(PIN_TEST_CMD)

perf: $(TARGET_TEST)
	@echo "\033[32m--- Running with perf stat (size $(SIZE), seed $(SEED)) ---\033[0m"
	@perf stat -e cycles,instructions,branches,branch-misses,cache-references,cache-misses,L1-dcache-load-misses,LLC-load-misses,dTLB-load-misses,stalled-cycles-frontend,stalled-cycles-backend \
	 bash -c "python3 gen_perlin.py --size $(SIZE) --seed $(SEED) | ./$(TARGET_TEST)"
	
RUN_WRAPPER := ./run.sh

$(RUN_WRAPPER):
	@printf '%s\n' '#!/usr/bin/env bash' \
'set -euo pipefail' \
'python3 gen_perlin.py --size $(SIZE) --seed $(SEED) | exec ./$(TARGET_TEST)' > $(RUN_WRAPPER)
	@chmod +x $(RUN_WRAPPER)

toplev: $(TARGET_TEST) $(RUN_WRAPPER)
	@echo "\033[32m--- Running with toplev stat (size $(SIZE), seed $(SEED)) ---\033[0m"
	@python3 ../../pmu-tools/toplev.py -l2 --run-sample -- "$(RUN_WRAPPER)"

BENCH_SIZE ?= $(SIZE)
BENCH_SEED ?= $(SEED)
benchmark: $(BASELINE_TARGET) $(TARGET)
	@echo "\033[32m--- Benchmarking Perlin submission vs. baseline (size $(BENCH_SIZE), seed $(BENCH_SEED)) ---\033[0m"
	@python3 ../utils/benchmark_perlin.py $(BENCH_SIZE) $(BENCH_SEED) $(BASELINE_TARGET) $(TARGET)

clean:
	@echo "\033[32m--- Cleaning up in Perlin ---\033[0m"
	@rm -f $(TARGET) $(TARGET_TEST) $(BASELINE_TARGET) *.o perf.data* run.sh *.ppm *.raw
