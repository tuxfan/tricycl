!------------------------------------------------------------------------------!
!
!------------------------------------------------------------------------------!

module tricycl_interface

   use, intrinsic :: ISO_C_BINDING
   use :: tricycl_bindings

   contains

   !---------------------------------------------------------------------------!
   ! tricycl_init_sp
   !---------------------------------------------------------------------------!

   subroutine tricycl_init_sp(id, context, queue, token)
      use, intrinsic :: ISO_C_BINDING
      implicit none
      type(c_ptr), value :: id
      type(c_ptr), value :: context
      type(c_ptr), value :: queue
      integer(c_size_t) :: token

      token = tricycl_init_sp_f90(id, context, queue)
   end subroutine tricycl_init_sp

   !---------------------------------------------------------------------------!
   ! tricycl_init_sp_ocl
   !---------------------------------------------------------------------------!

   subroutine tricycl_init_sp_ocl(instance, token)
      use, intrinsic :: ISO_C_BINDING
      implicit none
      type(c_ptr), value :: instance
      integer(c_size_t) :: token

      token = tricycl_init_sp_ocl_f90(instance)
   end subroutine tricycl_init_sp_ocl

   !---------------------------------------------------------------------------!
   ! tricycl_init_dp
   !---------------------------------------------------------------------------!

   subroutine tricycl_init_dp(id, context, queue, token)
      use, intrinsic :: ISO_C_BINDING
      implicit none
      type(c_ptr), value :: id
      type(c_ptr), value :: context
      type(c_ptr), value :: queue
      integer(c_size_t) :: token

      token = tricycl_init_dp_f90(id, context, queue)
   end subroutine tricycl_init_dp

   !---------------------------------------------------------------------------!
   ! tricycl_init_dp_ocl
   !---------------------------------------------------------------------------!

   subroutine tricycl_init_dp_ocl(instance, token)
      use, intrinsic :: ISO_C_BINDING
      implicit none
      type(c_ptr), value :: instance
      integer(c_size_t) :: token

      token = tricycl_init_dp_ocl_f90(instance)
   end subroutine tricycl_init_dp_ocl

   !---------------------------------------------------------------------------!
   ! tricycl_solve_sp
   !---------------------------------------------------------------------------!

   subroutine tricycl_solve_sp(token, system_size, num_systems, &
      a, b, c, d, x, ierr)
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

      ierr = tricycl_solve_sp_f90(token, system_size, num_systems, &
         a, b, c, d, x)
   end subroutine tricycl_solve_sp

   !---------------------------------------------------------------------------!
   ! tricycl_solve_dp
   !---------------------------------------------------------------------------!

   subroutine tricycl_solve_dp(token, system_size, num_systems, &
      a, b, c, d, x, ierr)
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

      ierr = tricycl_solve_dp_f90(token, system_size, num_systems, &
         a, b, c, d, x)
   end subroutine tricycl_solve_dp

end module tricycl_interface
