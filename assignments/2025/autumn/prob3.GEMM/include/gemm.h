#include <cstdlib>
#include <iostream>
#include <cstring>
#include <ctime>
#include <omp.h>

//矩阵乘的原始版本
template<typename T>
void gemmOrigin(const T* A,const T * B,T * C,int m,int n,int k){
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            T temp=0;
            for (int l = 0; l < k; ++l) {
                temp+=A[i*k+l]*B[l*n+j];
            }
            C[i*n+j]=temp;
        }
    }
}
template<typename T>
void gemm(const T* A,const T * B,T * C,int m,int n,int k){
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < m; ++i) {
        for (int l = 0; l < k; ++l) {
            for (int j = 0; j < n; ++j) {
                C[i*n+j]+=A[i*k+l]*B[l*n+j];
            }
        }
    }
}
