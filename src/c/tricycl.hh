/*----------------------------------------------------------------------------*
 * TriCyCL class definitions.
 *----------------------------------------------------------------------------*/

#ifndef tricycl_hh
#define tricycl_hh

#include <vector>
#include <iostream>
#include <cmath>

#define _include_tricycl_h

#include <tricycl_local.h>
#include <tricycl_strings.h>
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
 * Struct for initialization from OCL-MLA
 *----------------------------------------------------------------------------*/

struct ocl_instance_t {
	cl_device_id id;
	cl_context context;	
	cl_command_queue queue;
}; // struct external_instance_t

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
		cl_kernel pcr_kernel;
		cl_kernel copy_kernel;

		solver_data_t(cl_device_id & _id, cl_context & _context,
			cl_command_queue & _queue)
			: id(_id), context(_context), queue(_queue) {}
	}; // struct solver_data_t

	/*-------------------------------------------------------------------------*
	 * Interface system data structure.
	 *-------------------------------------------------------------------------*/

	struct interface_t {
		size_t elements;
		real_t * a;
		real_t * b;
		real_t * c;
		real_t * d;
		real_t * x;

		interface_t()
			: elements(0), a(nullptr), b(nullptr), c(nullptr),
			d(nullptr), x(nullptr)
			{}

		interface_t(size_t _elements)
			: elements(0), a(nullptr), b(nullptr), c(nullptr),
			d(nullptr), x(nullptr)
			{
				allocate(_elements);
			} // interface_t

		~interface_t()
			{
				delete[] a;
				delete[] b;
				delete[] c;
				delete[] d;
				delete[] x;
			} // ~interface_t

		void allocate(size_t _elements) {
			delete[] a;
			delete[] b;
			delete[] c;
			delete[] d;
			delete[] x;

			a = new real_t[_elements];
			b = new real_t[_elements];
			c = new real_t[_elements];
			d = new real_t[_elements];
			x = new real_t[_elements];
			elements = _elements;
		} // allocate

		void print() {
			for(size_t i(0); i<elements; ++i) {
				std::cerr << a[i] << " " << b[i] << " " <<
					c[i] << " " << d[i] << " " << x[i] << std::endl;
			} // for
		} // operator <<

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

	/*-------------------------------------------------------------------------*
	 * Kernel inforamtion.
	 *-------------------------------------------------------------------------*/

	struct kernel_work_group_info_t {
		size_t global_work_size[3];
		size_t work_group_size;
		size_t compile_work_group_size[3];
		cl_ulong local_mem_size;
		size_t preferred_multiple;
		cl_ulong private_mem_size;
	}; // struct kernel_work_group_info

	/*-------------------------------------------------------------------------*
	 * Hide these.
	 *-------------------------------------------------------------------------*/

	TriCyCL() {}
	TriCyCL(const TriCyCL &) {}
	TriCyCL & operator = (const TriCyCL &);

	~TriCyCL() {}

	/*-------------------------------------------------------------------------*
	 * Create interface systems.
	 *-------------------------------------------------------------------------*/

	interface_t * create_interface_system(size_t system_size,
		size_t num_systems, size_t sub_size, size_t sub_systems,
		real_t * a, real_t * b, real_t * c, real_t * d);

	void create_buffer(cl_context & context, cl_mem_flags flags,
		size_t bytes, cl_mem & d_p, void * h_p);

	/*-------------------------------------------------------------------------*
	 * Get device info.
	 *-------------------------------------------------------------------------*/

	device_info_t get_device_info(cl_device_id & id);

	/*-------------------------------------------------------------------------*
	 * Get kernel info.
	 *-------------------------------------------------------------------------*/

	kernel_work_group_info_t get_kernel_work_group_info(cl_device_id & id,
		device_info_t & device_info, cl_kernel & kernel);

	/*-------------------------------------------------------------------------*
	 * Iterations for cyclic reduction.
	 *-------------------------------------------------------------------------*/

	size_t iterations(size_t elements) {
		size_t ita(elements/2);
		size_t cnt(0);
		do { ++cnt; ita/=2; } while(ita>1);
		return cnt;
	} // iterations

	/*-------------------------------------------------------------------------*
	 * Private data members.
	 *-------------------------------------------------------------------------*/

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
		1, (const char **)&tricycl_PPSTR, NULL, &ierr);

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
	_solver_data.pcr_kernel = clCreateKernel(_solver_data.program,
		"pcr_branch_free_kernel", &ierr);

	// create copy kernel
	_solver_data.copy_kernel = clCreateKernel(_solver_data.program,
		"uncouple", &ierr);

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
	CALLER_SELF
	int32_t ierr = 0;

	cl_device_id device_id = data_[token].id;
	cl_kernel pcr_kernel = data_[token].pcr_kernel;
	cl_kernel copy_kernel = data_[token].copy_kernel;
	cl_context context = data_[token].context;
	cl_command_queue queue = data_[token].queue;

	/*-------------------------------------------------------------------------*
	 * Get device and kernel information.
	 *-------------------------------------------------------------------------*/
	device_info_t device_info = get_device_info(device_id);
	kernel_work_group_info_t kernel_info =
		get_kernel_work_group_info(device_id, device_info, pcr_kernel);

	// FIXME: Testing
//kernel_info.work_group_size = 32;

	/*-------------------------------------------------------------------------*
	 * Sub-system calculations.
	 *-------------------------------------------------------------------------*/
#define MIN(x,y) (x) < (y) ? (x) : (y)
	size_t places = log2(kernel_info.work_group_size);
	kernel_info.work_group_size = 1<<places;
	// FIXME: Testing
	size_t work_group_size = MIN(kernel_info.work_group_size, 4096);
	size_t sub_size(work_group_size);
	size_t sub_local_memory((sub_size+1)*5*sizeof(real_t));
	size_t sub_systems(0);
#undef MIN

	if(system_size > work_group_size) {
		while((system_size%sub_size != 0 ||
			sub_local_memory > device_info.local_mem_size) && sub_size > 1) {
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

// FIXME: Testing
//message("sub_size: %d\n", (int)sub_size);

	/*-------------------------------------------------------------------------*
	 * Setup interface system.
	 *-------------------------------------------------------------------------*/
	size_t interface_size(2*sub_systems*num_systems);
	size_t interface_local_memory((interface_size+1)*5*sizeof(real_t));
	size_t interface_iterations(iterations(interface_size));
	size_t interface_systems(1);

	if(interface_size > kernel_info.work_group_size ||
		interface_local_memory > device_info.local_mem_size) {
		message("Interface system is too large for device! Unrecoverable!\n");
		std::exit(1);
	} // if

	interface_t * interface = create_interface_system(system_size,
		num_systems, sub_size, sub_systems, a, b, c, d);

	/*-------------------------------------------------------------------------*
	 * Create interface buffers.
	 *-------------------------------------------------------------------------*/
	cl_mem_flags is_flags = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR;
	cl_mem d_ia, d_ib, d_ic, d_id, d_ix;
	cl_mem d_a, d_b, d_c, d_d, d_x;

	create_buffer(context, is_flags, interface_size*sizeof(real_t),
		d_ia, interface->a);
	create_buffer(context, is_flags, interface_size*sizeof(real_t),
		d_ib, interface->b);
	create_buffer(context, is_flags, interface_size*sizeof(real_t),
		d_ic, interface->c);
	create_buffer(context, is_flags, interface_size*sizeof(real_t),
		d_id, interface->d);
	create_buffer(context, CL_MEM_WRITE_ONLY, interface_size*sizeof(real_t),
		d_ix, NULL);

	/*-------------------------------------------------------------------------*
	 * Create system buffers.
	 *-------------------------------------------------------------------------*/
	size_t full_size(system_size*num_systems);

	create_buffer(context, CL_MEM_READ_WRITE, full_size*sizeof(real_t),
		d_a, NULL);
	create_buffer(context, CL_MEM_READ_WRITE, full_size*sizeof(real_t),
		d_b, NULL);
	create_buffer(context, CL_MEM_READ_WRITE, full_size*sizeof(real_t),
		d_c, NULL);
	create_buffer(context, CL_MEM_READ_WRITE, full_size*sizeof(real_t),
		d_d, NULL);
	create_buffer(context, CL_MEM_WRITE_ONLY, full_size*sizeof(real_t),
		d_x, NULL);

	/*-------------------------------------------------------------------------*
	 * Set interface arguments.
	 *-------------------------------------------------------------------------*/
	ierr = 0;
	ierr |= clSetKernelArg(pcr_kernel, 0, sizeof(cl_mem), &d_ia);
	ierr |= clSetKernelArg(pcr_kernel, 1, sizeof(cl_mem), &d_ib);
	ierr |= clSetKernelArg(pcr_kernel, 2, sizeof(cl_mem), &d_ic);
	ierr |= clSetKernelArg(pcr_kernel, 3, sizeof(cl_mem), &d_id);
	ierr |= clSetKernelArg(pcr_kernel, 4, sizeof(cl_mem), &d_ix);
	ierr |= clSetKernelArg(pcr_kernel, 5,
		(interface_size+1)*5*sizeof(real_t), NULL);
	ierr |= clSetKernelArg(pcr_kernel, 6, sizeof(int32_t), &interface_size);
	ierr |= clSetKernelArg(pcr_kernel, 7, sizeof(int32_t), &interface_systems);
	ierr |= clSetKernelArg(pcr_kernel, 8, sizeof(int32_t),
		&interface_iterations);

	if(ierr != CL_SUCCESS) {
		CL_ABORTerr(clSetKernelArg, ierr);
	} // if

	/*-------------------------------------------------------------------------*
	 * Set copy arguments.
	 *-------------------------------------------------------------------------*/
	ierr = 0;
	ierr |= clSetKernelArg(copy_kernel, 0, sizeof(cl_mem), &d_a);
	ierr |= clSetKernelArg(copy_kernel, 1, sizeof(cl_mem), &d_b);
	ierr |= clSetKernelArg(copy_kernel, 2, sizeof(cl_mem), &d_c);
	ierr |= clSetKernelArg(copy_kernel, 3, sizeof(cl_mem), &d_d);
	ierr |= clSetKernelArg(copy_kernel, 4, sizeof(cl_mem), &d_ix);
	ierr |= clSetKernelArg(copy_kernel, 5, sizeof(int32_t), &system_size);
	ierr |= clSetKernelArg(copy_kernel, 6, sizeof(int32_t), &sub_size);

	if(ierr != CL_SUCCESS) {
		CL_ABORTerr(clSetKernelArg, ierr);
	} // if

	size_t offset(0);
	size_t global_size(interface_size);
	size_t local_size(interface_size);
	cl_event events[4];
	cl_event event;

	/*-------------------------------------------------------------------------*
	 * Solve interface system.
	 *-------------------------------------------------------------------------*/
	ierr = clEnqueueNDRangeKernel(queue, pcr_kernel, 1, &offset,
		&global_size, &local_size, 0, NULL, &event);

	if(ierr != CL_SUCCESS) {
		CL_ABORTkernel(clEnqueueNDRangeKernel, ierr, "solve");
	} // if

	/*-------------------------------------------------------------------------*
	 * Begin writing full system to device.
	 *-------------------------------------------------------------------------*/
	ierr = 0;
	ierr |= clEnqueueWriteBuffer(queue, d_a, 0, offset,
		full_size*sizeof(real_t), a, 0, NULL, &events[0]);
	ierr |= clEnqueueWriteBuffer(queue, d_b, 0, offset,
		full_size*sizeof(real_t), b, 0, NULL, &events[1]);
	ierr |= clEnqueueWriteBuffer(queue, d_c, 0, offset,
		full_size*sizeof(real_t), c, 0, NULL, &events[2]);
	ierr |= clEnqueueWriteBuffer(queue, d_d, 0, offset,
		full_size*sizeof(real_t), d, 0, NULL, &events[3]);
	
	if(ierr != CL_SUCCESS) {
		CL_ABORTerr(clSetKernelArg, ierr);
	} // if

	/*-------------------------------------------------------------------------*
	 * Block for interface solve kernel.
	 *-------------------------------------------------------------------------*/
	ierr = clWaitForEvents(1, &event);

	if(ierr != CL_SUCCESS) {
		CL_ABORTerr(clSetKernelArg, ierr);
	} // if

	/*-------------------------------------------------------------------------*
	 * Set full system arguments.
	 *-------------------------------------------------------------------------*/
	size_t sub_iterations(iterations(sub_size));
	ierr = 0;
	ierr |= clSetKernelArg(pcr_kernel, 0, sizeof(cl_mem), &d_a);
	ierr |= clSetKernelArg(pcr_kernel, 1, sizeof(cl_mem), &d_b);
	ierr |= clSetKernelArg(pcr_kernel, 2, sizeof(cl_mem), &d_c);
	ierr |= clSetKernelArg(pcr_kernel, 3, sizeof(cl_mem), &d_d);
	ierr |= clSetKernelArg(pcr_kernel, 4, sizeof(cl_mem), &d_x);
	ierr |= clSetKernelArg(pcr_kernel, 5,
		(interface_size+1)*5*sizeof(real_t), NULL);
	ierr |= clSetKernelArg(pcr_kernel, 6, sizeof(int32_t), &sub_size);
	ierr |= clSetKernelArg(pcr_kernel, 7, sizeof(int32_t), &sub_systems);
	ierr |= clSetKernelArg(pcr_kernel, 8, sizeof(int32_t), &sub_iterations);

	if(ierr != CL_SUCCESS) {
		CL_ABORTerr(clSetKernelArg, ierr);
	} // if

	/*-------------------------------------------------------------------------*
	 * Block for full system write.
	 *-------------------------------------------------------------------------*/
	ierr = clWaitForEvents(4, events);

	if(ierr != CL_SUCCESS) {
		CL_ABORTerr(clWaitForEvents, ierr);
	} // if

	/*-------------------------------------------------------------------------*
	 * Copy interface results into full system.
	 *-------------------------------------------------------------------------*/
	global_size = interface_size;
	local_size = interface_size/num_systems;

	ierr = clEnqueueNDRangeKernel(queue, copy_kernel, 1, &offset,
		&global_size, &local_size, 0, NULL, &event);

	if(ierr != CL_SUCCESS) {
		CL_ABORTkernel(clEnqueueNDRangeKernel, ierr, "solve");
	} // if

	/*-------------------------------------------------------------------------*
	 * Block for copy operation.
	 *-------------------------------------------------------------------------*/
	ierr = clWaitForEvents(1, &event);

	if(ierr != CL_SUCCESS) {
		CL_ABORTerr(clSetKernelArg, ierr);
	} // if

	/*-------------------------------------------------------------------------*
	 * Solve full system.
	 *-------------------------------------------------------------------------*/
	global_size = full_size;
	local_size = sub_size;

	ierr = clEnqueueNDRangeKernel(queue, pcr_kernel, 1, &offset,
		&global_size, &local_size, 0, NULL, &event);

	if(ierr != CL_SUCCESS) {
		CL_ABORTkernel(clEnqueueNDRangeKernel, ierr, "solve");
	} // if

	/*-------------------------------------------------------------------------*
	 * Block for full system solve kernel.
	 *-------------------------------------------------------------------------*/
	ierr = clWaitForEvents(1, &event);

	if(ierr != CL_SUCCESS) {
		CL_ABORTerr(clWaitForEvents, ierr);
	} // if

	/*-------------------------------------------------------------------------*
	 * Read full system solution.
	 *-------------------------------------------------------------------------*/
	ierr = clEnqueueReadBuffer(queue, d_x, 1, offset,
		system_size*num_systems*sizeof(real_t), x, 0, NULL, NULL);

	if(ierr != CL_SUCCESS) {
		CL_ABORTerr(clEnqueueReadBuffer, ierr);
	} // if

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
	
	interface_t * interface =
		new interface_t(2*num_systems*sub_systems);

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
				const real_t ratio = -1.0*a[roff+i]/ib[lroff+1];

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
				const real_t ratio = -1.0*c[roff+i]/ib[lroff];

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
 * Create device buffers.
 *----------------------------------------------------------------------------*/

template<typename real_t>
void
TriCyCL<real_t>::create_buffer(cl_context & context, cl_mem_flags flags,
	size_t bytes, cl_mem & d_p, void * h_p) {
	CALLER_SELF
	int32_t ierr = 0;

	d_p = clCreateBuffer(context, flags, bytes, h_p, &ierr);

	if(ierr != CL_SUCCESS) {
		CL_ABORTerr(clCreateBuffer, ierr);
	} // if
} // create_buffer

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
} // get_kernel_work_group_info

#endif // tricycl_hh
