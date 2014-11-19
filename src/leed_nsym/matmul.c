/*********************************************************************
  GH/03.06.94
mat matmul( mat Mr, mat M1, mat M2 )
  Matrix multiplication.

Changes:
   GH/16.08.1994 - Mr can be equal to  M1 or M2
                  (Diagonal matrices are not included)
   GH/26.08.1994 - Error in the multiplication for complex matrices
                   corrected.
   LD/02.04.2014 - First attempt at OpenCL version of matmul code
  MGF/18.07.2014 - Workaround: 
                   replace native matrix multiplication by cblas_Xgemm
  
*********************************************************************/

/*! \file
 *
 * Implements matmul() function for matrix multiplication. 
 
 * The original Numerical recipes routines have been replaced with a
 * CBLAS one, which gives massive speed gains over the original textbook 
 * version (kindly contributed by Michael Fink <Michael.Fink@uibk.ac.at>).
 * 
 * \note An initial attempt to use OpenCL for GPGPU calculations has been added
 * but is not tested. It can be enabled by defining #_USE_OPENCL when compiling.
 */

#include <math.h>   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cblas.h>

#include "cblas_aux.h"
#include "mat.h"

#ifdef _USE_OPENCL
#include "err_code.h"
#ifndef CL_DEVICE
#define CL_DEVICE CL_DEVICE_TYPE_GPU   /* change as desired */
#endif
#endif

/*!
 * Multiplies two matrices \p Mr = \p M1 * \p M2
 *
 * \param[out] Mr Pointer to the matrix containing the result of the
 * multiplication. If \p Mr is passed in as \c NULL , the matrix will be
 * created and allocated memory.
 * \param[in] M1 Pointer to the first matrix to multiply.
 * \param[in] M2 Pointer to the second matrix to multiply.
 * \return Pointer to the resultant matrix.
 * \retval \c NULL if unsuccessful and #EXIT_ON_ERROR is not defined.
 */
mat matmul(mat Mr, const mat M1, const mat M2 )
{

  long int i;
  long int i_c2, i_r1;
  long int i_cr1, i_cr2;

  int result_num_type;

  real *cblas_m1, *cblas_m2, *cblas_mr; /* passed to cblas_Xgemm */

  mat Maux;

  Maux = NULL;

  /* check input matrices */

  /* check validity of the input matrices */
  if ((matcheck(M1) < 1) || (matcheck(M2) < 1))
  {

    #ifdef ERROR
    fprintf(STDERR, "*** error (matmul): ivalid input matrices\n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(NULL);
    #endif
  }

  /* check dimensions of input matrices */
  if (M1->cols != M2->rows)
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (matmul): "
            "dimensions of input matrices do not match\n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(NULL);
    #endif
  }
  
  /* Create cblas matrices */
  if((M1->num_type ==  NUM_REAL) && (M2->num_type ==  NUM_REAL) )
  {
   /* In this case we need:
    * - no intermediary storage for operands
    * - no conversion to complex
    */
   cblas_mr = calloc(M1->rows * M2->cols, sizeof(real));

   cblas_m1 = M1->rel + 1;   /* matrices are stored as row major */
   cblas_m2 = M2->rel + 1;

   result_num_type = NUM_REAL;

 }
 else
 {
   /* at least one operand is complex */
   cblas_mr = calloc(M1->rows * M2->cols, 2*sizeof(real));

   cblas_m1 = calloc(M1->rows * M1->cols, 2*sizeof(real));
   cblas_m2 = calloc(M2->rows * M2->cols, 2*sizeof(real));

   mat2cblas ( cblas_m1, NUM_COMPLEX, M1 ) ;
   mat2cblas ( cblas_m2, NUM_COMPLEX, M2 ) ;

   result_num_type = NUM_COMPLEX;
 }
  
  /* Create matrix Maux */
  if((M1->num_type ==  NUM_REAL) && (M2->num_type ==  NUM_REAL) )
  {
    Maux = matalloc(Maux, M1->rows, M2->cols, NUM_REAL);
  }
  else Maux = matalloc(Maux, M1->rows, M2->cols, NUM_COMPLEX);
   
  #ifdef _USE_OPENCL
/*====================================================================
 * Perform OpenCL matrix multiplication (GPU)
 *====================================================================*/
  
  cl_uint iplatform;
  cl_mem Ma, Mb, Mc;          /* Matrices in device memory */
  cl_int err;                 /* code returned from OpenCL calls */
  cl_device_id device_id;     /* compute device id */
  cl_context context;         /* compute context */
  cl_command_queue commands;  /* compute command queue */
  cl_program program;         /* compute program */
  cl_kernel kernel;           /* compute kernel */
  cl_platform_id* platform;   /* array of OpenCL platforms */ 
  
  char kernelsource[2000];
  strcpy(kernelsource, "__kernel void mmul( \n" \
                        " const int Mdim, \n" \
                        " const int Ndim, \n" \
                        " const int Pdim, \n" \
                        " __global float* A, \n" \
                        " __global float* B, \n" \
                        " __global float* C) \n" \
                        "{ \n" \
                        " int k; \n" \
                        " int i = get_global_id(0); \n" \
                        " int j = get_global_id(1); \n" \
                        " float tmp; \n" \
                        " if ( (i < Ndim) && (j <Mdim)) \n" \
                        " { \n" \
                        " tmp = 0.0; \n" \
                        " for(k=0;k<Pdim;k++) \n" \
                        " tmp += A[i*Ndim+k] * B[k*Pdim+j]; \n" \
                        " C[i*Ndim+j] = tmp; \n" \
                        " } \n" \
                        "} \n" \
                        "\n"
        );

  /*
   * ------------------------------------------------------------------
   * Create a context, queue and device.
   *------------------------------------------------------------------
   */

  /* Set up OpenCL context. queue, kernel, etc. */
  cl_uint numPlatforms;
    
  /* Find number of platforms */
  clGetPlatformIDs(0, NULL, &numPlatforms);
  if (numPlatforms == 0)
  {
    #ifdef ERROR
    fprintf(STDERR, "* warning (matmul): Failed to find an OpenCL platform\n");
    #endif
  }
  
  /* Get all platforms */
  if (numPlatforms) 
  {
    platform = (cl_platform_id*) malloc(sizeof(cl_platform_id) * numPlatforms);
    err = clGetPlatformIDs(numPlatforms, platform, NULL);
    if (err != CL_SUCCESS || numPlatforms == 0)
    {
      #ifdef ERROR
      fprintf(STDERR, "* warning (matmul): "
              "Failed to find an OpenCL platform\n");
      #endif
    }
  } /* numPlatforms */
  
  /* Secure a device */
  if (err == CL_SUCCESS) 
  {
    for (iplatform = 0; iplatform < numPlatforms; iplatform++)
    {
      err = clGetDeviceIDs(platform[iplatform], CL_DEVICE,
                           1, &device_id, NULL);
      if (err == CL_SUCCESS) break;
    }
    if (device_id == NULL)
    {
      #ifdef ERROR
      fprintf(STDERR, "*** warning (matmul): "
        "failed to create an OpenCL device group (%s)\n", err_code(err));
      #endif
    }

    /* Create a compute context */
    if (err == CL_SUCCESS)
    {
      context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
      if (!context)
      {
        #ifdef ERROR
        fprintf(STDERR, "* warning (matmul): "
            "Failed to create an OpenCL compute context (%s)\n", err_code(err));
        #endif
      }
    }

    /* Create a command queue */
    commands = clCreateCommandQueue(context, device_id, 0, &err);
    if (!commands)
    {
      #ifdef ERROR
      fprintf(STDERR, "* warning (matmul): "
        "Failed to create an OpenCL command queue (%s)\n", err_code(err));
      #endif
    }

    /*
     *-------------------------------------------------------------------------
     * Setup the buffers, initialize matrices, & write them into global memory
     *-------------------------------------------------------------------------
     */

    Ma = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                        sizeof(real) * M1->rows * M1->cols, M1->iel, &err);
    if (err != CL_SUCCESS)
    {
      #ifdef ERROR
      fprintf(STDERR, " *** warning (matmul): " 
              "failed to create buffer (%s)\n", err_code(err));
      #endif
    }
    Mb = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                        sizeof(real) * M2->rows * M2->cols, M2->iel, &err);
    if (err != CL_SUCCESS)
    {
      #ifdef ERROR
      fprintf(STDERR, " *** warning (matmul): " 
              "failed to create buffer (%s)\n", err_code(err));
      #endif
    }
    Mc = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                        sizeof(real) * Maux->rows * Maux->cols, NULL, &err);
    if (err != CL_SUCCESS)
    {
      #ifdef ERROR
      fprintf(STDERR, " *** warning (matmul): " 
              "failed to create buffer (%s)\n", err_code(err));
      #endif
    }


    /*
     *------------------------------------------------------------------------
     * OpenCL matrix multiplication ... Naive
     *------------------------------------------------------------------------
     */

    /* Create the compute program from the source buffer */
    program = clCreateProgramWithSource(context, 1, 
                    (const char **) &kernelsource, NULL, &err);
    if (err != CL_SUCCESS)
    {
      #ifdef ERROR
      fprintf(STDERR, "* warning (matmul): "
        "could not create OpenCL program (%s)\n", err_code(err));
      #endif
    }
    /* Build the program */
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];

        #ifdef ERROR
        fprintf(STDERR, " * warning (matmul): "
                "failed to build OpenCL program executable (%s)\n",
                err_code(err));
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 
                              sizeof(buffer), buffer, &len);
        fprintf(STDERR, "%s\n", buffer);
        #endif
    }

    /* check type of matrix multiplication to apply */
    switch(Maux->num_type)
    {
      /* real matrix */
      case (NUM_REAL):
      { 
    
        /* Create the compute kernel from the program */
        kernel = clCreateKernel(program, "mmul", &err);
        if (!kernel || err != CL_SUCCESS)
        {
          #ifdef ERROR
          fprintf(STDERR, "* warning (matmul): "
              "failed to create OpenCL compute kernel (%s)\n", err_code(err));
          #endif
        }
    
        /* Do the multiplication */
        /*err  = clSetKernelArg(kernel, 0, sizeof(int), &Mdim);
        err |= clSetKernelArg(kernel, 1, sizeof(int), &Ndim);
        err |= clSetKernelArg(kernel, 2, sizeof(int), &Pdim);
        err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &M1->);
        err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &M2->);
        err |= clSetKernelArg(kernel, 5, sizeof(cl_mem), &Maux->);
         */
        break;
      }
      
      /* complex matrix */
      case (NUM_COMPLEX):
      {    
 
        #ifdef CONTROL
        fprintf(STDCTR, "(matmul): entering OpenCL complex multiplication\n");
        #endif

        /* M1 and M2 are complex */
        if((M1->num_type ==  NUM_COMPLEX) && (M2->num_type ==  NUM_COMPLEX) )
        {
        
         ;
        }
        
        /*  
          Only M1 is complex 
        */

        else if (M1->num_type ==  NUM_COMPLEX)
        {
        
        
        
        } /* M1 is complex */

        /*  
          Only M2 is complex
        */

        else
        {
        
        }
        
        break;
        
      }  /* case COMPLEX */
    }
    
    if (err != CL_SUCCESS)
    {
      #ifdef ERROR
      fprintf(STDERR, " *** warning (matmul): " 
        "could not set kernel arguments\n");
      #endif
    }

    /* 
       Execute the kernel over the entire range of C matrix elements ... computing
       a dot product for each element of the product matrix. The local work
       group size is set to NULL ... so this tells the OpenCL runtime to
       figure out a local work group size for the program. 
    */
    const size_t global[2] = {10, 10};
    err = clEnqueueNDRangeKernel(
            commands, kernel, 2, NULL, global, NULL, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
      #ifdef ERROR
      fprintf(STDERR, " *** warning (matmul): " 
        "failed to execute kernel (%s)\n", err_code(err));
      #endif
    }

    err = clFinish(commands);
    if (err != CL_SUCCESS)
    {      
      #ifdef ERROR
      fprintf(STDERR, " *** warning (matmul): " 
       "waiting for queue to finish failed (%s)\n", err_code(err));
      #endif
    }

    err = clEnqueueReadBuffer(
        commands, Mc, CL_TRUE, 0,
        sizeof(real) * Maux->rows * Maux->cols , Maux,
        0, NULL, NULL);
    if (err != CL_SUCCESS)
    {      
      #ifdef ERROR
      fprintf(STDERR, " *** warning (matmul): " 
        "failed to read buffer (%s)\n", err_code(err));
      #endif
    }

    //------------------------------------------------------------------------
    // Clean up OpenCL objects
    //------------------------------------------------------------------------

    clReleaseMemObject(Ma);
    clReleaseMemObject(Mb);
    clReleaseMemObject(Mc);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);

    if (err == CL_SUCCESS)
    {
      return(Mr);
    }
    else 
    {
      #ifdef ERROR
      fprintf(STDERR, " *** warning (matmul): falling back to CPU matmul\n");
      #endif
    }
    
   } /* */
#endif

  /* Perform the multiplication (CPU only) */
  #ifdef CONTROL
  fprintf(STDCTR," (matmul) start multiplication \n");
  #endif

  switch(result_num_type)
  {
    /* real matrix */
    case (NUM_REAL):
    {
      register real rsum;
      register real *ptr1, *ptr2, *ptr_end;
      register real *ptrr;

      i_cr1 = M1->rows * M1->cols;
      i_cr2 = M2->rows * M2->cols;
  
      #ifdef CONTROL
      fprintf(STDCTR, " (matmul) entering real loop\n");
      #endif

      /*
       * i_r1 is the offset in M1 (current row),
       * i_c2 is the offset in M2 (current column)
       */
      for( ptrr = Maux->rel + 1, i_r1 = 1; i_r1 <= i_cr1; i_r1 += M1->cols)
      {
        for( i_c2 = 1; i_c2 <= M2->cols; i_c2 ++, ptrr ++)
        {
          rsum = 0;

          for (ptr1 = M1->rel + i_r1, ptr2 = M2->rel + i_c2,
               ptr_end = M1->rel + i_r1 + M1->cols;
               ptr1 < ptr_end; ptr1 ++, ptr2 += M2->cols)
          {
            rsum += *ptr1 * *ptr2;
          }

          *ptrr = rsum;
        }
      } /* for ptrr */

      break;
    } /* case REAL */

    /* complex matrix */
    case (NUM_COMPLEX):
    {
      i_cr1 = M1->rows * M1->cols;
      i_cr2 = M2->rows * M2->cols;
 
      #ifdef CONTROL
      fprintf(STDCTR, "(matmul): entering complex loop\n");
      #endif

      /* M1 and M2 are complex */
      if((M1->num_type ==  NUM_COMPLEX) && (M2->num_type ==  NUM_COMPLEX) )
      {
        register real rsum, isum;
        register real *ptri1, *ptri2, *ptr_end;
        register real *ptrr1, *ptrr2;
        register real *ptrr, *ptri;

        #ifdef CONTROL
        fprintf(STDCTR, "(matmul): M1 and M2 are complex \n");
        #endif

        for( ptrr = Maux->rel + 1, ptri = Maux->iel + 1, i_r1 = 1;
             i_r1 <= i_cr1; i_r1 += M1->cols)
        {
          for( i_c2 = 1; i_c2 <= M2->cols; i_c2 ++, ptrr ++, ptri ++)
          {
            rsum = 0;
            isum = 0;

            for (ptrr1 = M1->rel + i_r1, ptrr2 = M2->rel + i_c2,
                 ptri1 = M1->iel + i_r1, ptri2 = M2->iel + i_c2,
                 ptr_end = M1->rel + i_r1 + M1->cols;
                 ptrr1 < ptr_end;
                 ptrr1 ++, ptrr2 += M2->cols, ptri1 ++, ptri2 += M2->cols)
            {
              rsum += (*ptrr1 * *ptrr2) - (*ptri1 * *ptri2);
              isum += (*ptrr1 * *ptri2) + (*ptri1 * *ptrr2);
            }
 
            *ptrr = rsum;
            *ptri = isum;
          }
        } /* for ptrr */
      } /* both are complex */

      /* Only M1 is complex */
      else if (M1->num_type ==  NUM_COMPLEX)
      {
        register real rsum, isum;
        register real *ptri1, *ptr_end;
        register real *ptrr1, *ptrr2;
        register real *ptrr, *ptri;

        #ifdef CONTROL
        fprintf(STDCTR, "(matmul): only M1 is complex \n");
        #endif

        for( ptrr = Maux->rel + 1, ptri = Maux->iel + 1, i_r1 = 1;
             i_r1 <= i_cr1; i_r1 += M1->cols)
          for( i_c2 = 1; i_c2 <= M2->cols; i_c2 ++, ptrr ++, ptri ++)
          {
            rsum = 0;
            isum = 0;

            for (ptrr1 = M1->rel + i_r1, ptri1 = M1->iel + i_r1,
                 ptrr2 = M2->rel + i_c2, ptr_end = M1->rel + i_r1 + M1->cols;
                 ptrr1 < ptr_end;
                 ptrr1 ++, ptrr2 += M2->cols, ptri1 ++)
            {
              rsum += *ptrr1 * *ptrr2;
              isum += *ptri1 * *ptrr2;
            }
 
            *ptrr = rsum;
            *ptri = isum;
          } /* for ptrr */
      } /* M1 is complex */

      /* Only M2 is complex */
      else
      {
        register real rsum, isum;
        register real *ptrr1, *ptr_end;
        register real *ptri2, *ptrr2;
        register real *ptrr, *ptri;

        #ifdef CONTROL
        fprintf(STDCTR, "(matmul) only M2 is complex \n");
        #endif

        for( ptrr = Maux->rel + 1, ptri = Maux->iel + 1, i_r1 = 1;
             i_r1 <= i_cr1; i_r1 += M1->cols)
          for( i_c2 = 1; i_c2 <= M2->cols; i_c2 ++, ptrr ++, ptri ++)
          {
            rsum = 0;
            isum = 0;

            for (ptrr1 = M1->rel + i_r1, ptrr2 = M2->rel + i_c2,
                 ptri2 = M2->iel + i_c2, ptr_end = M1->rel + i_r1 + M1->cols;
                 ptrr1 < ptr_end;
                 ptrr1 ++, ptrr2 += M2->cols, ptri2 += M2->cols)
            {
              rsum += *ptrr1 * *ptrr2;
              isum += *ptrr1 * *ptri2;
            }
 
            *ptrr = rsum;
            *ptri = isum;
          } /* for ptrr */
      } /* M2 is complex */

      break;
    } /* case COMPLEX */
  } /* switch */

  /* Copy Maux into Mr */
  Mr = matcopy(Mr, Maux);
  matfree(Maux);
  return(Mr);
  
  i_cr2 = i_cr2 * 1;
}  /* end of function matmul */
