/* This file is part of the MPIR Library.

The MPIR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation; either version 2.1 of the License, or (at
your option) any later version.
The MPIR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
License for more details.
You should have received a copy of the GNU Lesser General Public License
along with the MPIR Library; see the file COPYING.LIB. If not, write
to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.
*/
#ifndef _GETRUSAGE_H
#define _GETRUSAGE_H

#if defined(__cplusplus)
extern "C"
{
#endif

#define ENODATA 61
#define RUSAGE_SELF 0
#define RUSAGE_CHILDREN -1

typedef struct
{
    long tv_sec;
    long tv_usec;
} timeval;

typedef struct rusage
{
    timeval ru_utime; /* user time used */
    timeval ru_stime; /* system time used */
    long ru_maxrss; /* integral max resident set size */
    long ru_ixrss; /* integral shared text memory size */
    long ru_idrss; /* integral unshared data size */
    long ru_isrss; /* integral unshared stack size */
    long ru_minflt; /* page reclaims */
    long ru_majflt; /* page faults */
    long ru_nswap; /* swaps */
    long ru_inblock; /* block input operations */
    long ru_oublock; /* block output operations */
    long ru_msgsnd; /* messages sent */
    long ru_msgrcv; /* messages received */
    long ru_nsignals;/* signals received */
    long ru_nvcsw; /* voluntary context switches */
    long ru_nivcsw; /* involuntary context switches */
} rusage;

int getrusage(int who, rusage *usage);

#if defined(__cplusplus)
}
#endif

#endif
