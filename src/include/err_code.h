
/*! \file
 *  \brief header for OpenCL error codes.
 */

#ifndef CL_ERR_CODE_H
#define CL_ERR_CODE_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifdef __APPLE__
  #include <OpenCL/opencl.h>
#else
  #include <CL/cl.h>
#endif

char *err_code(cl_int err_in);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* CL_ERR_CODE_H */


