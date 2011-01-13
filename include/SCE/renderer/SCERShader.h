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
 
/* created: 11/02/2007
   updated: 13/01/2011 */

#ifndef SCERSHADER_H
#define SCERSHADER_H

#include "SCE/renderer/SCERType.h"

#ifdef __cplusplus
extern "C" {
#endif

/* constantes propres a CShader */
#define SCE_VERTEX_SHADER 1
#define SCE_PIXEL_SHADER 2

/* structure d'un shader GLSL */
typedef struct sce_rshaderglsl SCE_RShaderGLSL;
struct sce_rshaderglsl {
    SCEuint id;     /* identifiant opengl */
    SCEenum type;   /* type du shader (vertex ou fragment) */
    SCEchar *data;  /* donnes */
    int is_pixelshader;
    int compiled;
};

/* structure d'un programme regroupant des shaders GLSL */
typedef struct sce_rprogram SCE_RProgram;
struct sce_rprogram {
    SCEuint id;                 /**< OpenGL ID */
    int compiled;
};


int SCE_RShaderInit (void);
void SCE_RShaderQuit (void);

SCE_RShaderGLSL* SCE_RCreateShaderGLSL (SCEenum);

void SCE_RDeleteShaderGLSL (SCE_RShaderGLSL*);

void SCE_RSetShaderGLSLSource (SCE_RShaderGLSL*, char*);
int SCE_RSetShaderGLSLSourceDup (SCE_RShaderGLSL*, char*);

int SCE_RBuildShaderGLSL (SCE_RShaderGLSL*);

SCE_RProgram* SCE_RCreateProgram (void);
void SCE_RDeleteProgram (SCE_RProgram*);

int SCE_RSetProgramShader (SCE_RProgram*, SCE_RShaderGLSL*, int);

int SCE_RBuildProgram (SCE_RProgram*);

void SCE_RUseProgram (SCE_RProgram*);

#if 0
void SCE_RDisableShaderGLSL (void);
#endif

SCEint SCE_RGetProgramIndex (SCE_RProgram*, const char*);

SCEint SCE_RGetProgramAttribIndex (SCE_RProgram*, const char*);

void SCE_RSetProgramParam (SCEint, int);
void SCE_RSetProgramParamf (SCEint, float);
void SCE_RSetProgramParam1fv (SCEint, size_t, const float*);
void SCE_RSetProgramParam2fv (SCEint, size_t, const float*);
void SCE_RSetProgramParam3fv (SCEint, size_t, const float*);
void SCE_RSetProgramParam4fv (SCEint, size_t, const float*);
void SCE_RSetProgramMatrix2 (SCEint, size_t, const float*);
void SCE_RSetProgramMatrix3 (SCEint, size_t, const float*);
void SCE_RSetProgramMatrix4 (SCEint, size_t, const float*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
