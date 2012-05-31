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
