///////////////////////////////////////////////////////////////////////////////
//
//  $Id: Nsound.h.in 875 2014-09-27 22:25:13Z weegreenblobbie $
//
//  Nsound is a C++ library and Python module for audio synthesis featuring
//  dynamic digital filters. Nsound lets you easily shape waveforms and write
//  to disk or plot them. Nsound aims to be as powerful as Csound but easy to
//  use.
//
//  Copyright (c) 2004, 2005 Nick Hilton
//
//  weegreenblobbie_at_yahoo_com
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Library General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _NSOUND_H_
#define _NSOUND_H_

#define PACKAGE_NAME "Nsound"

#define NSOUND_AUTHORS "Nick Hilton"
#define NSOUND_COPYRIGHT "Copyright 2004 - Present, Nick Hilton et al."

#define NSOUND_VERSION_A 0
#define NSOUND_VERSION_B 9
#define NSOUND_VERSION_C 4

#define PACKAGE_VERSION "0.9.4"
#define PACKAGE_RELEASE "Nsound-0.9.4"

// endianess
#define NSOUND_LITTLE_ENDIAN

// Platform
#define NSOUND_PLATFORM_OS_WINDOWS

// Build python module?
// no

// 64 bit floats enabled?
#define NSOUND_64_BIT 1

// Sample size
#define NSOUND_SAMPLE_SIZE sizeof(float64)

// M_PI macro
// M_PI is _NOT_ defined in math.h
#define M_PI 3.1415926535897932846

// Define if we have lib portaudio available
#define NSOUND_LIBPORTAUDIO 1

// Define if we have lib ao available
#undef NSOUND_LIBAO // disabled

// Define if we have Python module pylab (matplotlib) C API
#undef NSOUND_C_PYLAB  // disabled

// Ensure Python.h is included before anything else.
#if defined(NSOUND_C_PYLAB) || defined(NSOUND_IN_PYTHON_MODULE)

    #ifdef __clang__
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdeprecated"
        #pragma clang diagnostic ignored "-Wdeprecated-register"
    #endif

    #include <Python.h>

    #ifdef __clang__
        #pragma clang diagnostic pop
    #endif

#endif

// OpenMP usage
#undef NSOUND_OPENMP // disabled

#ifdef NSOUND_OPENMP
    #include <omp.h>
#endif

// C++11
#define NSOUND_CPP11 1

// Cuda usage
#undef NSOUND_CUDA // disabled

#ifdef NSOUND_CUDA
    #include <cuda.h>
#endif

// C++ headers
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>

// C header
#include <math.h>

#ifndef M_PI
    #define M_PI 3.141592653589793
#endif

//#ifndef SWIG
//	extern "C++" {
//#endif

///////////////////////////////////////////////////////////////////////////////
// Basic types

namespace Nsound
{

// setup boolean type
typedef bool boolean;

// Signed types

typedef signed char  byte;
typedef signed char  int8;
typedef signed short int16;
typedef signed int   int32;
typedef signed long long int64;

typedef float float32;
typedef double float64;

// unsigned types

typedef unsigned char ubyte;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

// Always declare these 64 bit types for use with Wavefiles
typedef signed long long raw_int64;
typedef unsigned long long raw_uint64;
typedef double raw_float64;


// A C++ exception class so our routines can raise exceptions.
struct Exception : public std::exception
{
	Exception(const std::string & message) : _message(message){};

	~Exception() throw() {};

	const char * what() const throw(){ return _message.c_str(); };

	private:

	std::string _message;
};

}; // namespace


///////////////////////////////////////////////////////////////////////////////
// Macros

// FIXME need to move these to Macros.h and prefix with M_

#define LINE_PREFIX __FILE__ << ":" << __LINE__ << ": "
#define ERROR_HEADER __FILE__ << ":" << __LINE__ << ": ***ERROR: "
#define WARNING_HEADER  __FILE__ << ":" << __LINE__ << ": ***WARNING: "

#include <Nsound/Macros.h>

//#ifndef SWIG
//	}; //extern "C++" {
//#endif

#endif

// :mode=c++: jEdit modeline