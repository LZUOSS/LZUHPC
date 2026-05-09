#!/bin/bash
#SBATCH --job-name=gemm_test             # 作业名称
#SBATCH --output=./out/%j.out        # 标准输出和错误文件
#SBATCH --error=./err/%j.err
#SBATCH --nodes=1                        # 节点数
#SBATCH --ntasks=1                       # 任务数
#SBATCH --cpus-per-task=56              # 每个任务分配的CPU核心数
#SBATCH --time=01:00:00                  # 运行时间上限
#SBATCH --partition=q_ys_test1           # 分区名称（根据实际集群填写）

export OMP_PROC_BIND=close
export OMP_NUM_THREADS=28


# 执行
taskset -c 0-27 ./build/gemm -m 2048 -n 2048 -k 2048 -t 10
taskset -c 0-27 ./build/gemm -m 4096 -n 4096 -k 4096 -t 10
taskset -c 0-27 ./build/gemm -m 1024 -n 1024 -k 1024 -t 10
taskset -c 0-27 ./build/gemm -m 8192 -n 2048 -k 2048 -t 10
taskset -c 0-27 ./build/gemm -m 4096 -n 2048 -k 4096 -t 10
taskset -c 0-27 ./build/gemm -m 8192 -n 2048 -k 4096 -t 10
taskset -c 0-27 ./build/gemm -m 1024 -n 8192 -k 4096 -t 10
taskset -c 0-27 ./build/gemm -m 8192 -n 2048 -k 4096 -t 1