/*----------------------------------------------------------------------------*
 * TriCyCL C interface.
 *----------------------------------------------------------------------------*/

#ifndef tricycl_h
#define tricycl_h

#include <stdint.h>
#include <stddef.h>

#define _include_tricycl_h

#include <tricycl_local.h>

#if defined(__cplusplus)
extern "C" {
#endif

size_t tricycl_init_sp(cl_device_id id, cl_context context,
	cl_command_queue queue);
size_t tricycl_init_dp(cl_device_id id, cl_context context,
	cl_command_queue queue);

#if 0
/*!
\page tricycl_solve_sp

\par Interface:
 */
int32_t tricycl_solve_sp(size_t token, size_t system_size, size_t num_systems,
	float * a, float * b, float * c, float * d, float * x);

/*!
\page tricycl_solve_dp

\par Interface:
 */
int32_t tricycl_solve_dp(size_t token, size_t system_size, size_t num_systems,
	double * a, double * b, double * c, double * d, double * x);
#endif

#if defined(__cplusplus)
}
#endif

#endif // tricycl_h
