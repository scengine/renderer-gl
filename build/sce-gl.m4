dnl SCE_CHECK_GL
dnl -----------------
dnl Checks for OpenGL and fails if not found
AC_DEFUN([SCE_CHECK_GL],
[
    SCE_REQUIRE_HEADER([GL/gl.h])
    SCE_REQUIRE_LIB([GL], [glEnable])
    dnl those are fake flags in case they will become useful, but currently
    dnl AC_CHECK_LIB (called through SCE_REQUIRE_LIB) fills LIBS as needed
    AC_SUBST([GL_CFLAGS], [])
    AC_SUBST([GL_LIBS], [])
])
