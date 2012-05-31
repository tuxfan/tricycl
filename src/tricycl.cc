/*----------------------------------------------------------------------------*
 *----------------------------------------------------------------------------*/

#include <tricycl.hh>
#include <tricycl.h>

TriCyCL<float> & tsp = TriCyCL<float>::instance();
TriCyCL<double> & tdp = TriCyCL<double>::instance();

size_t tricycl_init_sp(cl_device_id id, cl_context context,
	cl_command_queue queue) {
	return tsp.init(id, context, queue);
} // tricycl_init_sp

int32_t tricycl_solve_sp(size_t token, size_t system_size, size_t num_systems,
	float * a, float * b, float * c, float * d, float * x) {
	return tsp.solve(token, system_size, num_systems, a, b, c, d, x);
} // tricycl_solve_sp
