### Problem Pollution

银河纪40025年，Alice 和 Bob 作为新人来到了工业信息化与通用计算部 （DIGU: Departement für Industrielle Generalinformatik und Universalrechnen）工作。

他们的第一份工作是来到星球索拉里斯Ⅲ上接收前任首席工程师 Canon 留下的烂摊子：一个未完成的污染模拟程序。好消息是在 Canon 抱着一箱子粉色能量饮料飞升二次元星球前，他已经写好了程序的大部分框架，事实上，根据CRT屏幕上的记录这个程序已经能够正确的运行。坏消息是程序过低的单核模拟速度，使其完全不能解决实际问题。但是在 Canon 去异世界当勇者前，他似乎留下了标记关于并行化的一些建议，但是一些不明液体把这些记录都污染了。Alice 和 Bob 相视一笑，这正是他们接受入职培训来这里工作的目的：使用最新的OpenMP模块优化程序实现并行化，并且赶在源石粉尘污染彻底席卷整个泰拉之前拯救索拉里斯大陆。

然而事态的发展超出了预期。时间不等人，超级地球派出了你乘坐空投仓前往支援 Alice 和 Bob 。在一切不可挽回前，请优化程序吧，指挥官！

#### 优化指南

程序需要模拟一个 $N \times M$ 大小的网格共 $T$ 时间步的污染扩散过程：
$$
C_{i,j}^{n+1} = 
\begin{cases} 
C_{i,j}^n & \text{如果 } i=1 \text{ 或 } i=M \text{ 或 } j=1 \text{ 或 } j=N \\
C_{i,j}^n + \kappa \Delta t \left( \frac{C_{i+1,j}^n - 2C_{i,j}^n + C_{i-1,j}^n}{(\Delta x)^2} + \frac{C_{i,j+1}^n - 2C_{i,j}^n + C_{i,j-1}^n}{(\Delta y)^2} \right) & \text{否则}
\end{cases}
$$
本题目标是掌握使用 **OpenMP** 对简单程序进行优化，程序的输入输出和核心计算部分已经给出，同时编译参数需要自行指定。

### 输入输出

#### 输入

第一行输入 $N ,M , T$

第二行输入 $\Delta x,\Delta y,\Delta t,\kappa$ 

 随后输入 $N$ 行 $M$ 列的数据

#### 输出

输出 $N$ 行 $M$ 列的数据

### 样例

#### 输入

```
3 3 1
1.0 1.0 0.1 1.0
0.000000 0.000000 0.000000
0.000000 1.000000 0.000000
0.000000 0.000000 0.000000
```

#### 输出

```
    0.000000     0.000000     0.000000
    0.000000     0.600000     0.000000
    0.000000     0.000000     0.000000
```

#### 评测细则
评测使用GNU Fortran 13.3.1 , 在16核心上跑, 通过精度为 $5\times 10^{-7}$

| 输入大小                     | 条件                | 分值 |
| ---------------------------- | ------------------- | ---- |
| $N,M \leq 10, T=1$           | 输出正确，默认时限  | 5    |
| $N,M \leq 10^3 , T\leq 10^4$ | 16线程测评，60s时限 | 95   |

其中测试点2的得分公式由以下给出
$$
S =
\begin{cases}
95 \times \frac{1}{1+e^{k(T-T_0)}} ,\text{其中}\ T_0=1.14514 , k=0.0721 & T > 14.4 \\
95 \times \frac{T_1}{T} ,\text{其中}\ T_1=4 & 4 < T \leq 14.4 \\
95 & T \leq 4
\end{cases}
$$


### Canon留下的代码

```fortran
program pollution
  implicit none
  integer :: nx, ny, nt, i, j, n
  real :: dx, dy, dt, kappa
  real, allocatable :: C(:,:), Cnew(:,:)
  character(len=100) :: in_file, out_file

  in_file  = 'pollution.in'
  out_file = 'pollution.out'

  open(unit=10, file=in_file, status='old', action='read')
  read(10,*) nx, ny, nt
  read(10,*) dx, dy, dt, kappa

  allocate(C(nx,ny), Cnew(nx,ny))

  do i = 1, nx
     read(10,*) (C(i,j), j=1,ny)
  end do
  close(10)

  Cnew = 0.0

  ! 被不明液体混合物污染的一行代码
  do n = 1, nt

     ! 被不明液体混合物污染的一行代码
     do i = 2, nx-1
        do j = 2, ny-1
           Cnew(i,j) = C(i,j) + kappa*dt * ( (C(i+1,j) - 2.0*C(i,j) + C(i-1,j))/(dx*dx) + &
                                             (C(i,j+1) - 2.0*C(i,j) + C(i,j-1))/(dy*dy) )
        end do
     end do

     ! 被不明液体混合物污染的多行代码
     ! 望向这片污渍，你握紧你的键盘......
     ! *这使你充满了决心
     ! 你突然意识到在这里还有什么事情没有完成
     ! *你感到一种奇怪的平静

  end do
  ! 被咖啡污渍污染的一行代码

  open(unit=11, file=out_file, status='replace', action='write')
  do i = 1, nx
     write(11,'(10000(F12.6, 1X))') (C(i,j), j=1,ny)
  end do
  close(11)

end program pollution
```

