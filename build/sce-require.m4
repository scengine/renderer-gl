dnl SCE_REQUIRE_HEADER(HEADER, IF-PRESENT)
dnl ----------------------------
dnl Check presence of HEADER and exits if not present, or executes IF-PRESENT if
dnl present
AC_DEFUN([SCE_REQUIRE_HEADER],
[
    AC_CHECK_HEADERS([$1],
                     [$2],
                     [AC_MSG_ERROR([$1 not found])])
])

dnl SCE_REQUIRE_LIB(LIBRARY, FUNC, IF-PRESENT)
dnl ----------------------------
dnl Check presence of HEADER and exits if not present, or executes IF-PRESENT if
dnl present
AC_DEFUN([SCE_REQUIRE_LIB],
[
    AC_CHECK_LIB([$1], [$2],
                 [$3],
                 [AC_MSG_ERROR([$1 not found])])
])
