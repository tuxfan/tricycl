/******************************************************************************\
 * Copyright (c) 2012 Los Alamos National Security, LLC
 * All rights reserved.
\******************************************************************************/

#if !defined(_tricycl_source) && !defined(_include_tricycl_h)
#error "Error: do not include this file directly, use #include <tricycl.h>"
#endif

#ifndef tricycl_utils_h
#define tricycl_utils_h

#if defined(HAVE_CONFIG_H)
	#include <tricycl_config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/*------------------------------------------------------------------------------
 * Useful defines
 *----------------------------------------------------------------------------*/

#ifdef ENABLE_TRICYCL_ASSERTIONS
#define ASSERT(v, s)									\
	if((v) == 0) {										\
		message("Assertion Faile: %s\n", s);	\
		exit(1);											\
	}
#else
#define ASSERT(v, s)
#endif

#define STRINGIFY(token) #token
#define DEFINE_TO_STRING(define) STRINGIFY(define)

#if defined(__cplusplus)
extern "C" {
#endif

/*------------------------------------------------------------------------------
 * Output
 *----------------------------------------------------------------------------*/

void message(const char * format, ...);
void warning(const char * format, ...);
void error(const char * format, ...);

/*------------------------------------------------------------------------------
 * OpenCL error codes
 *----------------------------------------------------------------------------*/

const char * error_to_string(int err);

#if defined(__cplusplus)
} // extern
#endif

/*------------------------------------------------------------------------------
 * Macro definitions
 *----------------------------------------------------------------------------*/

#define CALLER_ARGS_DECL const char *caller_function,	\
	const char *caller_filename,								\
	unsigned caller_linenumber

#define CALLER_ARGS caller_function,caller_filename,caller_linenumber
#define CALLER_SELF										\
	const char * caller_function = __FUNCTION__;	\
	const char * caller_filename = __FILE__;		\
	unsigned caller_linenumber = __LINE__;

#define CL_ABORT()														\
	message("%s in file %s line %d\n",								\
		caller_function, caller_filename, caller_linenumber);	\
	exit(1);

#define CL_ABORTmalloc()												\
	message("Failed malloc in %s in file %s line %d\n",		\
		caller_function, caller_filename, caller_linenumber);	\
	exit(1);

#define CL_ABORTsanity(s) 														\
	message("%s\nSanity check failed in %s in file %s line %d\n",	\
		s, caller_function, caller_filename, caller_linenumber);		\
	exit(1);

#define CL_ABORTerr(call, err) 													\
	message("OpenCL call " #call													\
		" failed with %s(%d) in %s in file %s line %d\n",					\
		error_to_string(err), (err), caller_function, caller_filename,	\
		caller_linenumber);															\
	exit(1);

#define CL_ABORTkernel(call, err, name)							\
	message("OpenCL kernel %s failed in " #call					\
		" with %s(%d) in %s in file %s line %d\n",				\
		(name), error_to_string(err), (err), caller_function,	\
		caller_filename, caller_linenumber);						\
	exit(1);

#define CL_ABORTcreateKernel(err,name)			\
	CL_ABORTkernel(clCreateKernel,err,name)

#define CL_CHECKerr(call,...) {								\
		cl_int err;													\
		if((err = call(__VA_ARGS__)) != CL_SUCCESS) {	\
			CL_ABORTerr(call, err);								\
		}																\
	}

#define CL_CHECKerrKernel(kernel,call,...) {				\
		cl_int err;													\
		if ((err = call (__VA_ARGS__)) != CL_SUCCESS) {	\
			char name[256];										\
			size_t size;											\
			CL_CHECKerr(clGetKernelInfo,						\
							 kernel,									\
							 CL_KERNEL_FUNCTION_NAME,			\
							 sizeof(name), name, &size);		\
			CL_ABORTkernel(call,err,name);					\
		}																\
	}

#endif // tricycl_utils_h
