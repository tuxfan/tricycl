/******************************************************************************\
 * Copyright (c) 2012 Los Alamos National Security, LLC
 * All rights reserved.
\******************************************************************************/

#define _tricycl_source

#include "tricycl_utils.h"

/*----------------------------------------------------------------------------*
 * I/O
 *----------------------------------------------------------------------------*/

void message(const char * format, ...) {
// only output messages if we're doing ENABLE_TRICYCL_VERBOSE
#if defined(ENABLE_TRICYCL_VERBOSE)
	va_list args;
	va_start(args, format);
	vfprintf(stdout, format, args);
#endif
} // message

void error(const char * format, ...) {
	va_list args;
	va_start(args, format);

	fprintf(stderr, "Error: ");
	vfprintf(stderr, format, args);
} // error

void warning(const char * format, ...) {
// only output warnings if we're doing ENABLE_TRICYCL_VERBOSE
#if defined(ENABLE_TRICYCL_VERBOSE)
	va_list args;
	va_start(args, format);

	fprintf(stderr, "Warning: ");
	vfprintf(stdout, format, args);
#endif
} // warning

/*----------------------------------------------------------------------------*
 * OpenCL error codes
 *----------------------------------------------------------------------------*/

const char * ocl_error_codes[65] = {
	"CL_SUCCESS",
	"CL_DEVICE_NOT_FOUND",
	"CL_DEVICE_NOT_AVAILABLE",
	"CL_COMPILER_NOT_AVAILABLE",
	"CL_MEM_OBJECT_ALLOCATION_FAILURE",
	"CL_OUT_OF_RESOURCES",
	"CL_OUT_OF_HOST_MEMORY",
	"CL_PROFILING_INFO_NOT_AVAILABLE",
	"CL_MEM_COPY_OVERLAP",
	"CL_IMAGE_FORMAT_MISMATCH",
	"CL_IMAGE_FORMAT_NOT_SUPPORTED",
	"CL_BUILD_PROGRAM_FAILURE",
	"CL_MAP_FAILURE",
	"CL_MISALIGNED_SUB_BUFFER_OFFSET",
	"CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST",
	"", "", "", "", "", // Gap from 15 - 29
	"", "", "", "", "",
	"", "", "", "", "",
	"CL_INVALID_VALUE", // Index 30
	"CL_INVALID_DEVICE_TYPE",
	"CL_INVALID_PLATFORM",
	"CL_INVALID_DEVICE",
	"CL_INVALID_CONTEXT",
	"CL_INVALID_QUEUE_PROPERTIES",
	"CL_INVALID_COMMAND_QUEUE",
	"CL_INVALID_HOST_PTR",
	"CL_INVALID_MEM_OBJECT",
	"CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
	"CL_INVALID_IMAGE_SIZE",
	"CL_INVALID_SAMPLER",
	"CL_INVALID_BINARY",
	"CL_INVALID_BUILD_OPTIONS",
	"CL_INVALID_PROGRAM",
	"CL_INVALID_PROGRAM_EXECUTABLE",
	"CL_INVALID_KERNEL_NAME",
	"CL_INVALID_KERNEL_DEFINITION",
	"CL_INVALID_KERNEL",
	"CL_INVALID_ARG_INDEX",
	"CL_INVALID_ARG_VALUE",
	"CL_INVALID_ARG_SIZE",
	"CL_INVALID_KERNEL_ARGS",
	"CL_INVALID_WORK_DIMENSION",
	"CL_INVALID_WORK_GROUP_SIZE",
	"CL_INVALID_WORK_ITEM_SIZE",
	"CL_INVALID_GLOBAL_OFFSET",
	"CL_INVALID_EVENT_WAIT_LIST",
	"CL_INVALID_EVENT",
	"CL_INVALID_OPERATION",
	"CL_INVALID_GL_OBJECT",
	"CL_INVALID_BUFFER_SIZE",
	"CL_INVALID_MIP_LEVEL",
	"CL_INVALID_GLOBAL_WORK_SIZE",
	"CL_INVALID_PROPERTY"
};

const char * error_to_string(int err) {
	err = -err;
	ASSERT(err>=0, "Invalid error code!");
	return ocl_error_codes[err];
}
