#include "test_case_cuda.h"
#include "csr_matrix.h"
#include <iostream>
#include <omp.h>
#include <cstdio>
#include "gemm.h"
#include <algorithm>
#include "matrix_utils.h"
#include "spmm_cuda_ref.h"
#include "spmm_ref.h"
#include "spmm_cusparse.h"
#include "spmm_cuda_opt.h"
#include <chrono>
#include <cusparse.h>


#define CHECK(call)                                   \
do                                                    \
{                                                     \
    const cudaError_t error_code = call;              \
    if (error_code != cudaSuccess)                    \
    {                                                 \
        printf("CUDA Error:\n");                      \
        printf("    File:       %s\n", __FILE__);     \
        printf("    Line:       %d\n", __LINE__);     \
        printf("    Error code: %d\n", error_code);   \
        printf("    Error text: %s\n",                \
            cudaGetErrorString(error_code));          \
        exit(1);                                      \
    }                                                 \
} while (0)


void test_spmm_cuda(const int m, const int n, const int k, const int test_time, const double sparsity) {
    // Host内存分配
    float* A = (float*)malloc(m * k * sizeof(float));
    float* B = (float*)malloc(k * n * sizeof(float));
    float* C = (float*)calloc(m * n, sizeof(float));
    float* C_gpu = (float*)calloc(m * n, sizeof(float));
    
    // 生成测试数据
    Gen_Matrix_sparsity(A, m, k, sparsity);
    Gen_Matrix(B, k, n);
    CSRMatrix<float>* csr_matrix = dense_to_csr(A, m, k);
    
    // 参考结果B
    spmm_cpu_ref(csr_matrix->row_ptr, csr_matrix->col_indices, csr_matrix->values, B, C, m, n,k);

    // Device内存分配
    int *d_ptr, *d_idx;
    float *d_val, *d_vin, *d_vout;
    cudaMalloc(&d_ptr, (m + 1) * sizeof(int));
    cudaMalloc(&d_idx, csr_matrix->nnz * sizeof(int));
    cudaMalloc(&d_val, csr_matrix->nnz * sizeof(float));
    cudaMalloc(&d_vin, k * n * sizeof(float));
    cudaMalloc(&d_vout, m * n * sizeof(float));
    // Host to Device
    cudaMemcpy(d_ptr, csr_matrix->row_ptr, (m + 1) * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_idx, csr_matrix->col_indices, csr_matrix->nnz * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_val, csr_matrix->values, csr_matrix->nnz * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_vin, B, k * n * sizeof(float), cudaMemcpyHostToDevice);
    // 性能测试
    float min_time = 1e6;

    cudaEvent_t start, stop;
    for(int i = 0; i < test_time; i++) {
        cudaMemset(d_vout, 0, m * n * sizeof(float));
        // cudaDeviceSynchronize();
        CHECK(cudaEventCreate(&start));
        CHECK(cudaEventCreate(&stop));
        CHECK(cudaEventRecord(start));
        cudaEventQuery(start);
        spmm_cuda_opt(d_ptr, d_idx, d_val, d_vin, d_vout, m, n,k);
        CHECK(cudaEventRecord(stop));
        CHECK(cudaEventSynchronize(stop));
        float elapsed_time=0;
        CHECK(cudaEventElapsedTime(&elapsed_time, start, stop));
        CHECK(cudaEventDestroy(start));
        CHECK(cudaEventDestroy(stop));
        min_time=min(elapsed_time,min_time);
    }
    // Device to Host
    cudaMemcpy(C_gpu, d_vout, m * n * sizeof(float), cudaMemcpyDeviceToHost);
    // 验证结果
    float max_diff = max_diff_twoMatrix(C_gpu, C, m, n);
    bool is_correct = (max_diff < 1e-3);
    
    std::cout << "CUDA SpMM COST TIME: " << min_time << " ms " ;
    double gflops=(2.0*csr_matrix->nnz*n*1e-9)/(min_time/1000);
    std::cout << "CUDA SpMM GFLOPS: " << gflops << std::endl;
    std::cout << (is_correct ? "correct √" : "false !!")<< " max diff: " << max_diff << "\n";
    // 清理内存
    free(A); free(B); free(C); free(C_gpu);
    cudaFree(d_ptr); cudaFree(d_idx); cudaFree(d_val); cudaFree(d_vin); cudaFree(d_vout);
    free_csr_matrix(csr_matrix);
}


void test_spmm_cusparse(const int m, const int n, const int k, const int test_time, const double sparsity) {
    // Host内存分配
    float* A = (float*)malloc(m * k * sizeof(float));
    float* B = (float*)malloc(k * n * sizeof(float));
    float* C_gpu = (float*)calloc(m * n, sizeof(float));
    
    // 生成测试数据
    Gen_Matrix_sparsity(A, m, k, sparsity);
    Gen_Matrix(B, k, n);
    CSRMatrix<float>* csr_matrix = dense_to_csr(A, m, k);
    // Device内存分配
    int *d_ptr, *d_idx;
    float *d_val, *d_vin, *d_vout;
    cudaMalloc(&d_ptr, (m + 1) * sizeof(int));
    cudaMalloc(&d_idx, csr_matrix->nnz * sizeof(int));
    cudaMalloc(&d_val, csr_matrix->nnz * sizeof(float));
    cudaMalloc(&d_vin, k * n * sizeof(float));
    cudaMalloc(&d_vout, m * n * sizeof(float));
    // Host to Device
    cudaMemcpy(d_ptr, csr_matrix->row_ptr, (m + 1) * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_idx, csr_matrix->col_indices, csr_matrix->nnz * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_val, csr_matrix->values, csr_matrix->nnz * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_vin, B, k * n * sizeof(float), cudaMemcpyHostToDevice);
    // 性能测试
    float min_time = 1e6;
    cusparseHandle_t handle;
    cusparseCreate(&handle);
    
    // 创建稀疏矩阵描述符（CSR格式）
    cusparseSpMatDescr_t matA;
    cusparseCreateCsr(&matA, m, k, csr_matrix->nnz, d_ptr, d_idx, d_val,
                      CUSPARSE_INDEX_32I, CUSPARSE_INDEX_32I,
                      CUSPARSE_INDEX_BASE_ZERO, CUDA_R_32F);

    // 创建稠密矩阵描述符（行优先存储）
    cusparseDnMatDescr_t matB;
    cusparseCreateDnMat(&matB, k, n, n, d_vin, CUDA_R_32F, CUSPARSE_ORDER_ROW);
    // 设置SpMM参数
    float alpha = 1.0f;
    float beta = 0.0;  
    // 计算所需缓冲区大小
    cusparseDnMatDescr_t matC;
    cusparseCreateDnMat(&matC, m, n, n, d_vout, CUDA_R_32F, CUSPARSE_ORDER_ROW);
    size_t bufferSize = 0;
    cusparseSpMM_bufferSize(handle, 
                            CUSPARSE_OPERATION_NON_TRANSPOSE, 
                            CUSPARSE_OPERATION_NON_TRANSPOSE,
                            &alpha, matA, matB, &beta, matC,
                            CUDA_R_32F, CUSPARSE_SPMM_ALG_DEFAULT, &bufferSize);
    void *dBuffer = nullptr;
    cudaMalloc(&dBuffer, bufferSize);

    // 分配缓冲区
   
    cudaEvent_t start, stop;
    for(int i = 0; i < test_time; i++) {
        cudaDeviceSynchronize();
        CHECK(cudaEventCreate(&start));
        CHECK(cudaEventCreate(&stop));
        CHECK(cudaEventRecord(start));
        cudaEventQuery(start);

        cusparseSpMM(handle, 
                 CUSPARSE_OPERATION_NON_TRANSPOSE, 
                 CUSPARSE_OPERATION_NON_TRANSPOSE,
                 &alpha, matA, matB, &beta, matC,
                 CUDA_R_32F, CUSPARSE_SPMM_ALG_DEFAULT, dBuffer);


        CHECK(cudaEventRecord(stop));
        CHECK(cudaEventSynchronize(stop));
        float elapsed_time=0;
        CHECK(cudaEventElapsedTime(&elapsed_time, start, stop));
        CHECK(cudaEventDestroy(start));
        CHECK(cudaEventDestroy(stop));
        min_time=min(elapsed_time,min_time);
    }
    // Device to Host
    cudaMemcpy(C_gpu, d_vout, m * n * sizeof(float), cudaMemcpyDeviceToHost);
  
    std::cout << "CUSPARSE COST TIME: " << min_time << " ms ";
    double gflops=(2.0*csr_matrix->nnz*n*1e-9)/(min_time/1000);
    std::cout << " CUSPARSE GFLOPS: " << gflops << std::endl;
    // 清理内存
    free(A); free(B); free(C_gpu);
    cudaFree(d_ptr); cudaFree(d_idx); cudaFree(d_val); cudaFree(d_vin); cudaFree(d_vout);
    free_csr_matrix(csr_matrix);
    
    cudaFree(dBuffer);
    cusparseDestroyDnMat(matB);
    cusparseDestroySpMat(matA);
    cusparseDestroyDnMat(matC);
    cusparseDestroy(handle);
}