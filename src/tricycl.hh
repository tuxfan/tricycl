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
#include <tricycl_utils.h>

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
	 * Interface system data structure.
	 *-------------------------------------------------------------------------*/

	struct interface_t {
		real_t * a;
		real_t * b;
		real_t * c;
		real_t * d;

		interface_t()
			: a(nullptr), b(nullptr), c(nullptr), d(nullptr)
			{}

		interface_t(size_t elements)
			: a(nullptr), b(nullptr), c(nullptr), d(nullptr)
			{
				allocate(elements);
			} // interface_t

		~interface_t()
			{
				delete[] a;
				delete[] b;
				delete[] c;
				delete[] d;
			} // ~interface_t

		void allocate(size_t elements) {
			delete[] a;
			delete[] b;
			delete[] c;
			delete[] d;

			a = new real_t[elements];
			b = new real_t[elements];
			c = new real_t[elements];
			d = new real_t[elements];
		} // allocate
	}; // struct interface_t

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

	/*-------------------------------------------------------------------------*
	 * Device inforamtion.
	 *-------------------------------------------------------------------------*/

	struct device_info_t {
		char platform_name[256];
		char platform_defines[1024];
		uint32_t version_major;
		uint32_t version_minor;
		char name[256];
		cl_device_type type;
		cl_uint vendor_id;
		cl_uint max_compute_units;
		cl_uint max_clock_frequency;
		size_t max_work_group_size;
		cl_uint max_work_item_dimensions;
		size_t max_work_item_sizes[3];
		cl_ulong local_mem_size;
	}; // struct device_info_t

	struct kernel_work_group_info_t {
		size_t global_work_size[3];
		size_t work_group_size;
		size_t compile_work_group_size[3];
		cl_ulong local_mem_size;
		size_t preferred_multiple;
		cl_ulong private_mem_size;
	}; // struct kernel_work_group_info

	TriCyCL() {}
	TriCyCL(const TriCyCL &) {}
	TriCyCL & operator = (const TriCyCL &);

	~TriCyCL() {}

	interface_t * create_interface_system(size_t system_size,
		size_t num_systems, size_t sub_size, size_t sub_systems,
		real_t * a, real_t * b, real_t * c, real_t * d);

	device_info_t get_device_info(cl_device_id & id);
	kernel_work_group_info_t get_kernel_work_group_info(cl_device_id & id,
		device_info_t & device_info, cl_kernel & kernel);

	size_t iterations(size_t elements) {
		size_t ita(elements/2);
		size_t cnt(0);
		do { ++cnt; ita/=2; } while(ita>1);
		return cnt;
	} // iterations

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
	_solver_data.kernel = clCreateKernel(_solver_data.program,
		"pcr_branch_free_kernel", &ierr);

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
	int32_t ierr = 0;

	cl_device_id device_id = data_[token].id;
	cl_kernel kernel = data_[token].kernel;

	device_info_t device_info = get_device_info(device_id);
	kernel_work_group_info_t kernel_info =
		get_kernel_work_group_info(device_id, device_info, kernel);

	size_t sub_size(kernel_info.work_group_size);
	size_t sub_local_memory((sub_size+1)*5*sizeof(real_t));
	size_t sub_systems(0);
	//size_t interface_systems(1);

	if(system_size > kernel_info.work_group_size) {
		while(system_size%sub_size != 0 && sub_size > 1 &&
			sub_local_memory > device_info.local_mem_size) {
			sub_size /= 2;
			sub_local_memory = (sub_size+1)*5*sizeof(real_t);
		} // while

		if(sub_size == 1) {
			message("System size must be evenly divisible by "
				"a valid work group size");
			std::exit(1);
		} // if

		sub_systems = system_size/sub_size;
	} // if

	size_t interface_size = 2*sub_systems*num_systems;
	size_t interface_local_memory = (interface_size+1)*5*sizeof(real_t);

	if(interface_local_memory > device_info.local_mem_size) {
		message("Interface system is too large for device! Unrecoverable!");
		std::exit(1);
	} // if

	interface_t * interface = create_interface_system(system_size,
		num_systems, sub_size, sub_systems, a, b, c, d);

	delete interface;

	return ierr;
} // TriCyCL<>::solve

/*----------------------------------------------------------------------------*
 * Create the interface system.
 *----------------------------------------------------------------------------*/

template<typename real_t>
typename TriCyCL<real_t>::interface_t *
TriCyCL<real_t>::create_interface_system(size_t system_size,
	size_t num_systems, size_t sub_size, size_t sub_systems,
	real_t * a, real_t * b, real_t * c, real_t * d) {
	
	interface_t * interface = new interface_t[num_systems*sub_size*sub_systems];

	real_t * ia = interface->a;
	real_t * ib = interface->b;
	real_t * ic = interface->c;
	real_t * id = interface->d;

	for(size_t s(0); s<num_systems; ++s) {
		const size_t soff = s*system_size; // input matrix offset
		const size_t lsoff = s*2*sub_systems; // interface matrix offset

		for(size_t r(0); r<sub_systems; ++r) {
			const size_t roff = soff + r*sub_size;
			const size_t lroff = lsoff + 2*r; // interface matrix sub offset

			// set initial in-place temporaries
			ia[lroff+1] = a[roff+1]; // tmp0
			ib[lroff+1] = b[roff+1]; // tmp1
			ic[lroff+1] = c[roff+sub_size-1];
			id[lroff+1] = d[roff+1]; // tmp3

			// eliminate interface sub-diagonal
			for(size_t i=2; i<sub_size; ++i) {
				const float ratio = -1.0*a[roff+i]/ib[lroff+1];

				ia[lroff+1] = ratio*ia[lroff+1];
				ib[lroff+1] = ratio*c[roff+i-1] + b[roff+i];
				// ic is static
				id[lroff+1] = ratio*id[lroff+1] + d[roff+i];
			} // for

			// set initial in-place temporaries
			ia[lroff] = a[roff];
			ib[lroff] = b[roff+sub_size-2];
			ic[lroff] = c[roff+sub_size-2];
			id[lroff] = d[roff+sub_size-2];

			// eliminate interface super-diagonal
			for(size_t i=sub_size-3; i != 0; --i) {
				const float ratio = -1.0*c[roff+i]/ib[lroff];

				// ia is static
				ib[lroff] = ratio*a[roff+i+1] + b[roff+i];
				ic[lroff] = ratio*ic[lroff];
				id[lroff] = ratio*id[lroff] + d[roff+i-1];
			} // for
		} // for
	} // for

	return interface;
} // create_interface_system

/*----------------------------------------------------------------------------*
 * Get device information.
 *----------------------------------------------------------------------------*/

template<typename real_t>
typename TriCyCL<real_t>::device_info_t
TriCyCL<real_t>::get_device_info(cl_device_id & id) {
	CALLER_SELF
	device_info_t info;
	
	CL_CHECKerr(clGetDeviceInfo, id, CL_DEVICE_VENDOR_ID,
		sizeof(info.vendor_id), &info.vendor_id, NULL);

	CL_CHECKerr(clGetDeviceInfo, id, CL_DEVICE_MAX_COMPUTE_UNITS,
		sizeof(cl_uint), &info.max_compute_units, NULL);

	CL_CHECKerr(clGetDeviceInfo, id, CL_DEVICE_MAX_CLOCK_FREQUENCY,
		sizeof(cl_uint), &info.max_clock_frequency, NULL);

	CL_CHECKerr(clGetDeviceInfo, id, CL_DEVICE_MAX_WORK_GROUP_SIZE,
		sizeof(size_t), &info.max_work_group_size, NULL);

	CL_CHECKerr(clGetDeviceInfo, id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
		sizeof(cl_uint), &info.max_work_item_dimensions, NULL);

	CL_CHECKerr(clGetDeviceInfo, id, CL_DEVICE_MAX_WORK_ITEM_SIZES,
		info.max_work_item_dimensions*sizeof(size_t),
		info.max_work_item_sizes, NULL);

	CL_CHECKerr(clGetDeviceInfo, id, CL_DEVICE_LOCAL_MEM_SIZE,
		sizeof(cl_ulong), &info.local_mem_size, NULL);

	return info;
} // TriCyCL<>::get_device_info

/*----------------------------------------------------------------------------*
 * Get kernel information.
 *----------------------------------------------------------------------------*/

template<typename real_t>
typename TriCyCL<real_t>::kernel_work_group_info_t
TriCyCL<real_t>::get_kernel_work_group_info(cl_device_id & id,
	device_info_t & device_info, cl_kernel & kernel) {
	CALLER_SELF
	int32_t ierr = 0;
	size_t param_value_size;

	kernel_work_group_info_t info;

#if 0
	// VERSION 1.2
	// maximum global work size that can be enqueued for this kernel
	param_value_size = 3*sizeof(size_t);
	ierr = clGetKernelWorkGroupInfo(kernel, id, CL_KERNEL_GLOBAL_WORK_SIZE,
		param_value_size, (void *)&info.global_work_size, NULL);

	if(ierr != CL_SUCCESS) {
		CL_ABORTerr(clGetKernelWorkGroupInfo, ierr);
	} // if
#else
	info.global_work_size[0] = 0;
	info.global_work_size[1] = 0;
	info.global_work_size[2] = 0;
#endif

	// maximum work group size that can be used for this kernel
	param_value_size = sizeof(size_t);
	ierr = clGetKernelWorkGroupInfo(kernel, id, CL_KERNEL_WORK_GROUP_SIZE,
		param_value_size, (void *)&info.work_group_size, NULL);

	if(ierr != CL_SUCCESS) {
		CL_ABORTerr(clGetKernelWorkGroupInfo, ierr);
	} // if

	// compile-time specificed work group size
	param_value_size = 3*sizeof(size_t);
	ierr = clGetKernelWorkGroupInfo(kernel, id,
		CL_KERNEL_COMPILE_WORK_GROUP_SIZE, param_value_size,
		(void *)&info.compile_work_group_size, NULL);

	if(ierr != CL_SUCCESS) {
		CL_ABORTerr(clGetKernelWorkGroupInfo, ierr);
	} // if

	// local memory used by this kernel
	param_value_size = sizeof(cl_ulong);
	ierr = clGetKernelWorkGroupInfo(kernel, id, CL_KERNEL_LOCAL_MEM_SIZE,
		param_value_size, (void *)&info.local_mem_size, NULL);

	if(ierr != CL_SUCCESS) {
		CL_ABORTerr(clGetKernelWorkGroupInfo, ierr);
	} // if

	// preferred work group size multiple for this kernel
	if(device_info.version_major >= 1 && device_info.version_minor >= 1) {

// dummy value to enable compilation on older OpenCL installations
#ifndef CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE
#define CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE 0
#endif
		param_value_size = sizeof(size_t);
		ierr = clGetKernelWorkGroupInfo(kernel, id,
			CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, param_value_size,
			(void *)&info.preferred_multiple, NULL);

		if(ierr != CL_SUCCESS) {
			CL_ABORTerr(clGetKernelWorkGroupInfo, ierr);
		} // if
	}
	else {
		info.preferred_multiple = 1;
	} // if

	// private memory used by this kernel
	param_value_size = sizeof(cl_ulong);
	ierr = clGetKernelWorkGroupInfo(kernel, id, CL_KERNEL_PRIVATE_MEM_SIZE,
		param_value_size, (void *)&info.private_mem_size, NULL);

	if(ierr != CL_SUCCESS) {
		CL_ABORTerr(clGetKernelWorkGroupInfo, ierr);
	} // if

	return info;
} // ocl_kernel_work_group_info

#endif // tricycl_hh
