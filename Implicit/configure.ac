# Information about project
AC_PREREQ(2.59)

AC_INIT([mpm],[0.1.3],[kks32@cam.ac.uk])
# Check for main file
AC_CONFIG_SRCDIR([cartesian/src/main.cpp])

# Check for automake for minimal API version 1.9
AM_INIT_AUTOMAKE()

# Specify Macro Directory
#AC_CONFIG_MACRO_DIR([m4])

# Check for C++ Compilers and libraries
AC_PROG_CXX
#AC_CHECK_HEADER_STDBOOL
AC_C_INLINE

# use the C++ compiler for the following checks
AC_LANG([C++])

# Use to enable C++11 Standard in the compiler
AX_CXX_COMPILE_STDCXX_11([ext],[mandatory])


# BOOST Library Check 
BOOST_REQUIRE([1.40.0])

CORLIB_PATH( , , AC_MSG_ERROR(Need path to CORLIB)) 

TCLAP_PATH( , , AC_MSG_ERROR(Need path to TCLAP)) 

EIGEN_PATH( , , AC_MSG_ERROR(Need path to EIGEN))

# Checks for header files and functions.
AC_HEADER_STDC
AC_CHECK_HEADERS([string])
AC_CHECK_HEADERS([iostream])
AC_CHECK_HEADERS([limits.h])

AC_CHECK_FUNCS([mkdir])
AC_CHECK_FUNCS([sqrt])
AC_CHECK_FUNCS([pow])


# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# distribute additional compiler and linker flags
# --> set these variables instead of CXXFLAGS or LDFLAGS
AC_SUBST([ACLOCAL_AMFLAGS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])
AC_SUBST([LIBS])


##########################################################################
# debug compilation support
##########################################################################

AC_MSG_CHECKING([whether to build with debug information])
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
        [enable debug data generation (def=no)])],
    [debugit="$enableval"],
    [debugit=no])
AC_MSG_RESULT([$debugit])

if test x"$debugit" = x"yes"; then
    AC_DEFINE([DEBUG],[],[Debug Mode])
    AM_CXXFLAGS="$AM_CXXFLAGS -g -Wall -Werror -Wno-uninitialized -O0 -fopenmp"
else
    AC_DEFINE([NDEBUG],[],[No-debug Mode])
    AM_CXXFLAGS="$AM_CXXFLAGS -O3 -fopenmp"
fi

##########################################################################

# Files to generate via autotools specify source file location
AC_CONFIG_FILES([Makefile])

# Generate the final Makefile
AC_OUTPUT
