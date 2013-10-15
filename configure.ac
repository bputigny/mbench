#                                               -*- Autoconf -*-

AC_INIT([mbench], [0.2])

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
LT_INIT
AM_INIT_AUTOMAKE([foreign -Wall subdir-objects])
AC_PREREQ(2.63)
AC_PROG_CC
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_LIBTOOL
AC_LANG([C])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/lib/Makefile
	src/compiler/Makefile
	src/compiler/mbench_bc])
AC_CONFIG_COMMANDS([chmoding-scripts], [
	chmod +x src/compiler/mbench_bc])

AC_CHECK_FUNCS([strdup alloca malloc realloc gethostname getpagesize])
AC_CHECK_FUNCS([gettimeofday mkdir strstr])

AC_C_INLINE
AC_CONFIG_HEADERS([config.h])
AC_CHECK_HEADERS([malloc.h stdint.h stdlib.h string.h stddef.h libintl.h])
AC_CHECK_HEADERS([sys/ioctl.h])

AC_TYPE_UINT64_T
AC_TYPE_SIZE_T
AC_FUNC_ALLOCA
AC_FUNC_MALLOC
AC_FUNC_REALLOC

PAPI_FLAGS=""
AC_ARG_WITH([papi],
	[AS_HELP_STRING([--with-papi],
		[enable support for PAPI library])],
	[PAPI_LIBS="-L$withval/lib -lpapi" PAPI_CFLAGS="-I$withval/include" PAPI_CPPFLAGS="-DUSE_PAPI"],
	[with_papi=no])

AC_SUBST(PAPI_FLAGS)
CPPFLAGS="$CPPFLAGS $PAPI_CPPFLAGS"
CFLAGS="$CFLAGS $PAPI_CFLAGS -fopenmp -Wall"
LDFLAGS="$PAPI_LIBS $LDFLAGS"

AC_MSG_CHECKING([if the assembler supports vmovaps and %ymm0])
AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE([
    void f(void) { void *buffer; __asm__ __volatile__("vmovaps (%%rbx), %%ymm0;" : : "b"(buffer) : "%ymm0"); }
  ])], [avx_supported=yes], [avx_supported=no])
AC_MSG_RESULT($avx_supported)

AC_MSG_CHECKING([if AVX is enabled])
AC_ARG_ENABLE([avx],
	      AS_HELP_STRING([--disable-avx],
			[Disable AVX extention]),
	      [enable_avx=$enableval], [enable_avx=$avx_supported])
AC_MSG_RESULT($enable_avx)

AS_IF([test "x$enable_avx" != "xno"], [
	    CPPFLAGS="$CPPFLAGS -DUSE_AVX"
])

AC_OUTPUT
