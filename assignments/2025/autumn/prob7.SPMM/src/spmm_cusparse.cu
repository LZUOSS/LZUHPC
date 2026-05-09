#include <cusparse.h>
#include <spmm_cusparse.h>
#include <iostream>
void spmm_cusparse(int *d_ptr, int *d_idx, float *d_val, float *d_vin, float *d_vout, int m, int n, int k,int nnz)
{
    cusparseHandle_t handle;
    cusparseCreate(&handle);
    
    // 创建稀疏矩阵描述符（CSR格式）
    cusparseSpMatDescr_t matA;
    cusparseCreateCsr(&matA, m, k, nnz, d_ptr, d_idx, d_val,
                      CUSPARSE_INDEX_32I, CUSPARSE_INDEX_32I,
                      CUSPARSE_INDEX_BASE_ZERO, CUDA_R_32F);

    // 创建稠密矩阵描述符（行优先存储）
    cusparseDnMatDescr_t matB;
    cusparseCreateDnMat(&matB, k, n, n, d_vin, CUDA_R_32F, CUSPARSE_ORDER_ROW);
    cusparseDnMatDescr_t matC;
    cusparseCreateDnMat(&matC, m, n, n, d_vout, CUDA_R_32F, CUSPARSE_ORDER_ROW);
    // 设置SpMM参数
    float alpha = 1.0f;
    float beta = 0.0;  

    // 计算所需缓冲区大小
    size_t bufferSize = 0;
    cusparseSpMM_bufferSize(handle, 
                            CUSPARSE_OPERATION_NON_TRANSPOSE, 
                            CUSPARSE_OPERATION_NON_TRANSPOSE,
                            &alpha, matA, matB, &beta, matC,
                            CUDA_R_32F, CUSPARSE_SPMM_ALG_DEFAULT, &bufferSize);

    // 分配缓冲区
    void *dBuffer = nullptr;
    cudaMalloc(&dBuffer, bufferSize);

    // 执行SpMM
    cusparseSpMM(handle, 
                 CUSPARSE_OPERATION_NON_TRANSPOSE, 
                 CUSPARSE_OPERATION_NON_TRANSPOSE,
                 &alpha, matA, matB, &beta, matC,
                 CUDA_R_32F, CUSPARSE_SPMM_ALG_DEFAULT, dBuffer);

    // 清理资源
    cudaFree(dBuffer);
    cusparseDestroyDnMat(matC);
    cusparseDestroyDnMat(matB);
    cusparseDestroySpMat(matA);
    cusparseDestroy(handle);
}