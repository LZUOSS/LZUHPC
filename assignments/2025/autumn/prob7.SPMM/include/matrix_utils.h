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
//生成随机稠密矩阵格式的两个函数，确保每次生成结果相同,多线程版本，固定线程数
template<typename T>
void Gen_Matrix(T * a, int rows,int cols){
   
    const int num_threads=8;
    #pragma omp parallel num_threads(num_threads)
    {
        int tid=omp_get_thread_num();
        std::mt19937_64 gen(20250828+tid);
        std::normal_distribution<T> dist(0, 2); 
        int chunk_size = (rows * cols+num_threads-1) / num_threads;
        for(int i=tid*chunk_size;i<(tid+1)*chunk_size && i<rows*cols;i++){
            a[i] =  dist(gen);
        }
    }
}
template<typename T>
void Gen_Matrix2(T * a, int rows,int cols){
    std::mt19937_64 gen(20250828);
    std::normal_distribution<T> dist(0, 2); 
    for(int i=0;i<rows*cols;i++){
        a[i] =  dist(gen);
    }
   
}

// 验证两个矩阵是否相等
template<typename T>
bool matrices_equal(const T* matrix1, const T* matrix2, int rows, int cols) {
    for (int i = 0; i < rows * cols; i++) {
        if (matrix1[i] != matrix2[i]) {
            return false;
        }
    }
    return true;
}
//获取两个矩阵之间的最大差异
template<typename T>
T max_diff_twoMatrix(const T* matrix1, const T* matrix2, int rows, int cols) {
    T max_diff=0;
    #pragma omp parallel for reduction(max:max_diff) schedule(static,256)
    for (int i = 0; i < rows * cols; i++) {
        if (matrix1[i] != matrix2[i]) {
            max_diff = std::max(max_diff, std::abs(matrix1[i] - matrix2[i]));
        }
    }
    return max_diff;
}
// 打印普通矩阵
template<typename T>
void print_dense_matrix(const T* matrix, int rows, int cols, const char* title = "Dense Matrix") {
    std::cout << title << " (" << rows << "x" << cols << "):\n";
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            std::cout << matrix[i*cols+j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}


// 释放普通矩阵内存
template<typename T>
void free_dense_matrix(T* dense_matrix) {
    if (dense_matrix) {
        free(dense_matrix);
    }
}
