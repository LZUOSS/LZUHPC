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