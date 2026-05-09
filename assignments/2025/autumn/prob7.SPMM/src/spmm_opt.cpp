#include <cstdlib>
#include <iostream>
#include <cstring>
#include <omp.h>
#include "spmm_opt.h"


//分块读取B到三缓然后处理(不适合只有2级缓存的cpu)
void spmm_cpu_opt(int *ptr, int *idx, float *val, float *vin, float *vout, int num_v, int INFEATURE,int k)
{
    const int K_BLOCK_SIZE = 512;
    const int N_BLOCK_SIZE = 512;
    float* buf_B = (float*) aligned_alloc(64, sizeof(float) * K_BLOCK_SIZE * N_BLOCK_SIZE);
    #pragma omp parallel
    {
        for (int n_start = 0; n_start < INFEATURE; n_start += N_BLOCK_SIZE) {
            for (int k_start = 0; k_start < k; k_start += K_BLOCK_SIZE) {
                int n_end = n_start + N_BLOCK_SIZE;
                if (n_end > INFEATURE) n_end = INFEATURE;
                int block_cols = n_end - n_start;
                int k_end = k_start + K_BLOCK_SIZE;
                if (k_end > k) k_end = k;
                int block_rows = k_end - k_start;
                // 打包B矩阵的块到buf_B中（行优先存储）
                #pragma omp for schedule(static)
                for (int r = k_start; r < k_end; r++) {
                    for (int c = n_start; c < n_end; c++) {
                        buf_B[(r - k_start) * block_cols + (c - n_start)] = vin[r * INFEATURE + c];
                    }
                }
                // 处理所有行m
                #pragma omp for schedule(static)
                for (int m = 0; m < num_v; m++) {
                    int begin = ptr[m];
                    int end = ptr[m+1];
                    // 遍历稀疏矩阵A的第m行的非零元素
                    for (int i = begin; i < end; i++) {
                        int col = idx[i];
                        if (col < k_start){
                            continue;
                        }
                        if(col >= k_end) {
                            break;
                        }
                        // 只处理列在当前k块内的非零元素
                        float val_i = val[i];
                        int col_index = (col - k_start)*block_cols;
                        // 对当前n块中的每一列j进行计算
                        for (int c = 0; c < block_cols; c++) {
                            int j = n_start + c;
                            vout[m * INFEATURE + j] += val_i * buf_B[col_index + c];
                        }
                    }
                }
            }
        }
    }
    free(buf_B);
}
