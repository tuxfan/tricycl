!------------------------------------------------------------------------------!
!------------------------------------------------------------------------------!

module tricycl_bindings

interface

   !---------------------------------------------------------------------------!
   ! tricycl_init_sp_f90
   !---------------------------------------------------------------------------!

   function tricycl_init_sp_f90(id, context, queue) &
      result(ierr) bind(C, name="tricycl_init_sp")
      use, intrinsic :: ISO_C_BINDING
      implicit none
      type(c_ptr), value :: id
      type(c_ptr), value :: context
      type(c_ptr), value :: queue
      integer(c_int32_t) :: ierr
   end function tricycl_init_sp_f90

   !---------------------------------------------------------------------------!
   ! tricycl_init_dp_f90
   !---------------------------------------------------------------------------!

   function tricycl_init_dp_f90(id, context, queue) &
      result(ierr) bind(C, name="tricycl_init_dp")
      use, intrinsic :: ISO_C_BINDING
      implicit none
      type(c_ptr), value :: id
      type(c_ptr), value :: context
      type(c_ptr), value :: queue
      integer(c_int32_t) :: ierr
   end function tricycl_init_dp_f90

   !---------------------------------------------------------------------------!
   ! tricycl_solve_sp_f90
   !---------------------------------------------------------------------------!

   function tricycl_solve_sp_f90(token, system_size, num_systems, &
      a, b, c, d, x) &
      result(ierr) bind(C, name="tricycl_solve_sp")
      use, intrinsic :: ISO_C_BINDING
      implicit none
      integer(c_size_t), value :: token
      integer(c_size_t), value :: system_size
      integer(c_size_t), value :: num_systems
      type(c_ptr), value :: a
      type(c_ptr), value :: b
      type(c_ptr), value :: c
      type(c_ptr), value :: d
      type(c_ptr), value :: x
      integer(c_int32_t) :: ierr
   end function tricycl_solve_sp_f90

   !---------------------------------------------------------------------------!
   ! tricycl_solve_dp_f90
   !---------------------------------------------------------------------------!

   function tricycl_solve_dp_f90(token, system_size, num_systems, &
      a, b, c, d, x) &
      result(ierr) bind(C, name="tricycl_solve_dp")
      use, intrinsic :: ISO_C_BINDING
      implicit none
      integer(c_size_t), value :: token
      integer(c_size_t), value :: system_size
      integer(c_size_t), value :: num_systems
      type(c_ptr), value :: a
      type(c_ptr), value :: b
      type(c_ptr), value :: c
      type(c_ptr), value :: d
      type(c_ptr), value :: x
      integer(c_int32_t) :: ierr
   end function tricycl_solve_dp_f90

end interface
end module
