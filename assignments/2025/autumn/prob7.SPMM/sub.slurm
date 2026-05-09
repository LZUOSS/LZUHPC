#!/bin/bash
#SBATCH --job-name=spmm_test           # 作业名称
#SBATCH --output=./out/%j.out      # 标准输出文件
#SBATCH --error=./err/%j.err       # 标准错误文件
#SBATCH --partition=GPU                # 分区名称，必须和 sinfo 查询一致
#SBATCH --nodes=1                      # 使用1个节点
#SBATCH --ntasks=1                     # 一个任务
#SBATCH --cpus-per-task=32             # 16个CPU核心
#SBATCH --gres=gpu:1                   # 申请1个GPU
#SBATCH --time=02:00:00                # 最长运行时间2小时
# 环境变量
export OMP_PROC_BIND=close
export OMP_NUM_THREADS=32
# 执行你的程序
./build/spmm -m 4096 -n 4096 -k 4096 -s 0.9 -t 20
./build/spmm -m 8192 -n 4096 -k 8192 -s 0.9 -t 20
./build/spmm -m 16384 -n 2048 -k 64 -s 0.9 -t 20
./build/spmm -m 16384 -n 8192 -k 128 -s 0.95 -t 20
./build/spmm -m 16384 -n 4096 -k 512 -s 0.95 -t 20
./build/spmm -m 16384 -n 2048 -k 2048 -s 0.95 -t 20
./build/spmm -m 16384 -n 128 -k 4096 -s 0.95 -t 20
./build/spmm -m 16384 -n 64 -k 8192 -s 0.95 -t 20

