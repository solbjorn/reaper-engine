/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/

#ifndef _ODE_ODECONFIG_H_
#define _ODE_ODECONFIG_H_

/* Pull in the standard headers */
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>

#include <malloc.h>		// for alloca under windows

#ifdef __cplusplus
extern "C" {
#endif

/* Well-defined common data types...need to be defined for 64 bit systems */
#if defined(__aarch64__) || defined(__alpha__) || defined(__ppc64__) \
    || defined(__s390__) || defined(__s390x__) || defined(__zarch__) \
    || defined(__mips__) || defined(__powerpc64__) || defined(__riscv) \
    || defined(__loongarch64) || defined(__e2k__) \
    || (defined(__sparc__) && defined(__arch64__))
    #include <stdint.h>
    typedef int64_t         dint64;
    typedef uint64_t        duint64;
    typedef int32_t         dint32;
    typedef uint32_t        duint32;
    typedef int16_t         dint16;
    typedef uint16_t        duint16;
    typedef int8_t          dint8;
    typedef uint8_t         duint8;

    typedef intptr_t        dintptr;
    typedef uintptr_t       duintptr;
    typedef ptrdiff_t       ddiffint;
    typedef size_t          dsizeint;

#elif (defined(_M_IA64) || defined(__ia64__) || defined(_M_AMD64) || defined(__x86_64__)) && !defined(__ILP32__) && !defined(_ILP32)
  #define X86_64_SYSTEM   1
#if defined(_MSC_VER)
  typedef __int64         dint64;
  typedef unsigned __int64 duint64;
#else
#if defined(_LP64) || defined(__LP64__)
typedef long              dint64;
typedef unsigned long     duint64;
#else
  typedef long long       dint64;
  typedef unsigned long long duint64;
#endif
#endif
  typedef int             dint32;
  typedef unsigned int    duint32;
  typedef short           dint16;
  typedef unsigned short  duint16;
  typedef signed char     dint8;
  typedef unsigned char   duint8;

  typedef dint64          dintptr;
  typedef duint64         duintptr;
  typedef dint64          ddiffint;
  typedef duint64         dsizeint;

#else
#if defined(_MSC_VER)
  typedef __int64         dint64;
  typedef unsigned __int64 duint64;
#else
  typedef long long       dint64;
  typedef unsigned long long duint64;
#endif
  typedef int             dint32;
  typedef unsigned int    duint32;
  typedef short           dint16;
  typedef unsigned short  duint16;
  typedef signed char     dint8;
  typedef unsigned char   duint8;

  typedef dint32          dintptr;
  typedef duint32         duintptr;
  typedef dint32          ddiffint;
  typedef duint32         dsizeint;

#endif

/* Define the dInfinity macro */
#ifdef INFINITY
#ifdef dSINGLE
#define dInfinity ((float)INFINITY)
#else
#define dInfinity ((double)INFINITY)
#endif
#elif defined(HUGE_VAL)
#ifdef dSINGLE
#ifdef HUGE_VALF
#define dInfinity HUGE_VALF
#else
#define dInfinity ((float)HUGE_VAL)
#endif
#else
#define dInfinity HUGE_VAL
#endif
#else
#ifdef dSINGLE
#define dInfinity ((float)(1.0/0.0))
#else
#define dInfinity (1.0/0.0)
#endif
#endif


#define SHAREDLIBIMPORT __declspec (dllimport)

#define SHAREDLIBEXPORT __declspec (dllexport)



/* some types. assume `int' >= 32 bits */

typedef unsigned int    uint;

typedef int             int32;

typedef unsigned int    uint32;

typedef short           int16;

typedef unsigned short  uint16;

typedef char            int8;

typedef unsigned char   uint8;





/* an integer type that we can safely cast a pointer to and from without

 * loss of bits.

 */

typedef uintptr_t intP;





/* if we're compiling on a pentium, we may need to know the clock rate so

 * that the timing function can report accurate times. this number only needs

 * to be set accurately if we're doing performance tests - otherwise just

 * ignore this. i have not worked out how to determine this number

 * automatically yet.

 */



#ifdef PENTIUM

#ifndef PENTIUM_HZ

#define PENTIUM_HZ (496.318983e6)

#endif

#endif





/* the efficient alignment. most platforms align data structures to some

 * number of bytes, but this is not always the most efficient alignment.

 * for example, many x86 compilers align to 4 bytes, but on a pentium it is

 * important to align doubles to 8 byte boundaries (for speed), and the 4

 * floats in a SIMD register to 16 byte boundaries. many other platforms have

 * similar behavior. setting a larger alignment can waste a (very) small

 * amount of memory. NOTE: this number must be a power of two.

 */



#define EFFICIENT_ALIGNMENT 16





/* for unix, define this if your system supports anonymous memory maps

 * (linux does).

 */



#define MMAP_ANONYMOUS





#ifdef __cplusplus
}
#endif

#endif
