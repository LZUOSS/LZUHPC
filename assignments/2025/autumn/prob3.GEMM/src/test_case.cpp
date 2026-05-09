#include "test_case.h"
#include <iostream>
#include <omp.h>
#include "gemm.h"
#include "matrix_utils.h"
#include "gemm_opt.h"
#include <chrono>
#include <algorithm>

void flush_cache_all_cores(size_t flush_size_per_thread = 800 * 1024) {
    //清理cache缓存
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        std::vector<char> buffer(flush_size_per_thread, tid);
        volatile char sink = 0;
        for (size_t i = 0; i < buffer.size(); i += 64) {
            sink += buffer[i];
        }
        if (sink == 123) std::cout << "";
    }
}

void test_gemm_cpu(const int m, const int n, const int k,const int test_time){
    float* A = (float*)aligned_alloc(64, m * k * sizeof(float));
    float* B = (float*)aligned_alloc(64, k * n * sizeof(float));
    float* C_ref = (float*)aligned_alloc(64, m * n * sizeof(float));
    float* C_check = (float*)aligned_alloc(64, m * n * sizeof(float));
    memset(C_ref, 0, m * n * sizeof(float));
    memset(C_check, 0, m * n * sizeof(float));
    // Gen_Matrix_sparsity(A,m,k,sparsity);
    Gen_Matrix(A,m,k);
    Gen_Matrix(B,k,n);
    gemm(A, B, C_ref, m, n, k);
    double min_time=1e6;
    for(int i=0;i<test_time;i++){
        memset(C_check,0,m*n*sizeof(float));
        flush_cache_all_cores();
        auto iter_start = std::chrono::high_resolution_clock::now();
        gemm_opt(A, B, C_check, m, n, k);
        auto iter_end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(iter_end - iter_start);
        min_time = std::min(duration.count() / 1e6,min_time); 
    }
    std::cout << "CPU Gemm COST TIME: " << min_time << " ms" ;
    double gflops=(2.0*m*n*k*1e-9)/(min_time/1000);
    std::cout << "   CPU Gemm GFLOPS: " << gflops << std::endl;
    float max_diff = max_diff_twoMatrix(C_check,C_ref,m,n);
    bool is_correct=false;
    if(max_diff<1e-2) 
    {
        is_correct=true;
    }
    std::cout << (is_correct ? "correct √" : "false !!")<< " max diff: " << max_diff << "\n";
    // Clean up
    free(A);
    free(B);
    free(C_ref);
    free(C_check);
}









