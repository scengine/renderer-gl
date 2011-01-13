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
 
/* created: 15/12/2006
   updated: 13/01/2011 */

#ifndef SCERENDERER_H
#define SCERENDERER_H

/* external dependencies */
#include <stdio.h>
#include <SCE/utils/SCEUtils.h>

/* internal dependencies */
#include "SCE/renderer/SCERSupport.h"
#include "SCE/renderer/SCERMatrix.h"
#include "SCE/renderer/SCERBuffer.h"
#include "SCE/renderer/SCERVertexArray.h"
#include "SCE/renderer/SCERVertexBuffer.h"
#include "SCE/renderer/SCERImage.h"
#include "SCE/renderer/SCERTexture.h"
#include "SCE/renderer/SCERFramebuffer.h"
#include "SCE/renderer/SCERShader.h"
#include "SCE/renderer/SCERMaterial.h"
#include "SCE/renderer/SCERLight.h"
#include "SCE/renderer/SCEROcclusionQuery.h"
#include "SCE/renderer/SCERPointSprite.h"

#ifdef __cplusplus
extern "C" {
#endif

int SCE_RInit (FILE*, SCEbitfield);
void SCE_RQuit (void);

void SCE_RClearColor (float, float, float, float);
void SCE_RClearDepth (float);
void SCE_RClear (const SCEbitfield);
void SCE_RFlush (void);

void SCE_RSetState (SCEenum, int);
void SCE_RSetState2 (SCEenum, SCEenum, int);
void SCE_RSetState3 (SCEenum, SCEenum, SCEenum, int);
void SCE_RSetState4 (SCEenum, SCEenum, SCEenum, SCEenum, int);
void SCE_RSetState5 (SCEenum, SCEenum, SCEenum, SCEenum, SCEenum, int);
void SCE_RSetState6 (SCEenum, SCEenum, SCEenum, SCEenum, SCEenum, SCEenum, int);

void SCE_RSetBlending (SCEenum, SCEenum);

void SCE_RActivateColorBuffer (int);
void SCE_RActivateDepthBuffer (int);

void SCE_RSetCulledFaces (SCEenum);
void SCE_RSetValidPixels (SCEenum);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
