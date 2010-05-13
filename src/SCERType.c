/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2010  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 -----------------------------------------------------------------------------*/
 
/* created: 07/03/2006
   updated: 17/04/2010 */

#include <string.h>
#include <SCE/utils/SCEUtils.h>
#include "SCE/renderer/SCEGLee.h"
#include "SCE/renderer/SCERType.h"

/* according to the definition in SCEType.h (TODO: ugly dependencie) */
SCEenum sce_rgltypes[9] = {
    GL_BYTE,
    GL_UNSIGNED_BYTE,
    GL_SHORT,
    GL_UNSIGNED_SHORT,
    GL_INT,
    GL_UNSIGNED_INT,
    GL_FLOAT,
    GL_DOUBLE,
    GL_UNSIGNED_INT             /* SCE_SIZE_T */
};

/* according to the definition in SCEType.h (TODO: ugly dependencie) */
SCEenum sce_rprimtypes[5] = {
    GL_POINTS,
    GL_LINES,
    GL_TRIANGLES,
    GL_TRIANGLE_STRIP,
    GL_TRIANGLE_FAN
};


int SCE_RTypeInit (void)
{
    /* check types size */
#define SCE_CHECK_SIZE(n) (sizeof (GL##n) != sizeof (SCE##n))
    /* check only critical types, ie those who might be used for heavy
       memory-to-memory transfers and computation */
    if (SCE_CHECK_SIZE (int) ||
        SCE_CHECK_SIZE (uint) ||
        SCE_CHECK_SIZE (short) ||
        SCE_CHECK_SIZE (ushort) ||
        SCE_CHECK_SIZE (byte) ||
        SCE_CHECK_SIZE (ubyte) ||
        /* TODO: check for it */
#ifdef SCE_HAVE_HALF_FLOAT_SUPPORT
        SCE_CHECK_SIZE (half) ||
#endif
        SCE_CHECK_SIZE (float) ||
        SCE_CHECK_SIZE (double)) {
        SCEE_Log (666);
        SCEE_LogMsg ("one of the SCE utils type size differs from GL's");
        return SCE_ERROR;
    }
#undef SCE_CHECK_SIZE
    return SCE_OK;
}
void SCE_RTypeQuit (void)
{
}

/* TODO: those are deprecated */
int SCE_RGetInteger (SCEenum t)
{
    int v;
    glGetIntegerv (t, &v);
    return v;
}
float SCE_RGetFloat (SCEenum t)
{
    float v;
    glGetFloatv (t, &v);
    return v;
}
void SCE_RGetIntegerv (SCEenum t, int *v)
{
    glGetIntegerv (t, v);
}
void SCE_RGetFloatv (SCEenum t, float *v)
{
    glGetFloatv (t, v);
}

SCE_EType SCE_RGLTypeToSCE (SCEenum t)
{
#define SCE_TYPE_CASE(t)\
    case GL_##t: return SCE_##t

    switch (t) {
        SCE_TYPE_CASE (UNSIGNED_BYTE);
        SCE_TYPE_CASE (SHORT);
        SCE_TYPE_CASE (UNSIGNED_SHORT);
        SCE_TYPE_CASE (INT);
        SCE_TYPE_CASE (UNSIGNED_INT);
        SCE_TYPE_CASE (FLOAT);
        SCE_TYPE_CASE (DOUBLE);
    }
    return 0;
#undef SCE_TYPE_CASE
}

#if 0
SCEenum SCE_RSCETypeToGL (SCE_EType t)
{
    return rgltypes[t];
}
#endif
