#!/bin/bash
#SBATCH --job-name=conway             # 作业名称
#SBATCH --output=./out/%j.out        # 标准输出和错误文件
#SBATCH --error=./err/%j.err
#SBATCH --nodes=1                        # 节点数
#SBATCH --ntasks=1                       # 任务数
#SBATCH --cpus-per-task=56              # 每个任务分配的CPU核心数
#SBATCH --time=01:00:00                  # 运行时间上限
#SBATCH --partition=q_ys_test1           # 分区名称（根据实际集群填写）

export OMP_NUM_THREADS=16


# 执行
taskset -c 0-15 python3 src/main.py -F ./dat/128x128islands.rle -I 200
taskset -c 0-15 python3 src/main.py -F ./dat/128x128symmetric.rle -I 200
taskset -c 0-15 python3 src/main.py -F ./dat/128x128uniform.rle -I 200
taskset -c 0-15 python3 src/main.py -F ./dat/256x256islands.rle -I 100
taskset -c 0-15 python3 src/main.py -F ./dat/256x256symmetric.rle -I 100
taskset -c 0-15 python3 src/main.py -F ./dat/256x256uniform.rle -I 100
taskset -c 0-15 python3 src/main.py -F ./dat/384x384uniform.rle -I 40
taskset -c 0-15 python3 src/main.py -F ./dat/512x512uniform.rle -I 40
taskset -c 0-15 python3 src/main.py -F ./dat/768x768uniform.rle -I 30
taskset -c 0-15 python3 src/main.py -F ./dat/1024x1024uniform.rle -I 20
