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

template<typename T>
void Gen_Matrix(T * a, int size){
    const int num_threads=8;
    #pragma omp parallel num_threads(num_threads)
    {
        int tid=omp_get_thread_num();
        std::mt19937_64 gen(20250928+tid);
        std::normal_distribution<T> dist(0.1, 1); 
        int chunk_size = (size+num_threads-1) / num_threads;
        for(int i=tid*chunk_size;i<(tid+1)*chunk_size && i<size;i++){
            a[i] =  dist(gen);
        }
    }
    
}