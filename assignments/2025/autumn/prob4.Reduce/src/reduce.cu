#include "reduce.h"
#include <iostream>
#include <cuda_runtime.h>
#include <omp.h>




// GPU Reduce包装函数
float gpuReduce(const float* d_data, const int size) {
    //在这里实现你的代码，注意数据已经被拷贝到GPU上
    return 0.0f; // 返回归约结果
}
