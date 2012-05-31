/*----------------------------------------------------------------------------*
 * TriCyCL class definitions.
 *----------------------------------------------------------------------------*/

#ifndef tricycl_hh
#define tricycl_hh

#include <vector>
#include <iostream>

#define _include_tricycl_h

#include <tricycl_local.h>
#include <tricycl_kernels.hh>

/*----------------------------------------------------------------------------*
 * Utility for selecting correct compiler options.
 *----------------------------------------------------------------------------*/

template<typename T> struct TypeToOpt {};

template<> struct TypeToOpt<float> {
	inline static const char * option_string() {
		return "-Dreal_t=float";
	} // option_string
}; // struct TypeToOpt

template<> struct TypeToOpt<double> {
	inline static const char * option_string() {
		return "-Dreal_t=double";
	} // option_string
}; // struct TypeToOpt

/*----------------------------------------------------------------------------*
 * TriCyCL class.
 *----------------------------------------------------------------------------*/

template<typename real_t>
class TriCyCL
{
public:

	/*-------------------------------------------------------------------------*
	 * Public typedefs.
	 *-------------------------------------------------------------------------*/

	typedef size_t data_token_t;

	/*-------------------------------------------------------------------------*
	 * OpenCL things that need to be stored.
	 *-------------------------------------------------------------------------*/

	struct solver_data_t {
		cl_device_id id;
		cl_context context;	
		cl_command_queue queue;
		cl_program program;
		cl_kernel kernel;

		solver_data_t(cl_device_id & _id, cl_context & _context,
			cl_command_queue & _queue)
			: id(_id), context(_context), queue(_queue) {}
	}; // struct solver_data_t

	/*-------------------------------------------------------------------------*
	 * Meyer's singleton instance method.
	 *-------------------------------------------------------------------------*/

	static TriCyCL & instance() {
		static TriCyCL t;
		return t;
	} // instance

	/*-------------------------------------------------------------------------*
	 * Initialize OpenCL stuff.
	 *-------------------------------------------------------------------------*/

	data_token_t init(cl_device_id & id, cl_context & context,
		cl_command_queue & queue);

	/*-------------------------------------------------------------------------*
	 * Solve method.
	 *-------------------------------------------------------------------------*/

	int32_t solve(data_token_t token, size_t system_size, size_t num_systems,
		real_t * a, real_t * b, real_t * c, real_t * d, real_t * x);

private:

	TriCyCL() {}
	TriCyCL(const TriCyCL &) {}
	TriCyCL & operator = (const TriCyCL &);

	~TriCyCL() {}

	std::vector<solver_data_t> data_;

}; // class TriCyCL

/*----------------------------------------------------------------------------*
 * Init
 *----------------------------------------------------------------------------*/

template<typename real_t>
typename TriCyCL<real_t>::data_token_t
TriCyCL<real_t>::init(cl_device_id & id, cl_context & context,
	cl_command_queue & queue) {
	int32_t ierr = 0;
	solver_data_t _solver_data(id, context, queue);

	// create program object
	_solver_data.program = clCreateProgramWithSource(_solver_data.context,
		1, (const char **)&pcr_kernels_PPSTR, NULL, &ierr);

	if(ierr != CL_SUCCESS) {
		std::cerr << "clCreateProgramWithSource failed with " <<
			ierr << std::endl;
		std::exit(1);
	} // if

	// compile the program
	const char * compile_options = TypeToOpt<real_t>::option_string();
	ierr = clBuildProgram(_solver_data.program, 1, &_solver_data.id,
		compile_options, NULL, NULL);

	// output something useful if the build fails
	if(ierr == CL_BUILD_PROGRAM_FAILURE) {
		char buffer[256*1024];
		size_t length;

		clGetProgramBuildInfo(_solver_data.program, _solver_data.id,
			CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);

		std::cerr << "clBuildProgram failed:" << std::endl <<
			buffer << std::endl << compile_options << std::endl;
		std::exit(1);
	} // if

	// create solver kernel
	_solver_data.kernel = clCreateKernel(_solver_data.program, "solve", &ierr);

	if(ierr != CL_SUCCESS) {
		std::cerr << "clCreateKernel failed with " << ierr << std::endl;
		std::exit(1);
	} // if

	data_.push_back(_solver_data);

	return data_.size()-1;
} // TriCyCL<>::init

/*----------------------------------------------------------------------------*
 * Solve.
 *----------------------------------------------------------------------------*/

template<typename real_t>
int32_t
TriCyCL<real_t>::solve(data_token_t token, size_t system_size,
	size_t num_systems, real_t * a, real_t * b, real_t * c, real_t * d,
	real_t * x) {
} // TriCyCL<>::solve

#endif // tricycl_hh
