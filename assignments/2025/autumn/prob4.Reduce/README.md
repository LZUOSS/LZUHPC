## 1. 背景介绍

在高性能计算（HPC）和数据处理领域，**Reduce** 操作是最常见的并行计算模式之一。它指的是将一个数组（或向量）中的所有元素通过某种二元操作（如求和、最大值、乘积等）归约为一个单一值。例如，数组求和就是一种典型的 Reduce 操作。

在 GPU 编程中，利用 CUDA 并行化 Reduce 操作可以极大提升处理大规模数据的效率。掌握并优化 Reduce 算法，是 CUDA 编程的基础能力之一。


CUDA 入门简介
CUDA（Compute Unified Device Architecture）是 NVIDIA 推出的并行计算平台和编程模型。它允许开发者使用 C/C++ 等语言，直接在 NVIDIA GPU 上编写高性能并行程序。
1. 为什么用 CUDA？
高并行性：GPU 拥有成百上千个计算核心，适合处理大规模数据并行任务。
易用性：CUDA 提供了类似 C 的编程接口，易于学习和使用。
广泛应用：科学计算、机器学习、图像处理等领域都在用 CUDA 加速。
2. CUDA 编程模型简述
主机（Host）：指的是 CPU 和系统内存。
设备（Device）：指的是 GPU 及其显存。
编写 CUDA 程序时，通常需要：

在主机端分配和初始化数据。
在设备上启动并行计算（称为“核函数”或 kernel）。
将结果从设备传回主机。
3. 基本代码结构
``` c++
// CUDA 核函数（在 GPU 上运行）
__global__ void add(float *a, float *b, float *c, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n)
        c[idx] = a[idx] + b[idx];
}

// 主机代码（在 CPU 上运行）
int main() {
    // 分配、初始化数据
    // 数据传输到 GPU
    // 启动 kernel
    // 结果传回 CPU
}
```
4. 线程、块和网格
线程（thread）：GPU 的最小计算单位。
线程块（block）：一组线程，能共享高速的共享内存。
网格（grid）：多个线程块组成的整体。
通过合理划分线程和块，可以让 GPU 高效并行处理大量数据。

## 2. 赛题任务

本赛题提供了一个 CUDA 项目框架，包含基础的 Reduce 实现接口和测试用例。你的任务是：

- 在 `src/reduce.cu` 文件中，实现高效的并行 Reduce 操作（求和）。
- 支持输入为 float 类型的一维数组。
- 保证结果的正确性，并尽量提升性能。

你可以参考 `include/reduce.h` 中的接口声明，以及使用 `main.cpp` 和 `test_case.cu` 进行功能和性能测试。


## 3. 项目结构
```
CUDA-REDUCE/
├── CMakeLists.txt
├── include
│   ├── reduce.h
│   ├── test_case.h
│   └── utils.h
├── main.cpp
└── src
    ├── reduce.cu
    └── test_case.cu
```

## 4. 优化提示

- **线程并行**：利用 CUDA 的线程块和网格结构，将归约操作分配到多个线程并行处理。
- **共享内存**：合理使用共享内存，减少全局内存访问延迟。可以搜索并思考下怎么使用共享内存，共享内存是什么，为什么要使用共享内存，这是cuda程序优化的关键
- **避免分支和冲突**：优化线程同步和避免bank冲突，提高效率。

## 5. 测试与提交

- 使用 `main.cpp` 和 `test_case.cu` 进行功能和性能测试。
- 提交时需包含你的核心实现代码（`reduce.cu`）。

编译
``` bash
bash build.sh
```
运行
``` bash
bash run.sh
```
四个测试案例，每个测试指标达到对应指标即可获得相应分数

推荐在测试平台上进行测试，测试平台的gpu为a100-40g,有着与最终评测所使用的gpu(a100-80g)相似
注意在测试平台上不要直接运行，使用slurm提交作业
```bash
bash build.sh
mkdir out
mkdir err
sbatch sub.slurm 
```


样例输出
```
len: 1024000 , time: 836.221 us
Result correct! diff is 0.0078125
len: 102400000 , time: 16149.7 us
Result correct! diff is 2
len: 8192000 , time: 1917.53 us
Result correct! diff is 0.0625
len: 20480000 , time: 3819.01 us
Result correct! diff is 0.625
```
---

**提示**：本赛题为 CUDA 入门级，重点考察你对 GPU 并行编程模型的理解和基础优化能力。欢迎大胆尝试不同的优化方法！

祝你编程愉快，取得优异成绩！