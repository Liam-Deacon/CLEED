/* header for OpenCL error codes */
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
char *err_code (cl_int err_in);