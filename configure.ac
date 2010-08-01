# Process this file with autoconf to produce a configure script.

# prelude
AC_PREREQ([2.64])
AC_INIT([SCEngine GL renderer library],
        [0.1.0],
        [martin.antony@yahoo.fr],
        [scerenderer],
        [http://scengine.tuxfamily.org/])
AC_CONFIG_SRCDIR([src/SCERenderer.c])
AC_CONFIG_AUX_DIR([build/aux])
AC_CONFIG_MACRO_DIR([build/m4])
AM_INIT_AUTOMAKE([1.11.1 -Wall -Werror foreign])

AM_SILENT_RULES([yes])

# check for programs
LT_PREREQ([2.2.0])
LT_INIT
AC_PROG_CC

# basic setup

#
# revision of the library
# 
# CURRENT:REVISION:AGE
#
# Please remeber to bump library version before releases
#   Remeberer:
# =====================================================================
# |   If you have...                              |   Bump version to |
# =====================================================================
# | Not changed the interface (bug fixes)         | CURRENT:REV+1:AGE |
# ---------------------------------------------------------------------
# | Augmented the interface (new functions)       | CURRENT+1:0:AGE+1 |
# ---------------------------------------------------------------------
# | Broken old interface (e.g. removed functions) | CURRENT+1:0:0     |
# ---------------------------------------------------------------------
SCE_RENDERER_LTVERSION="0:0:0"
AC_SUBST([SCE_RENDERER_LTVERSION])

pkgconfigdir="$libdir/pkgconfig"
sce_includedir="$includedir/SCE"
sce_include_rendererdir="$sce_includedir/renderer"
AC_SUBST(pkgconfigdir)
AC_SUBST(sce_includedir)
AC_SUBST(sce_include_rendererdir)

# legacy version names
AC_DEFINE([SCE_RENDERER_VERSION_STRING], [VERSION], [Version of the package])

# check for libs & headers

# AX_REQUIRE_HEADER(HEADER, IF-PRESENT)
# ----------------------------
# Check presence of HEADER and exit if not present, or execute IF-PRESENT if
# prsent
AC_DEFUN([AX_REQUIRE_HEADER],
         [AC_CHECK_HEADERS([$1],
                           [$2],
                           [AC_MSG_ERROR([[$1 not found]])])])

# AX_REQUIRE_LIB(LIBRARY, FUNC, IF-PRESENT)
# ----------------------------
# Check presence of HEADER and exit if not present, or execute IF-PRESENT if
# prsent
AC_DEFUN([AX_REQUIRE_LIB],
         [AC_CHECK_LIB([$1], [$2],
                       [$3],
                       [AC_MSG_ERROR([[$1 not found]])])])

# pthread
AX_REQUIRE_HEADER([pthread.h])
AX_REQUIRE_LIB([pthread], [pthread_create])
# GL/GLU
AX_REQUIRE_HEADER([GL/gl.h])
AX_REQUIRE_LIB([GL], [glEnable])
# IL/ILU
AX_REQUIRE_HEADER([IL/il.h])
AX_REQUIRE_LIB([IL], [ilInit])
AX_REQUIRE_HEADER([IL/ilu.h])
AX_REQUIRE_LIB([ILU], [iluInit])

# Cg
AC_ARG_ENABLE(
  [cg],
  AC_HELP_STRING([--enable-cg], [enable Cg shaders support [[default=no]]]),
  [enable_cg="$enableval"],
  [enable_cg="no"]
)
AC_MSG_CHECKING([[whether to compile Cg support]])
if test "x$enable_cg" = "xyes"; then
  AC_MSG_RESULT([[yes]])
  AX_REQUIRE_HEADER([Cg/cg.h])
  AX_REQUIRE_LIB([Cg], [cgSetErrorCallback])
  AX_REQUIRE_HEADER([Cg/cgGL.h])
  AX_REQUIRE_LIB([CgGL], [cgGLGetLatestProfile])
  AC_DEFINE([SCE_USE_CG], [1], [whether to use Cg or not])
else
  AC_MSG_RESULT([[no]])
fi

# Renderer dependencies
PKG_CHECK_MODULES([GLEW],      [glew])
PKG_CHECK_MODULES([SCE_UTILS], [sceutils])
PKG_CHECK_MODULES([SCE_CORE],  [scecore])
CFLAGS="${CFLAGS} ${GLEW_FLAGS} ${SCE_UTILS_CFLAGS} ${SCE_CORE_CFLAGS}"
LIBS="${LIBS} ${GLEW_LIBS} ${SCE_UTILS_LIBS} ${SCE_CORE_LIBS}"

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
# FIXME: these ones are not conditional, we need them
AC_CHECK_FUNCS([memset pow sqrt strerror strstr strtol strtoul])

# paranoiac compilation
AC_ARG_ENABLE(
  [paranoia],
  AC_HELP_STRING([--enable-paranoia], [enable paranioac compiler options [[default=no]]]),
  [enable_paranoia="$enableval"],
  [enable_paranoia="no"]
)
AC_MSG_CHECKING([[whether to enable paranoiac compiler options]])
if test "x$enable_paranoia" = "xyes"; then
  CFLAGS="$CFLAGS -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int \
          -Werror-implicit-function-declaration -Wmain \
          -Wparentheses -Wsequence-point -Wreturn-type -Wswitch \
          -Wtrigraphs -Wunused -Wuninitialized -Wunknown-pragmas \
          -Wfloat-equal -Wundef -Wshadow -Wpointer-arith \
          -Wbad-function-cast -Wwrite-strings \
          -Wconversion \
          -Wsign-compare -Waggregate-return -Wstrict-prototypes \
          -Wmissing-prototypes -Wmissing-declarations \
          -Wmissing-noreturn -Wformat -Wmissing-format-attribute \
          -Wpacked -Wredundant-decls -Wnested-externs \
          -Winline -Wlong-long -Wunreachable-code"
  AC_MSG_RESULT([[yes]])
else
  AC_MSG_RESULT([[no]])
fi

# Debugging flags
AC_ARG_ENABLE(
  [debug_all],
  AC_HELP_STRING([--enable-debug-all], [enable all paranoiac and commonly useless debugging [[default=no]]]),
  [enable_debug_all="$enableval"
   enable_optimisation="no"
   enable_debug_stack="yes"
   enable_debug="yes"],
  [enable_debug_all="no"]
)
AC_ARG_ENABLE(
  [optimisation],
  AC_HELP_STRING([--disable-optimisation], [disable any compiler optimisation [[default=no]]]),
  [enable_optimisation="$enableval"],
  [enable_optimisation="yes"]
)
AC_ARG_ENABLE(
  [debug_stack],
  AC_HELP_STRING([--enable-debug-stack], [enable stack debugging [[default=no]]]),
  [enable_debug_stack="$enableval"],
  [enable_debug_stack="no"]
)
AC_ARG_ENABLE(
  [debug],
  AC_HELP_STRING([--enable-debug], [enable debugging [[default=yes]]]),
  [enable_debug="$enableval"],
  [enable_debug="yes"]
)
# normal debugging
AC_MSG_CHECKING([[whether to enable debugging]])
if test "x$enable_debug" = "xyes"; then
  AC_DEFINE([SCE_DEBUG], [1], [is debugging enabled])
  DEBUG_CFLAGS="-DSCE_DEBUG"
  CFLAGS="$CFLAGS $DEBUG_CFLAGS"
  AC_MSG_RESULT([[yes]])
else
  DEBUG_CFLAGS=
  AC_MSG_RESULT([[no]])
fi
AC_SUBST([DEBUG_CFLAGS])
# stack debugging
AC_MSG_CHECKING([[whether to enable stack debugging]])
if test "x$enable_debug_stack" = "xyes"; then
  CFLAGS="$CFLAGS -fstack-protector-all"
  AC_MSG_RESULT([[yes]])
else
  AC_MSG_RESULT([[no]])
fi
# optimisation debugging (must be the last change to CFLAGS)
AC_MSG_CHECKING([[whether to disable any compiler optimisation]])
if test "x$enable_optimisation" = "xno"; then
  CFLAGS="$CFLAGS -O0"
  AC_MSG_RESULT([[yes]])
else
  AC_MSG_RESULT([[no]])
fi

# check to Doxygen and generate right target for doc according to its presence
AC_CHECK_PROGS([DOXYGEN], [doxygen], [:])
if test "x$DOXYGEN" = "x:"; then
  AC_MSG_WARN([[Doxygen not found, you will not able to generate documentation]])
  DOC_TARGET='@echo "no compatible doc generator found, please install one then re-run configure" >&2; exit 1'
else
  DOC_TARGET='$(DOXYGEN) Doxyfile'
fi
AC_SUBST([DOC_TARGET])

# output files
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile
                 Doxyfile
                 doc/Makefile
                 src/Makefile
                 include/Makefile
                 include/SCE/Makefile
                 include/SCE/renderer/Makefile
                 scerenderer.pc
                 ])
AC_OUTPUT

echo "------------------------------------------"
echo "SCERenderer version            : $VERSION ($SCE_RENDERER_LTVERSION)"
echo "Cg shaders support             : $enable_cg"
echo "Backtracer enabled             : $enable_bt"
echo "Debugging enabled              : $enable_debug"
echo "Paranoiac compiler options     : $enable_paranoia"
echo "Base installation directory    : $prefix"
echo ""
echo "Configuration succeed."
echo ""
