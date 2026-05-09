#include <iostream>
#include <omp.h>
#include <chrono>
#include "utils.h"
#include "reduce.h"
#include <cuda_runtime.h>


void test_reduce(const int len,const int iter_time){
    float * a =(float*)malloc(len * sizeof(float));
    Gen_Matrix<float>(a,len);
    float sum=0.0f;
    #pragma omp parallel for reduction(+:sum) schedule(static,1024)
    for(int i=0;i<len;i++){
        sum+=a[i];
    }
    // printf("CPU sum is %f\n",sum);
    double min_time=1e6;
    float max_diff=0.0f;
    float* d_in;
    cudaMalloc(&d_in, len * sizeof(float));
    cudaMemcpy(d_in,a,len*sizeof(float),cudaMemcpyHostToDevice);
    cudaDeviceSynchronize();
    for(int i=0;i<iter_time;i++){
        auto iter_start = std::chrono::high_resolution_clock::now();
        float gpu_sum=gpuReduce(d_in, len);
        auto iter_end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(iter_end - iter_start);
        if(std::abs(gpu_sum - sum) > max_diff){
            max_diff = std::abs(gpu_sum - sum);
        }
        min_time = std::min(duration.count() / 1e3,min_time); 
    }
    std::cout<<"len: "<<len<<" , time: "<<min_time<<" us"<<std::endl;
    if(max_diff>sum*1e-5){
        std::cout<<"Result incorrect! diff is "<<max_diff<<std::endl;
        free(a);
        return;
    }else{
        std::cout<<"Result correct! diff is "<<max_diff <<std::endl;
    }
    cudaFree(d_in);
    free(a);

}