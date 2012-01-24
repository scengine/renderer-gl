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
 
/* created: 06/03/2007
   updated: 23/01/2012 */

#ifndef SCERTYPES_H
#define SCERTYPES_H

/* include APIs */
#include <GL/glew.h>
#include <IL/il.h>
#include <IL/ilu.h>

#include <SCE/utils/SCEUtils.h>
#include <SCE/core/SCECore.h>   /* SCE_EPixelFormat & co */

#ifdef __cplusplus
extern "C" {
#endif

extern SCEenum sce_rgltypes[SCE_NUM_TYPES];
extern SCEenum *sce_rprimtypes;

/* TODO: what the hell are these doing here?? */
/* types de culling */
#define SCE_FRONT           GL_FRONT
#define SCE_BACK            GL_BACK

/* comparaisons */
#define SCE_LESS            GL_LESS
#define SCE_LEQUAL          GL_LEQUAL
#define SCE_GREATER         GL_GREATER
#define SCE_GEQUAL          GL_GEQUAL

int SCE_RTypeInit (void);
void SCE_RTypeQuit (void);

void SCE_RUsePatches (void);
void SCE_RUsePrimitives (void);

/* lol gl commands. */
/* deprecated */
int SCE_RGetInteger (SCEenum);
float SCE_RGetFloat (SCEenum);
void SCE_RGetIntegerv (SCEenum, int*);
void SCE_RGetFloatv (SCEenum, float*);

SCE_EType SCE_RGLTypeToSCE (SCEenum);

SCEenum SCE_RSCEImgTypeToGL (SCE_EImageType);
SCEenum SCE_RSCEImgFormatToGL (SCE_EImageFormat);
SCEenum SCE_RSCEPxfToGL (SCE_EPixelFormat);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
