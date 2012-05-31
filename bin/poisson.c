/*----------------------------------------------------------------------------*
 * 1-D Poisson problem
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include <tricycl.h>

typedef float real_t;

#define SQR(x) (x)*(x)

int main(int argc, char ** argv) {
	int32_t ierr;

	if(argc != 3) {
		fprintf(stderr, "Usage: %s <elements> <systems>\n", argv[0]);
		exit(1);
	} // if

	size_t elements = atoi(argv[1]);
	int32_t systems = atoi(argv[2]);

	/*-------------------------------------------------------------------------*
	 * Initialize OpenCL
	 *-------------------------------------------------------------------------*/

	cl_device_id device_id;
	cl_context context;
	cl_command_queue queue;
	cl_platform_id platform;

	ierr = clGetPlatformIDs(1, &platform, NULL);

	if(ierr != CL_SUCCESS) {
		fprintf(stderr, "clGetPlatformIDs failed with %d\n", ierr);
		exit(1);
	} // if

	int32_t cpu = 1;
	ierr = clGetDeviceIDs(platform, cpu == 1 ? CL_DEVICE_TYPE_CPU :
		CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

	if(ierr != CL_SUCCESS) {
		fprintf(stderr, "clGetDeviceIDs failed with %d\n", ierr);
		exit(1);
	} // if

	context = clCreateContext(0, 1, &device_id, NULL, NULL, &ierr);

	if(ierr != CL_SUCCESS) {
		fprintf(stderr, "clCreateContext failed with %d\n", ierr);
		exit(1);
	} // if

	queue = clCreateCommandQueue(context, device_id, 0, &ierr);

	if(ierr != CL_SUCCESS) {
		fprintf(stderr, "clCreateCommandQueue failed with %d\n", ierr);
		exit(1);
	} // if

	
	/*-------------------------------------------------------------------------*
	 * Initialize TriCyCL
	 *-------------------------------------------------------------------------*/
	size_t token = tricycl_init_sp(device_id, context, queue);

	/*-------------------------------------------------------------------------*
	 * Create system
	 *-------------------------------------------------------------------------*/

	real_t * sub = (real_t *)malloc(systems*elements*sizeof(real_t));
	real_t * diag = (real_t *)malloc(systems*elements*sizeof(real_t));
	real_t * sup = (real_t *)malloc(systems*elements*sizeof(real_t));
	real_t * rhs = (real_t *)malloc(systems*elements*sizeof(real_t));
	real_t * x = (real_t *)malloc(systems*elements*sizeof(real_t));
	real_t * analytic = (real_t *)malloc(systems*elements*sizeof(real_t));
	real_t x0 = 0.0;
	real_t x1 = 1.0;
	real_t h = (x1-x0)/(real_t)(elements-1);
	real_t hinv2 = 1.0/SQR(h);

	for(size_t s=0; s<systems; ++s) {
		for(size_t i=0; i<elements; ++i) {
			sub[s*elements + i] = hinv2*(-1.0);	
			diag[s*elements + i] = hinv2*(2.0);	
			sup[s*elements + i] = hinv2*(-1.0);	

			analytic[s*elements + i] = 1.0;
			rhs[s*elements + i] = (i==0 || i==(elements-1)) ? 1.0 : 0.0;

			x[s*elements + i] = 0.0;
		} // for

		sub[s*elements] = 0.0;
		sup[s*elements + elements-1] = 0.0;
	} // for

	/*-------------------------------------------------------------------------*
	 * Solve
	 *-------------------------------------------------------------------------*/
	ierr = tricycl_solve_sp(token, elements, systems, sub, diag, sup, rhs, x);

	return 0;
} // main
