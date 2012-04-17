/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2012  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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
   updated: 08/04/2012 */

#include <string.h>
#include <GL/glew.h>
#include <SCE/utils/SCEUtils.h>
#include <SCE/core/SCECore.h>   /* SCE_NUM_PRIMITIVE_TYPES */
#include "SCE/renderer/SCERType.h"

SCEenum sce_rgltypes[SCE_NUM_TYPES] = {
    GL_BYTE,                    /* SCE_NONE_TYPE */
    GL_BYTE,
    GL_UNSIGNED_BYTE,
    GL_SHORT,
    GL_UNSIGNED_SHORT,
    GL_INT,
    GL_UNSIGNED_INT,
    GL_FLOAT,
    GL_DOUBLE,
    GL_UNSIGNED_INT,             /* SCE_SIZE_T */
    0,                           /* SCE_NUM_NORMAL_TYPES */

    GL_HALF_FLOAT,
    GL_UNSIGNED_BYTE_3_3_2,
    GL_UNSIGNED_BYTE_2_3_3_REV,
    GL_UNSIGNED_SHORT_5_6_5,
    GL_UNSIGNED_SHORT_5_6_5_REV,
    GL_UNSIGNED_SHORT_4_4_4_4,
    GL_UNSIGNED_SHORT_4_4_4_4_REV,
    GL_UNSIGNED_SHORT_5_5_5_1,
    GL_UNSIGNED_SHORT_1_5_5_5_REV,
    GL_UNSIGNED_INT_8_8_8_8,
    GL_UNSIGNED_INT_8_8_8_8_REV,
    GL_UNSIGNED_INT_10_10_10_2,
    GL_UNSIGNED_INT_2_10_10_10_REV,
    GL_UNSIGNED_INT_24_8,
    GL_UNSIGNED_INT_10F_11F_11F_REV,
    GL_UNSIGNED_INT_5_9_9_9_REV,
    GL_FLOAT_32_UNSIGNED_INT_24_8_REV
};

SCEenum sce_rtextype[3] = {
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D
};

SCEenum sce_rtexformat[SCE_NUM_IMAGE_FORMATS] = {
    GL_RED,                     /* SCE_IMAGE_NONE */
    GL_RED,
    GL_RG,
    GL_RGB,
    GL_BGR,
    GL_RGBA,
    GL_BGRA,
    GL_DEPTH_COMPONENT,
    GL_STENCIL_INDEX,           /* NOTE: not sure if accepted by TexImage() */
    GL_DEPTH_STENCIL,
    GL_RED_INTEGER,
    GL_RG_INTEGER,
    GL_RGB_INTEGER,
    GL_BGR_INTEGER,
    GL_RGBA_INTEGER,
    GL_BGRA_INTEGER
};

SCEenum sce_rpxf[SCE_NUM_PIXEL_FORMATS] = {
    GL_LUMINANCE,               /* SCE_PXF_NONE */
    GL_LUMINANCE,
    GL_LUMINANCE_ALPHA,
    GL_RGB,
    GL_RGBA,
    GL_BGR,
    GL_BGRA,
    GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
    GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
    /*GL_COMPRESSED_RGB_3DC_ATI*/ 0,
    GL_DEPTH_COMPONENT24,
    GL_DEPTH_COMPONENT32,
    GL_DEPTH_COMPONENT32F,
    GL_STENCIL_INDEX,
    GL_STENCIL_INDEX1,
    GL_STENCIL_INDEX4,
    GL_STENCIL_INDEX8,
    GL_STENCIL_INDEX16,
    GL_DEPTH_STENCIL,
    GL_DEPTH24_STENCIL8,
    GL_DEPTH32F_STENCIL8,
    GL_R32UI,
    GL_RGBA8UI,
    GL_R16UI,
    GL_R8UI
};

SCEenum sce_rprimtypes_true[SCE_NUM_PRIMITIVE_TYPES] = {
    GL_POINTS,
    GL_LINES,
    GL_LINE_STRIP,
    GL_TRIANGLES,
    GL_TRIANGLE_STRIP,
    GL_TRIANGLE_FAN
};

SCEenum sce_rprimtypes_patches[SCE_NUM_PRIMITIVE_TYPES] = {
    GL_PATCHES,
    GL_PATCHES,
    GL_PATCHES,
    GL_PATCHES,
    GL_PATCHES,
    GL_PATCHES
};

SCEenum *sce_rprimtypes = sce_rprimtypes_true;

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


void SCE_RUsePatches (void)
{
    sce_rprimtypes = sce_rprimtypes_patches;
}
void SCE_RUsePrimitives (void)
{
    sce_rprimtypes = sce_rprimtypes_true;
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


SCEenum SCE_RSCEImgTypeToGL (SCE_EImageType t)
{
    return sce_rtextype[t];
}

SCEenum SCE_RSCEImgFormatToGL (SCE_EImageFormat f)
{
    return sce_rtexformat[f];
}

SCEenum SCE_RSCEPxfToGL (SCE_EPixelFormat p)
{
    return sce_rpxf[p];
}
