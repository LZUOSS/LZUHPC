import numpy as np

# 参数
nx = 1000
ny = 1000
nt = 10000
dx = 1.0
dy = 1.0
dt = 0.01
kappa = 0.01

# 输入输出文件名
in_file = "pollution.in"
out_file = "pollution.out"

# 初始化污染物场
C = np.zeros((nx, ny), dtype=np.float64) + 1
C[nx//2-10:nx//2+10+1, ny//2-10:ny//2+10+1] = 100.0

# 写入输入文件
with open(in_file, "w") as f:
    f.write(f"{nx} {ny} {nt}\n")
    f.write(f"{dx} {dy} {dt} {kappa}\n")
    for i in range(nx):
        f.write(" ".join(f"{val:.6f}" for val in C[i, :]) + "\n")