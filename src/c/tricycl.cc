/*----------------------------------------------------------------------------*
 *----------------------------------------------------------------------------*/

#include <tricycl.hh>
#include <tricycl.h>

TriCyCL<float> & sp = TriCyCL<float>::instance();
TriCyCL<double> & dp = TriCyCL<double>::instance();

/*----------------------------------------------------------------------------*
 * Single-precision initialization
 *----------------------------------------------------------------------------*/

size_t tricycl_init_sp(cl_device_id id, cl_context context,
	cl_command_queue queue) {
	return sp.init(id, context, queue);
} // tricycl_init_sp

/*----------------------------------------------------------------------------*
 * Double-precision initialization
 *----------------------------------------------------------------------------*/

size_t tricycl_init_dp(cl_device_id id, cl_context context,
	cl_command_queue queue) {
	return dp.init(id, context, queue);
} // tricycl_init_dp

/*----------------------------------------------------------------------------*
 * Single-precision solver
 *----------------------------------------------------------------------------*/

int32_t tricycl_solve_sp(size_t token, size_t system_size, size_t num_systems,
	float * a, float * b, float * c, float * d, float * x) {
	return sp.solve(token, system_size, num_systems, a, b, c, d, x);
} // tricycl_solve_sp

/*----------------------------------------------------------------------------*
 * Double-precision solver
 *----------------------------------------------------------------------------*/

int32_t tricycl_solve_dp(size_t token, size_t system_size, size_t num_systems,
	double * a, double * b, double * c, double * d, double * x) {
	return dp.solve(token, system_size, num_systems, a, b, c, d, x);
} // tricycl_solve_dp
