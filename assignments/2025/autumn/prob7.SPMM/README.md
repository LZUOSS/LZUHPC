# 稀疏矩阵乘

## 赛题介绍
稀疏矩阵-稠密矩阵乘法（SpMM）是科学计算和工程模拟中的基础性计算内核，其计算形式为：

``` TEXT
C = A × B
``` 
其中：

A 是 M×K 的稀疏矩阵（通常非零元素比例 <5%）包含非零元素nnz个，采用CSR格式存储
B 是 K×N 的稠密矩阵
C 是 M×N 的结果矩阵

### 稀疏矩阵介绍
稀疏矩阵是指大部分元素为零的矩阵。在实际应用中，很多矩阵的非零元素占比不到5%，甚至更少。例如：
原始矩阵（6×6）：
```
[2.0  0   0   3.0  0   0  ]
[0   4.0  0   0   0   1.0]
[0   0   0   0   0   0  ]
[1.0  0   0   5.0  0   0  ]
[0   0   2.0  0   6.0  0  ]
[0   0   0   0   0   7.0]
```
这个矩阵有36个位置，但只有8个非零元素，稀疏度为78%。

为什么需要特殊存储格式？
如果按照传统的二维数组存储这个6×6矩阵，需要存储36个浮点数。但实际上只有8个有用的数据，其余28个都是0，造成了巨大的存储浪费和计算浪费。

CSR格式详解
CSR（Compressed Sparse Row）是最常用的稀疏矩阵存储格式之一。它用三个一维数组来表示稀疏矩阵：

三个核心数组
values数组：存储所有非零元素的值
col_idx数组：存储每个非零元素对应的列索引
row_ptr数组：存储每行第一个非零元素在values数组中的位置

具体示例
对于上面的6×6矩阵，CSR格式存储为：

``` 
values   = [2.0, 3.0, 4.0, 1.0, 1.0, 5.0, 2.0, 6.0, 7.0]
col_idx  = [0,   3,   1,   5,   0,   3,   2,   4,   5  ]
row_ptr  = [0,   2,   4,   4,   6,   8,   9]
```
如何理解row_ptr数组？
row_ptr数组的长度是行数+1，它告诉我们每一行的非零元素在values数组中的起始和结束位置：
第0行：从row_ptr[0]=0到row_ptr[1]=2，包含values[0]和values[1]
第1行：从row_ptr[1]=2到row_ptr[2]=4，包含values[2]和values[3]
第2行：从row_ptr[2]=4到row_ptr[3]=4，空行（无非零元素）
第3行：从row_ptr[3]=4到row_ptr[4]=6，包含values[4]和values[5]
第4行：从row_ptr[4]=6到row_ptr[5]=8，包含values[6]和values[7]
第5行：从row_ptr[5]=8到row_ptr[6]=9，包含values[8]

如何访问矩阵元素？
要访问第i行的所有非零元素：

``` c
for (int j = row_ptr[i]; j < row_ptr[i+1]; j++) {
    int column = col_idx[j];     // 列索引
    float value = values[j];     // 对应的值
    // 矩阵A[i][column] = value
}
```
## 优化任务

项目结构为
```
.
├── CMakeLists.txt
├── include
│   ├── csr_matrix.h
│   ├── gemm.h
│   ├── matrix_utils.h
│   ├── spmm_cuda_opt.h
│   ├── spmm_cuda_ref.h
│   ├── spmm_opt.h
│   ├── spmm_ref.h
│   ├── test_case_cuda.h
│   └── test_case.h
├── main.cpp
├── README.md
└── src
    ├── csr_matrix.cpp
    ├── spmm_cuda_opt.cu
    ├── spmm_cuda_ref.cu
    ├── spmm_opt.cpp
    ├── spmm_ref.cpp
    ├── test_case.cpp
    └── test_case_cuda.cu
```
你所需要做的是修改且只能修改 src/spmm_cuda_ref.cu中的spmm实现
其中ref 是参考实现

 如何运行
 ``` bash
mkridr build
bash build_and_run.sh
 ```
推荐在测试平台上进行测试，测试平台的gpu为a100-40g,有着与最终评测所使用的gpu(a100-80g)相似
注意在测试平台上不要直接运行（避免干扰到他人测试），使用slurm提交作业
```bash
bash build.sh
mkdir out
mkdir err
sbatch sub.slurm 
```


### 计分规则
样例输出
``` 
 bash run_test.sh 
=== SpMM Performance Test ===
Matrix dimensions: 4096 x 4096 (sparse) * 4096 x 4096 (dense)   Test iterations: 20  Sparsity ratio: 0.9
CUDA SpMM COST TIME: 3.79597 ms CUDA SpMM GFLOPS: 3620.65
correct √ max diff: 0
CUSPARSE COST TIME: 6.1993 ms  CUSPARSE GFLOPS: 2217
=== SpMM Performance Test ===
Matrix dimensions: 8192 x 8192 (sparse) * 8192 x 4096 (dense)   Test iterations: 20  Sparsity ratio: 0.9
CUDA SpMM COST TIME: 15.7747 ms CUDA SpMM GFLOPS: 3485.04
correct √ max diff: 0
CUSPARSE COST TIME: 25.0552 ms  CUSPARSE GFLOPS: 2194.17
=== SpMM Performance Test ===
Matrix dimensions: 16384 x 64 (sparse) * 64 x 1024 (dense)   Test iterations: 20  Sparsity ratio: 0.9
CUDA SpMM COST TIME: 0.105472 ms CUDA SpMM GFLOPS: 2036.06
correct √ max diff: 0
CUSPARSE COST TIME: 0.128 ms  CUSPARSE GFLOPS: 1677.71
=== SpMM Performance Test ===
Matrix dimensions: 16384 x 128 (sparse) * 128 x 8192 (dense)   Test iterations: 20  Sparsity ratio: 0.95
CUDA SpMM COST TIME: 0.786432 ms CUDA SpMM GFLOPS: 2184.52
correct √ max diff: 0
CUSPARSE COST TIME: 0.958464 ms  CUSPARSE GFLOPS: 1792.43
=== SpMM Performance Test ===
Matrix dimensions: 16384 x 512 (sparse) * 512 x 4096 (dense)   Test iterations: 20  Sparsity ratio: 0.95
CUDA SpMM COST TIME: 1.39469 ms CUDA SpMM GFLOPS: 2463.61
correct √ max diff: 0
CUSPARSE COST TIME: 1.52576 ms  CUSPARSE GFLOPS: 2251.97
=== SpMM Performance Test ===
Matrix dimensions: 16384 x 2048 (sparse) * 2048 x 2048 (dense)   Test iterations: 20  Sparsity ratio: 0.95
CUDA SpMM COST TIME: 3.05357 ms CUDA SpMM GFLOPS: 2250.46
correct √ max diff: 0
CUSPARSE COST TIME: 3.58605 ms  CUSPARSE GFLOPS: 1916.3
=== SpMM Performance Test ===
Matrix dimensions: 16384 x 4096 (sparse) * 4096 x 128 (dense)   Test iterations: 20  Sparsity ratio: 0.95
CUDA SpMM COST TIME: 0.350208 ms CUDA SpMM GFLOPS: 2452.81
correct √ max diff: 0
CUSPARSE COST TIME: 0.50176 ms  CUSPARSE GFLOPS: 1711.96
=== SpMM Performance Test ===
Matrix dimensions: 16384 x 8192 (sparse) * 8192 x 64 (dense)   Test iterations: 20  Sparsity ratio: 0.95
CUDA SpMM COST TIME: 0.401408 ms CUDA SpMM GFLOPS: 2139.95
correct √ max diff: 0
CUSPARSE COST TIME: 0.774144 ms  CUSPARSE GFLOPS: 1109.6

```
其中8个样例得Gflops总和为你得成绩


## 注意事项
1 禁止使用32位以下的精度
2 禁止修改计时与评测代码


