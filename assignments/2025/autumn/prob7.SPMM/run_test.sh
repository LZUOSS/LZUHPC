export OMP_PROC_BIND=close
export OMP_NUM_THREADS=16

./build/spmm -m 4096 -n 4096 -k 4096 -s 0.9 -t 20
./build/spmm -m 8192 -n 4096 -k 8192 -s 0.9 -t 20
./build/spmm -m 16384 -n 2048 -k 64 -s 0.9 -t 20
./build/spmm -m 16384 -n 8192 -k 128 -s 0.95 -t 20
./build/spmm -m 16384 -n 4096 -k 512 -s 0.95 -t 20
./build/spmm -m 16384 -n 2048 -k 2048 -s 0.95 -t 20
./build/spmm -m 16384 -n 128 -k 4096 -s 0.95 -t 20
./build/spmm -m 16384 -n 64 -k 8192 -s 0.95 -t 20



