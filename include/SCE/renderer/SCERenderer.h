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
 
/* created: 15/12/2006
   updated: 03/04/2012 */

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
#include "SCE/renderer/SCERFeedback.h"
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

/* buffers */
#define SCE_COLOR_BUFFER_BIT    GL_COLOR_BUFFER_BIT
#define SCE_DEPTH_BUFFER_BIT    GL_DEPTH_BUFFER_BIT
#define SCE_STENCIL_BUFFER_BIT  GL_STENCIL_BUFFER_BIT

/* faces */
#define SCE_FRONT           GL_FRONT
#define SCE_BACK            GL_BACK
#define SCE_FRONT_AND_BACK  GL_FRONT_AND_BACK

/* comparaisons */
#define SCE_LESS            GL_LESS
#define SCE_LEQUAL          GL_LEQUAL
#define SCE_GREATER         GL_GREATER
#define SCE_GEQUAL          GL_GEQUAL
#define SCE_NOTEQUAL        GL_NOTEQUAL
/* stencil stuff */
#define SCE_KEEP            GL_KEEP
#define SCE_ZERO            GL_ZERO
#define SCE_REPLACE         GL_REPLACE
#define SCE_INCR            GL_INCR
#define SCE_INCR_WRAP       GL_INCR_WRAP
#define SCE_DECR            GL_DECR
#define SCE_DECR_WRAP       GL_DECR_WRAP
#define SCE_INVERT          GL_INVERT

int SCE_RInit (FILE*, SCEbitfield);
void SCE_RQuit (void);

const char* SCE_RGetError (void);

void SCE_RClearColor (float, float, float, float);
void SCE_RClearColori (int, int, int, int);
void SCE_RClearColorui (SCEuint, SCEuint, SCEuint, SCEuint);
void SCE_RClearDepth (float);
void SCE_RClearStencil (SCEuint);
void SCE_RClear (const SCEbitfield);
void SCE_RFlush (void);

void SCE_RSetState (SCEenum, int);
void SCE_RSetState2 (SCEenum, SCEenum, int);
void SCE_RSetState3 (SCEenum, SCEenum, SCEenum, int);
void SCE_RSetState4 (SCEenum, SCEenum, SCEenum, SCEenum, int);
void SCE_RSetState5 (SCEenum, SCEenum, SCEenum, SCEenum, SCEenum, int);
void SCE_RSetState6 (SCEenum, SCEenum, SCEenum, SCEenum, SCEenum, SCEenum, int);

void SCE_RSetBlending (SCEenum, SCEenum);
void SCE_RSetAlphaFunc (SCEenum, float);

void SCE_REnableDepthTest (void);
void SCE_RDisableDepthTest (void);
void SCE_REnableStencilTest (void);
void SCE_RDisableStencilTest (void);

void SCE_RActivateColorBuffer (int);
void SCE_RActivateDepthBuffer (int);
void SCE_RDepthRange (SCEdouble, SCEdouble);

void SCE_RSetCulledFaces (SCEenum);
void SCE_RSetValidPixels (SCEenum);

void SCE_RSetStencilOp (SCEenum, SCEenum, SCEenum);
void SCE_RSetStencilFunc (SCEenum, SCEuint, SCEuint);

void SCE_RVertexAttrib4fv (SCEuint, const SCEfloat*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
