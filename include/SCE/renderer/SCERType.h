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
 
/* created: 06/03/2007
   updated: 17/04/2010 */

#ifndef SCERTYPES_H
#define SCERTYPES_H

#include <SCE/utils/SCEUtils.h>

/* include APIs */
#include "SCE/renderer/SCEGLee.h"
#include <IL/il.h>
#include <IL/ilu.h>

#ifdef SCE_USE_CG
 #include <Cg/cg.h>
 #include <Cg/cgGL.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern SCEenum sce_rgltypes[9];
extern SCEenum sce_rprimtypes[5];

/* TODO: what the hell are these doing here?? */
/* types de culling */
#define SCE_FRONT           GL_FRONT
#define SCE_BACK            GL_BACK

/* comparaisons */
#define SCE_LESS            GL_LESS
#define SCE_LEQUAL          GL_LEQUAL
#define SCE_GREATER         GL_GREATER
#define SCE_GEQUAL          GL_GEQUAL

/* matrices */
#define SCE_MAT_MODELVIEW   GL_MODELVIEW
#define SCE_MAT_PROJECTION  GL_PROJECTION
#define SCE_MAT_TEXTURE     GL_TEXTURE

int SCE_RTypeInit (void);
void SCE_RTypeQuit (void);

/* lol gl commands. */
/* deprecated */
int SCE_RGetInteger (SCEenum);
float SCE_RGetFloat (SCEenum);
void SCE_RGetIntegerv (SCEenum, int*);
void SCE_RGetFloatv (SCEenum, float*);

#if 0
SCEenum SCE_RGLType (SCE_EType);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
