python3 setup.py install
export OMP_NUM_THREADS=16
taskset -c 0-15 python3 src/main.py -F ./dat/128x128islands.rle -I 200
taskset -c 0-15 python3 src/main.py -F ./dat/128x128symmetric.rle -I 200
taskset -c 0-15 python3 src/main.py -F ./dat/128x128uniform.rle -I 200
taskset -c 0-15 python3 src/main.py -F ./dat/256x256islands.rle -I 100
taskset -c 0-15 python3 src/main.py -F ./dat/256x256symmetric.rle -I 100
taskset -c 0-15 python3 src/main.py -F ./dat/256x256uniform.rle -I 100
taskset -c 0-15 python3 src/main.py -F ./dat/384x384uniform.rle -I 40
taskset -c 0-15 python3 src/main.py -F ./dat/512x512uniform.rle -I 40
taskset -c 0-15 python3 src/main.py -F ./dat/768x768uniform.rle -I 30
taskset -c 0-15 python3 src/main.py -F ./dat/1024x1024uniform.rle -I 20

