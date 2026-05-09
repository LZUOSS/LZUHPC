#include <cstdlib>
#include <iostream>
#include <cstring>
#include <ctime>
#include <omp.h>
#include "spmm_ref.h"



void spmm_cpu_ref(int *ptr, int *idx, float *val, float *vin, float *vout, int num_v, int INFEATURE,int k)
{
   //遍历每一行
    #pragma omp parallel for schedule(static)
    for (int m = 0; m < num_v; ++m) {
        int begin = ptr[m], end = ptr[m + 1];
        for (int i = begin; i < end; ++i) {
            int col = idx[i];
            float matrix_val = val[i];
            for (int j = 0; j < INFEATURE; ++j) {
                vout[m * INFEATURE + j] += vin[col * INFEATURE + j] * matrix_val;
            }
        }
    }
}

