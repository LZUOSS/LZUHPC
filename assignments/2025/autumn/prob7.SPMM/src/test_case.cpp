#include "test_case.h"
#include "csr_matrix.h"
#include <iostream>
#include <omp.h>
#include "gemm.h"
#include "matrix_utils.h"
#include "spmm_ref.h"
#include "spmm_opt.h"
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
void test_converter(){
    // 创建测试矩阵 4x5
    const int rows = 4;
    const int cols = 6;
    // 分配内存并初始化测试矩阵
    float* test_matrix = (float*)calloc(cols*rows, sizeof(float));
    test_matrix[0] = 10;
    test_matrix[1] = 20;
    test_matrix[1*cols+1] = 30;
    test_matrix[1*cols+3] = 40;
    test_matrix[2*cols+2] = 50.0;
    test_matrix[2*cols+3] = 60.0;
    test_matrix[2*cols+4] = 70.0;
    test_matrix[3*cols+5] = 80;
    double sparsity=calculate_sparsity(test_matrix,rows,cols);
    std::cout << "矩阵稀疏度: " << sparsity * 100 << "%\n";
    std::cout << "=== 原始矩阵 ===\n";
    print_dense_matrix(test_matrix, rows, cols);
    // 转换为CSR格式
    std::cout << "=== 转换为CSR格式 ===\n";
    CSRMatrix<float>* csr_matrix = dense_to_csr(test_matrix, rows, cols);
    print_csr_matrix(csr_matrix);
    // 从CSR格式转换回普通矩阵
    std::cout << "=== 从CSR转换回普通矩阵 ===\n";
    float* converted_matrix = csr_to_dense(csr_matrix);
    print_dense_matrix(converted_matrix, rows, cols);
    bool is_correct = matrices_equal(converted_matrix, test_matrix, rows, cols);
    std::cout << "转换结果: " << (is_correct ? "正确" : "错误") << "\n";
    // 释放内存
    free_dense_matrix(test_matrix);
    free_dense_matrix(converted_matrix);
    free_csr_matrix(csr_matrix);
}
//测试生成随机矩阵
void test_generator(){
    const int rows = 4096;
    const int cols = 4096;
    float* test_matrix = (float*)malloc(cols*rows* sizeof(float));
    double start_time=omp_get_wtime();
    Gen_Matrix_sparsity(test_matrix, rows, cols,0.9);
    CSRMatrix<float>* csr_matrix = dense_to_csr(test_matrix, rows, cols);
    std::cout<<"generate cost time:"<<omp_get_wtime()-start_time<<"\n";
    // print_dense_matrix(test_matrix, rows, cols);
    double sparsity=calculate_sparsity(test_matrix,rows,cols);
    std::cout << "矩阵稀疏度: " << sparsity * 100 << "%\n";
    std::cout << "CSR nnz: " << csr_matrix->nnz << "\n";
    // 从CSR格式转换回普通矩阵
    std::cout << "=== 从CSR转换回普通矩阵 ===\n";
    float* converted_matrix = csr_to_dense(csr_matrix);
    float max_diff = max_diff_twoMatrix(converted_matrix,test_matrix,rows,cols);
    bool is_correct=false;
    if(max_diff<1e-4) 
    {
        is_correct=true;
    }

    std::cout << "转换结果: " << (is_correct ? "正确" : "错误")<< " 最大差异: " << max_diff << "\n";
    // print_dense_matrix(test_matrix, rows, cols);
    free_dense_matrix(test_matrix);
}




void test_spmm_cpu(const int m, const int n, const int k,const int test_time,const double sparsity){
    float* A = (float*)malloc(m * k * sizeof(float));
    float* B = (float*)malloc(k * n * sizeof(float));
    float* C = (float*)calloc(m * n, sizeof(float));
    float* C2 = (float*)calloc(m * n ,sizeof(float));
    Gen_Matrix_sparsity(A,m,k,sparsity);
    Gen_Matrix(B,k,n);
    CSRMatrix<float>* csr_matrix = dense_to_csr(A, m, k);
    spmm_cpu_ref(csr_matrix->row_ptr, csr_matrix->col_indices, csr_matrix->values, B, C, m, n,k);
    double min_time=1e6;
    for(int i=0;i<test_time;i++){
        memset(C2,0,m*n*sizeof(float));
        flush_cache_all_cores();
        auto iter_start = std::chrono::high_resolution_clock::now();
        spmm_cpu_opt(csr_matrix->row_ptr, csr_matrix->col_indices, csr_matrix->values, B, C2, m, n,k);

        auto iter_end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(iter_end - iter_start);
        min_time = std::min(duration.count() / 1e6,min_time); 
    }

    std::cout << "CPU SpMM COST TIME: " << min_time << " ms" ;
    double gflops=(2.0*csr_matrix->nnz*n*1e-9)/(min_time/1000);
    std::cout << "   CPU SpMM GFLOPS: " << gflops << std::endl;
    float max_diff = max_diff_twoMatrix(C2,C,m,n);
    bool is_correct=false;
    if(max_diff<1e-3) 
    {
        is_correct=true;
    }
    std::cout << (is_correct ? "correct √" : "false !!")<< " max diff: " << max_diff << "\n";
    
    // Clean up
    free_csr_matrix(csr_matrix);
    free(A);
    free(B);
    free(C);
    free(C2);
}









