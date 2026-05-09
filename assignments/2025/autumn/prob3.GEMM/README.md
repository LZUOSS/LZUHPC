## 1. 矩阵乘法介绍

通用矩阵乘法（GEMM）是线性代数中的核心操作，定义为对于两个矩阵 A（大小为 M×K）和 B（大小为 K×N），其乘积 C（大小为 M×N）的每个元素计算为：

 $$C_{i,j} = \sum_{k=1}^{K} A_{i,k} \times B_{k,j} $$


在计算科学中，GEMM 是许多应用的基础，如机器学习、科学模拟和图像处理。由于其计算密集型特性（时间复杂度为 O(M×N×K)），GEMM 常被用作高性能计算（HPC）的基准测试，优化 GEMM 能显著提升整体系统性能。

## 2. 背景

在现代 HPC 系统中，GEMM 优化是衡量处理器和内存架构效率的关键。尽管简单的三重循环实现容易编写，但往往无法充分利用硬件资源（如缓存层次、向量单元和多核并行性）。因此，优化 GEMM 需要深入理解计算机体系结构，应用循环变换、数据局部性、向量化等技术。

本赛题提供了一个基础的 GEMM 实现框架，旨在挑战参赛者通过代码优化，最大化 GEMM 的性能。优化目标包括提高计算吞吐量、降低内存延迟，并保持数值准确性。

## 3. 任务描述

参赛者需基于提供的代码框架（见文件结构如下），优化 `gemm_opt.cpp` 中的矩阵乘法实现。代码框架包括：
- 基础 GEMM 实现（`gemm.h` 和 `gemm_opt.h`）
- 测试用例和矩阵工具（`test_case.h` 和 `matrix_utils.h`）
- 构建和运行脚本（`build_and_run.sh` 和 `run_test.sh`）

**核心任务**：修改且只能 `src/gemm_opt.cpp` 中的函数，使其在保证正确性的前提下，比原始实现（`src/gemm.cpp`，如果存在）或参考实现性能更高。优化应针对典型矩阵大小（如 1024×1024）进行，并支持通用浮点矩阵。

评估将基于：
- 性能提升（通过FLOPS 衡量，所有case的glfops和为你的最终成绩）
- 代码正确性（通过所有提供的测试用例正确性校验）


## 4. 优化提示
以下是一些可能的优化方向，供参赛者参考（但鼓励超越这些提示）：
### 1. CPU Cache 基础概念

什么是 CPU Cache？
CPU Cache 是 CPU 内部的小容量高速内存，用于存储最近访问的数据。现代 CPU 通常有三级缓存：

L1 Cache：最小最快，通常 32-64KB，每个核心独享
L2 Cache：中等大小，通常 256KB-1MB，每个核心独享
L3 Cache：最大最慢，通常 8-32MB，所有核心共享
为什么 Cache 重要？
速度差异：从 L1 Cache 读取数据比从主内存快 100 倍以上
带宽限制：内存带宽有限，Cache 命中率高能极大提升性能
### 2. Cache Miss 的类型
三种 Cache Miss：

强制失效 (Compulsory Miss)：第一次访问数据，无法避免

容量失效 (Capacity Miss)：Cache 容量不足，数据被替换出去

冲突失效 (Conflict Miss)：多个数据映射到同一 Cache 位置
### 3. 局部性原理

时间局部性 (Temporal Locality)
```CPP
// 坏例子：反复从内存读取
for(int i=0; i<N; i++) {
    sum += data[i];  // 每次都要从内存加载
}

// 好例子：重用寄存器中的数据
float temp = data[i];
for(int j=0; j<M; j++) {
    result += temp * other_data[j];  // temp 在寄存器中
}
```
空间局部性 (Spatial Locality)
```CPP
// 坏例子：跳跃访问
for(int i=0; i<N; i+=stride) {
    process(data[i]);  // 每次访问都跨越很大距离
}

// 好例子：连续访问
for(int i=0; i<N; i++) {
    process(data[i]);  // 访问连续内存，预取器能预测
}
```
借此可以思考下，按照内积的方法进行矩阵乘时，对矩阵B的访问是什么样的，怎么改善

### 4. 分块 (Blocking/ Tiling)
为什么需要分块？

当矩阵很大时，无法全部放入 Cache，导致反复从内存加载数据。
分块原理：
```c++
// 原始三重循环 - Cache 不友好
for(int i=0; i<M; i++) {
    for(int j=0; j<N; j++) {
        for(int k=0; k<K; k++) {
            C[i][j] += A[i][k] * B[k][j];
        }
    }
}

// 分块版本 - Cache 友好
const int block_size = 64;  // 根据 L1 Cache 大小选择
for(int ii=0; ii<M; ii+=block_size) {
    for(int jj=0; jj<N; jj+=block_size) {
        for(int kk=0; kk<K; kk+=block_size) {
            // 处理小块
            
        }
    }
}
```
### AVX向量化
AVX 是一种让 CPU 一次能“批量”处理多个数据的技术（SIMD）。
普通代码一次只能算一个数，AVX 可以一次算 8 个 float（单精度小数）。
为什么用 AVX？
更快：同样的计算，AVX 可以一次做 8 倍的工作。
常用于：矩阵乘法、图像处理、科学计算等。
简单例子：矩阵乘法
普通写法（每次算一个）：
```CPP
for (int i = 0; i < N; i++)
    C[i] = A[i] + B[i];
//AVX 写法（每次算 8 个）：
#include <immintrin.h>
for (int i = 0; i < N; i += 8) {
    __m256 a = _mm256_loadu_ps(&A[i]); // 读入A的8个数
    __m256 b = _mm256_loadu_ps(&B[i]); // 读入B的8个数
    __m256 c = _mm256_add_ps(a, b);    // 8个数同时相加
    _mm256_storeu_ps(&C[i], c);        // 写回结果
}
```
AVX 就像“多筷子夹菜”，一次夹 8 个，速度快很多。
只要用对方法，矩阵乘法等计算能大幅提速。


**注意**：优化时应避免过度复杂化，优先保证可维护性和可移植性。提供的测试框架（`run_test.sh`）可用于验证正确性和性能对比。

## 5. 评测环境说明
评测所采用的cpu为8358
编译参数如与当前CMakeLists.txt中保持一致
评测时cpu核心限制为28

可以在测试平台测试后再提交到hpcgame平台
测试平台编译后使用slurm提交
注意测试平台需要使用spack预先加载对应cmake以及make环境
``` bash
spack load cmake@3.23.1%gcc@10.2.0
mkdir build
cd build
cmake ..
make 

cd ..
mkdir out
mkdir err
sbatch sub.slurm
```


## 6. 注意事项
1 禁止使用32位以下的精度
2 禁止修改计时与评测代码

