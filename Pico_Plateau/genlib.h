/****************************************************************************************
Copyright (c) 2023 Cycling '74

The code that Max generates automatically and that end users are capable of
exporting and using, and any associated documentation files (the “Software”)
is a work of authorship for which Cycling '74 is the author and owner for
copyright purposes.

This Software is dual-licensed either under the terms of the Cycling '74
License for Max-Generated Code for Export, or alternatively under the terms
of the General Public License (GPL) Version 3. You may use the Software
according to either of these licenses as it is most appropriate for your
project on a case-by-case basis (proprietary or not).

A) Cycling '74 License for Max-Generated Code for Export

A license is hereby granted, free of charge, to any person obtaining a copy
of the Software (“Licensee”) to use, copy, modify, merge, publish, and
distribute copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The Software is licensed to Licensee for all uses that do not include the sale,
sublicensing, or commercial distribution of software that incorporates this
source code. This means that the Licensee is free to use this software for
educational, research, and prototyping purposes, to create musical or other
creative works with software that incorporates this source code, or any other
use that does not constitute selling software that makes use of this source
code. Commercial distribution also includes the packaging of free software with
other paid software, hardware, or software-provided commercial services.

For entities with UNDER $200k in annual revenue or funding, a license is hereby
granted, free of charge, for the sale, sublicensing, or commercial distribution
of software that incorporates this source code, for as long as the entity's
annual revenue remains below $200k annual revenue or funding.

For entities with OVER $200k in annual revenue or funding interested in the
sale, sublicensing, or commercial distribution of software that incorporates
this source code, please send inquiries to licensing (at) cycling74.com.

The above copyright notice and this license shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Please see
https://support.cycling74.com/hc/en-us/articles/360050779193-Gen-Code-Export-Licensing-FAQ
for additional information

B) General Public License Version 3 (GPLv3)
Details of the GPLv3 license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
****************************************************************************************/

#ifndef GENLIB_H
#define GENLIB_H 1

#include "genlib_common.h"

//////////// genlib.h ////////////
// genlib.h -- max (gen~) version

#define GENLIB_NO_JSON


#ifndef GEN_WINDOWS
#	ifndef _SIZE_T
#		define	_SIZE_T
		typedef __typeof__(sizeof(int)) size_t;
#	endif
#endif

#ifndef __INT32_TYPE__
#	define __INT32_TYPE__ int
#endif

#ifdef MSP_ON_CLANG
// gen~ hosted:
typedef unsigned __INT32_TYPE__ uint32_t;
typedef unsigned __INT64_TYPE__ uint64_t;
typedef __INT32_TYPE__ int32_t;
typedef __INT64_TYPE__ int64_t;
#else
#	ifdef __GNUC__
#		include <stdint.h>
#	endif
#endif

#define inf				(__DBL_MAX__)
#define GEN_UINT_MAX	(4294967295)
#define TWO_TO_32		(4294967296.0)

#define C74_CONST const

// max_types.h:
#ifdef C74_X64
typedef unsigned long long t_ptr_uint;
typedef long long t_ptr_int;
typedef double t_atom_float;
typedef t_ptr_uint t_getbytes_size;
#else
typedef unsigned long t_ptr_uint;
typedef long t_ptr_int;
typedef float t_atom_float;
typedef short t_getbytes_size;
#endif // C74_X64

typedef uint32_t t_uint32;
typedef t_ptr_int t_atom_long;		// the type that is an A_LONG in an atom

typedef t_ptr_int t_int;			///< an integer  @ingroup misc
typedef t_ptr_uint t_ptr_size;		///< unsigned pointer-sized value for counting (like size_t)  @ingroup misc
typedef t_ptr_int t_atom_long;		///< the type that is an A_LONG in a #t_atom  @ingroup misc
typedef t_atom_long t_max_err;		///< an integer value suitable to be returned as an error code  @ingroup misc

extern "C" {
	extern t_sample gen_msp_pow (t_sample, t_sample);

#ifdef MSP_ON_CLANG
	// TODO: remove (for debugging only)
	//int printf(const char *fmt, ...);

	// math.h:
	extern double acos(double);
	extern double asin(double);
	extern double atan(double);
	extern double atan2(double, double);
	extern double cos(double);
	extern double sin(double);
	extern double tan(double);
	extern double acosh(double);
	extern double asinh(double);
	extern double atanh(double);
	extern double cosh(double);
	extern double sinh(double);
	extern double tanh(double);
	extern double exp(double);
	extern double log(double);
	extern double log10(double);
	extern double fmod(double, double);
	extern double modf(double, double *);
	extern double fabs(double);
	extern double hypot(double, double);
	extern double pow(double, double);

	extern double sqrt(double);
	extern double ceil(double);
	extern double floor(double);
	extern double trunc(double);
	extern double round(double);
	extern int abs(int);

	extern char *strcpy(char *, const char *);
#else
#	include <stdlib.h> // abs
#endif // MSP_ON_CLANG


#if defined(GENLIB_USE_ARMMATH) // ARM embedded support
#	include "arm_math.h"
#	define sin(x)		arm_sin_f32(x)
#	define sinf(x)		arm_sin_f32(x)
#	define cos(x)		arm_cos_f32(x)
#	define cosf(x)		arm_cos_f32(x)
#	define sqrt(x)		arm_sqrtf(x)
#	define sqrtf(x)		arm_sqrtf(x)
#	define rand(...)	arm_rand32()
#	undef RAND_MAX
#	define RAND_MAX		UINT32_MAX
#endif // GENLIB_USE_ARMMATH

#if defined(GENLIB_USE_FASTMATH)
#	include <math.h>
#	define tan(x)		fastertanfull(x)
#	define exp(x)		fasterexp(x)
#	define log2(x)		fasterlog2(x)
#	define pow(x,y)		fasterpow(x,y)
#	define pow2(x)		fasterpow2(x)
#	define atan2(x,y)	fasteratan2(x,y)
#	define tanh(x)		fastertanh(x)
#	if !defined(GENLIB_USE_ARMMATH)
#		define sin(x)	fastersinfull(x);
#		define cos(x)	fastercosfull(x);
#	endif
#endif // GENLIB_USE_FASTMATH

	// string reference handling:
	void *genlib_obtain_reference_from_string(const char *name);
	char *genlib_reference_getname(void *ref);

	// buffer handling:
	t_genlib_buffer *genlib_obtain_buffer_from_reference(void *ref);
	t_genlib_err genlib_buffer_edit_begin(t_genlib_buffer *b);
	t_genlib_err genlib_buffer_edit_end(t_genlib_buffer *b, long valid);
	t_genlib_err genlib_buffer_getinfo(t_genlib_buffer *b, t_genlib_buffer_info *info);
	void genlib_buffer_dirty(t_genlib_buffer *b);
	t_genlib_err genlib_buffer_perform_begin(t_genlib_buffer *b);
	void genlib_buffer_perform_end(t_genlib_buffer *b);

	// data handling:
	t_genlib_data *genlib_obtain_data_from_reference(void *ref);
	t_genlib_err genlib_data_getinfo(t_genlib_data *b, t_genlib_data_info *info);
	void genlib_data_resize(t_genlib_data *b, long dim, long channels);
	void genlib_data_setbuffer(t_genlib_data *b, void *ref);
	void genlib_data_release(t_genlib_data *b);
	void genlib_data_setcursor(t_genlib_data *b, long cursor);
	long genlib_data_getcursor(t_genlib_data *b);

	// other notification:
	void genlib_reset_complete(void *data);

	// get/set state of parameters
	size_t genlib_getstatesize(CommonState *cself, getparameter_method getmethod);
	short genlib_getstate(CommonState *cself, char *state, getparameter_method getmethod);
	short genlib_setstate(CommonState *cself, const char *state, setparameter_method setmethod);

}; // extern "C"

#define genlib_sysmem_newptr(s) sysmem_newptr(s)
#define genlib_sysmem_newptrclear(s) sysmem_newptrclear(s)
#define genlib_sysmem_resizeptr(p, s) sysmem_resizeptr(p, s)
#define genlib_sysmem_resizeptrclear(p, s) sysmem_resizeptrclear(p, s)
#define genlib_sysmem_ptrsize(p) sysmem_ptrsize(p)
#define genlib_sysmem_freeptr(p) sysmem_freeptr(p)
#define genlib_sysmem_copyptr(s, d, b) sysmem_copyptr(s, d, b)
#define genlib_set_zero64(d, n) set_zero64(d, n)
#define genlib_ticks systime_ticks

#endif // GENLIB_H

