# rm -rf build
mkdir build
cd build
cmake ..
make 
cd ..
export OMP_PROC_BIND=close
export OMP_NUM_THREADS=28

./build/gemm -m 2048 -n 2048 -k 2048 -t 10
./build/gemm -m 4096 -n 4096 -k 4096 -t 10
./build/gemm -m 1024 -n 1024 -k 1024 -t 10
./build/gemm -m 8192 -n 2048 -k 2048 -t 10
./build/gemm -m 4096 -n 2048 -k 4096 -t 10
./build/gemm -m 8192 -n 2048 -k 4096 -t 10
./build/gemm -m 1024 -n 8192 -k 4096 -t 10
./build/gemm -m 8192 -n 2048 -k 4096 -t 10

