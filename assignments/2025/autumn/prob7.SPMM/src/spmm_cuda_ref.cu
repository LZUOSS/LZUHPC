#include <cuda_runtime.h>
#include <iostream>
#include <spmm_cuda_ref.h>
__global__ void spmm_kernel_ref_device(int *ptr, int *idx, float *val, float *vin, float *vout, int m, int n)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid >= m) return;
    int begin = ptr[tid], end = ptr[tid + 1];
    for (int j = 0; j < n; ++j)
    {
        float result = 0.0f;
        for (int i = begin; i < end; ++i)
        {
            result += vin[idx[i] * n + j] * val[i];
        }
        vout[tid * n + j] = result;
    }
}



void spmm_cuda_ref(int *d_ptr, int *d_idx, float *d_val, float *d_vin, float *d_vout, int m, int n,int k)
{
    int BLOCK_SIZE = 128;
    dim3 grid((m + BLOCK_SIZE - 1) / BLOCK_SIZE);
    dim3 block(BLOCK_SIZE);
    spmm_kernel_ref_device<<<grid, block>>>(d_ptr, d_idx, d_val, d_vin, d_vout, m, n);
}
