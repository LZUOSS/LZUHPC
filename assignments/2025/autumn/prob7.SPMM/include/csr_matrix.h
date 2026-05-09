
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <random>
#include <ctime>
#include <omp.h>
#include "random"
#include <algorithm>  
#include <numeric>    
#include <vector>   
// CSR矩阵结构体模板
template<typename T>
struct CSRMatrix {
    T* values;           // 非零元素值数组
    int* col_indices;    // 列索引数组
    int* row_ptr;        // 行指针数组
    int rows;            // 矩阵行数
    int cols;            // 矩阵列数
    int nnz;             // 非零元素数量
};
// 矩阵访问宏，将二维索引转换为一维索引 (行优先存储)
#define MATRIX_INDEX(i, j, cols) ((i) * (cols) + (j))
// 普通矩阵转换为CSR格式
template<typename T>
CSRMatrix<T>* dense_to_csr(const T* dense_matrix, int rows, int cols) {
    // 分配CSR矩阵内存
    CSRMatrix<T>* csr_matrix = (CSRMatrix<T>*)malloc(sizeof(CSRMatrix<T>));
    csr_matrix->row_ptr = (int*)malloc((rows + 1) * sizeof(int));
    csr_matrix->rows = rows;
    csr_matrix->cols = cols;
    // 首先计算非零元素数量
    int nnz = 0;
    // int idx = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (dense_matrix[MATRIX_INDEX(i, j, cols)] != static_cast<T>(0)) {
                nnz++;
            }
        }
        csr_matrix->row_ptr[i + 1] = nnz;
    }
    csr_matrix->nnz = nnz;
    csr_matrix->values = (T*)malloc(nnz * sizeof(T));
    csr_matrix->col_indices = (int*)malloc(nnz * sizeof(int));

    // 填充CSR数据
    int idx = 0;
    csr_matrix->row_ptr[0] = 0;
    // #pragma omp parallel for schedule (static,128)
    for (int i = 0; i < rows; i++) {
        // int idx = csr_matrix->row_ptr[i];
        for (int j = 0; j < cols; j++) {
            T val = dense_matrix[MATRIX_INDEX(i, j, cols)];
            if (val != static_cast<T>(0)) {
                csr_matrix->values[idx] = val;
                csr_matrix->col_indices[idx] = j;
                idx++;
            }
        }
        // csr_matrix->row_ptr[i + 1] = idx;
    }
    
    return csr_matrix;
}

template<typename T>
void Gen_Matrix_sparsity(T * a, int rows, int cols, double sparsity = 0.0){
    std::mt19937_64 gen(20250828);
    std::normal_distribution<T> dist(0, 1);
    int total_elements = rows * cols;
    int no_zero_count = static_cast<int>(total_elements * (1.0-sparsity));
    memset(a, 0, sizeof(T) * total_elements);
    // 创建索引数组并打乱
    std::vector<int> indices(total_elements);
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), gen);
    // 将前no_zero_count个位置设为非零
    for(int i = 0; i < no_zero_count; i++){
        a[indices[i]] = dist(gen);
    }
}
// CSR格式转换为普通矩阵
template<typename T>
T* csr_to_dense(const CSRMatrix<T>* csr_matrix) {
    // 分配连续内存并初始化为0
    T* dense_matrix = (T*)calloc(csr_matrix->rows * csr_matrix->cols, sizeof(T));
    // 从CSR格式填充普通矩阵
    for (int i = 0; i < csr_matrix->rows; i++) {
        for (int j = csr_matrix->row_ptr[i]; j < csr_matrix->row_ptr[i + 1]; j++) {
            int col = csr_matrix->col_indices[j];
            T val = csr_matrix->values[j];
            dense_matrix[MATRIX_INDEX(i, col, csr_matrix->cols)] = val;
        }
    }
    return dense_matrix;
}

// 释放CSR矩阵内存
template<typename T>
void free_csr_matrix(CSRMatrix<T>* csr_matrix) {
    if (csr_matrix) {
        free(csr_matrix->values);
        free(csr_matrix->col_indices);
        free(csr_matrix->row_ptr);
        free(csr_matrix);
    }
}

// 打印CSR矩阵
template<typename T>
void print_csr_matrix(const CSRMatrix<T>* csr_matrix, const char* title = "CSR Matrix") {
    std::cout << title << " (" << csr_matrix->rows << "x" << csr_matrix->cols 
              << ", nnz=" << csr_matrix->nnz << "):\n";
    std::cout << "Row ptr: ";
    for (int i = 0; i <= csr_matrix->rows; i++) {
        std::cout << csr_matrix->row_ptr[i] << " ";
    }
    std::cout << "\n";
    std::cout << "Col indices: ";
    for (int i = 0; i < csr_matrix->nnz; i++) {
        std::cout << csr_matrix->col_indices[i] << " ";
    }
    std::cout << "\n";
    std::cout << "Values: ";
    for (int i = 0; i < csr_matrix->nnz; i++) {
        std::cout << csr_matrix->values[i] << " ";
    }
    std::cout << "\n";
}

// 计算矩阵稀疏度
template<typename T>
double calculate_sparsity(const T* matrix, int rows, int cols) {
    int zero_count = 0;
    int total = rows * cols;
    #pragma omp parallel for reduction(+:zero_count) schedule(static,128)
    for (int i = 0; i < total; i++) {
        if (matrix[i] == static_cast<T>(0)) {
            zero_count++;
        }
    }
    return static_cast<double>(zero_count) / total;
}
