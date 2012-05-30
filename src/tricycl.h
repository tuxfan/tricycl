/*----------------------------------------------------------------------------*
 * TriCyCL C interface.
 *----------------------------------------------------------------------------*/

#ifndef tricycl_h
#define tricycl_h

#if defined(__cplusplus__)
extern "C" {
#endif

/*!
\page tricycl_solve_sp

\par Interface:
 */
int32_t tricycl_solve_sp(size_t system_size, size_t num_systems,
	float * a, float * b, float * c, float * d, float * x);

/*!
\page tricycl_solve_dp

\par Interface:
 */
int32_t tricycl_solve_dp(size_t system_size, size_t num_systems,
	double * a, double * b, double * c, double * d, double * x);

#if defined(__cplusplus__)
}
#endif

#endif // tricycl_h
