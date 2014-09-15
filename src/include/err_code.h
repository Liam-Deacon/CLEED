/* header for OpenCL error codes */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef CL_ERR_CODE_H
#define CL_ERR_CODE_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
char *err_code (cl_int err_in);

#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif