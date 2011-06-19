/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2011  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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
 
/* created: 10/01/2007
   updated: 19/06/2011 */

#ifndef SCERMATRIX_H
#define SCERMATRIX_H

#include <SCE/utils/SCEUtils.h>
#include "SCE/renderer/SCERType.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SCE_RURRENT_MATRIX 0

enum sce_rmatrix {
    SCE_MAT_MODELVIEW = GL_MODELVIEW,
    SCE_MAT_PROJECTION = GL_PROJECTION,
    SCE_MAT_TEXTURE = GL_TEXTURE
};
typedef enum sce_rmatrix SCE_RMatrix;

void SCE_RLoadIdentityMatrix (void);

void SCE_RPushMatrix (void);
void SCE_RPopMatrix (void);

void SCE_RSetActiveMatrix (SCE_RMatrix);
SCE_RMatrix SCE_RGetActiveMatrix (void);

void SCE_RMultMatrix (const float*);
void SCE_RLoadMatrix (const float*);

void SCE_RGetMatrix (int, float*);

void SCE_RViewport (int, int, int, int);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
