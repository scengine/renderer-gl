# Process this file with autoconf to produce a configure script.

# prelude
AC_PREREQ([2.60])
AC_INIT([SCEngine GL renderer library],
        [0.1.0],
        [martin.antony@yahoo.fr],
        [scerenderer])
AC_CONFIG_SRCDIR([src/SCERenderer.c])
AC_CONFIG_AUX_DIR([build/aux])
AC_CONFIG_MACRO_DIR([build/m4])
AM_INIT_AUTOMAKE([1.10 -Wall -Werror foreign])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# check for programs
LT_PREREQ([2.2.0])
LT_INIT
AC_PROG_CC

SCE_CHECK_DOXYGEN

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

SCE_CHECK_PTHREAD
SCE_CHECK_GL
# Renderer dependencies
PKG_CHECK_MODULES([GLEW],      [glew])
PKG_CHECK_MODULES([SCE_UTILS], [sceutils])
PKG_CHECK_MODULES([SCE_CORE],  [scecore])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
SCE_REQUIRE_FUNCS([memset strstr])

SCE_CHECK_DEBUG


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
echo "Debugging enabled              : $enable_debug"
echo "Base installation directory    : $prefix"
echo ""
echo "Configuration succeed."
echo ""
