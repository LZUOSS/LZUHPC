program test_netcdf_fortran
    use netcdf
    implicit none

    integer, parameter :: nx = 10
    integer, parameter :: ny = 5

    character(len=*), parameter :: filename = "test_netcdf_fortran.nc"

    integer :: ncid
    integer :: x_dimid, y_dimid
    integer :: varid
    integer :: dimids(2)

    integer :: data(nx, ny)
    integer :: data_read(nx, ny)

    integer :: i, j

    ! 初始化测试数组
    do j = 1, ny
        do i = 1, nx
            data(i, j) = i + 100 * j
        end do
    end do

    print *, "========================================"
    print *, " NetCDF-Fortran API test program"
    print *, "========================================"

    ! 创建 NetCDF 文件
    call check_status( &
        nf90_create(filename, NF90_CLOBBER, ncid), &
        "nf90_create" )

    ! 定义维度
    call check_status( &
        nf90_def_dim(ncid, "x", nx, x_dimid), &
        "nf90_def_dim: x" )

    call check_status( &
        nf90_def_dim(ncid, "y", ny, y_dimid), &
        "nf90_def_dim: y" )

    ! Fortran 中数组维度顺序为 data(nx, ny)
    dimids = (/ x_dimid, y_dimid /)

    ! 定义变量
    call check_status( &
        nf90_def_var(ncid, "sample_array", NF90_INT, dimids, varid), &
        "nf90_def_var" )

    ! 写入全局属性
    call check_status( &
        nf90_put_att(ncid, NF90_GLOBAL, "title", &
                     "NetCDF-Fortran build test file"), &
        "nf90_put_att: global title" )

    ! 结束定义模式
    call check_status( &
        nf90_enddef(ncid), &
        "nf90_enddef" )

    ! 写入数组数据
    call check_status( &
        nf90_put_var(ncid, varid, data), &
        "nf90_put_var" )

    ! 关闭文件
    call check_status( &
        nf90_close(ncid), &
        "nf90_close after write" )

    print *, "File written successfully: ", trim(filename)

    ! 重新打开文件进行读取测试
    call check_status( &
        nf90_open(filename, NF90_NOWRITE, ncid), &
        "nf90_open" )

    ! 获取变量 ID
    call check_status( &
        nf90_inq_varid(ncid, "sample_array", varid), &
        "nf90_inq_varid" )

    ! 读取数组
    data_read = -999

    call check_status( &
        nf90_get_var(ncid, varid, data_read), &
        "nf90_get_var" )

    ! 关闭文件
    call check_status( &
        nf90_close(ncid), &
        "nf90_close after read" )

    ! 校验读取结果
    if (any(data_read /= data)) then
        print *, "ERROR: data read from NetCDF file is not identical to original data."
        error stop 2
    else
        print *, "Data verification passed."
    end if

    print *, "========================================"
    print *, " NetCDF-Fortran test passed successfully"
    print *, "========================================"

contains

    subroutine check_status(status, api_name)
        integer, intent(in) :: status
        character(len=*), intent(in) :: api_name

        if (status /= NF90_NOERR) then
            print *, "NetCDF API call failed:"
            print *, "API    : ", trim(api_name)
            print *, "Status : ", status
            print *, "Message: ", trim(nf90_strerror(status))
            error stop 1
        else
            print *, "OK: ", trim(api_name)
        end if

    end subroutine check_status

end program test_netcdf_fortran