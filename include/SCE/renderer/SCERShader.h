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
   updated: 01/08/2011 */

#ifndef SCERSHADER_H
#define SCERSHADER_H

#include <SCE/core/SCECore.h>  /* SCE_EPrimitiveType */
#include "SCE/renderer/SCERType.h"
#include "SCE/renderer/SCERVertexArray.h" /* SCE_RVertexAttributesMap */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Available shader types.
 */
typedef enum {
    SCE_VERTEX_SHADER,
    SCE_PIXEL_SHADER,
    SCE_GEOMETRY_SHADER,
    SCE_TESS_CONTROL_SHADER,
    SCE_TESS_EVALUATION_SHADER,
    SCE_NUM_SHADER_TYPES
} SCE_RShaderType;


#define SCE_POSITION_ATTRIB_NAME "sce_position"
#define SCE_COLOR_ATTRIB_NAME "sce_color"
#define SCE_NORMAL_ATTRIB_NAME "sce_normal"
#define SCE_TANGENT_ATTRIB_NAME "sce_tangent"
#define SCE_BINORMAL_ATTRIB_NAME "sce_binormal"
#define SCE_TEXCOORD0_ATTRIB_NAME "sce_texcoord0"
#define SCE_TEXCOORD1_ATTRIB_NAME "sce_texcoord1"
#define SCE_TEXCOORD2_ATTRIB_NAME "sce_texcoord2"
#define SCE_TEXCOORD3_ATTRIB_NAME "sce_texcoord3"
#define SCE_TEXCOORD4_ATTRIB_NAME "sce_texcoord4"
#define SCE_TEXCOORD5_ATTRIB_NAME "sce_texcoord5"
#define SCE_TEXCOORD6_ATTRIB_NAME "sce_texcoord6"
#define SCE_TEXCOORD7_ATTRIB_NAME "sce_texcoord7"

#define SCE_MAT_OBJECT_NAME "sce_objectmatrix"
#define SCE_MAT_CAMERA_NAME "sce_cameramatrix"
#define SCE_MAT_PROJECTION_NAME "sce_projectionmatrix"
#define SCE_MAT_TEXTURE_NAME "sce_texturematrix"
#define SCE_MAT_MODELVIEW_NAME "sce_modelviewmatrix"


/**
 * \brief GL shader
 */
typedef struct sce_rshaderglsl SCE_RShaderGLSL;
struct sce_rshaderglsl {
    SCEuint id;                 /**< OpenGL identifier */
    SCE_RShaderType type;       /**< Type */
    SCEenum gltype;             /**< OpenGL type constant */
    SCEchar *data;              /**< Source code */
    int compiled;               /**< Is the shader compiled? */
};

/**
 * \brief GL program
 */
typedef struct sce_rprogram SCE_RProgram;
struct sce_rprogram {
    SCEuint id;                   /**< OpenGL identifier */
    int linked;                   /**< Is the program linked? */
    SCE_RVertexAttributesMap map; /**< Vertex attributes mappings */
    int map_built;                /**< Is the attributes map built? */
    int use_vmap;                 /**< Use vertex attributes mapping? */
    int mat_map[SCE_NUM_MATRICES]; /**< Uniform locations map */
    /** Function that maps the matrices */
    SCE_RSetMatrixFunc funs[SCE_NUM_MATRICES];
    int use_mmap;                 /**< Use matrices mapping? */
    int use_tess;
    int patch_vertices;           /**< Tessellation patch vertices */
};


int SCE_RShaderInit (void);
void SCE_RShaderQuit (void);

SCE_RShaderGLSL* SCE_RCreateShaderGLSL (SCE_RShaderType);

void SCE_RDeleteShaderGLSL (SCE_RShaderGLSL*);

void SCE_RSetShaderGLSLSource (SCE_RShaderGLSL*, char*);

int SCE_RBuildShaderGLSL (SCE_RShaderGLSL*);

SCE_RProgram* SCE_RCreateProgram (void);
void SCE_RDeleteProgram (SCE_RProgram*);

int SCE_RSetProgramShader (SCE_RProgram*, SCE_RShaderGLSL*, int);

int SCE_RBuildProgram (SCE_RProgram*);
int SCE_RValidateProgram (SCE_RProgram*);

void SCE_RSetupProgramAttributesMapping (SCE_RProgram*);
void SCE_RActivateProgramAttributesMapping (SCE_RProgram*, int);

void SCE_RSetupProgramMatricesMapping (SCE_RProgram*);
void SCE_RActivateProgramMatricesMapping (SCE_RProgram*, int);

void SCE_RSetProgramPatchVertices (SCE_RProgram*, int);

void SCE_RUseProgram (SCE_RProgram*);

#if 0
void SCE_RDisableShaderGLSL (void);
#endif

int SCE_RSetProgramInputPrimitive (SCE_RProgram*, SCE_EPrimitiveType, int);
int SCE_RSetProgramOutputPrimitive (SCE_RProgram*, SCE_EPrimitiveType);

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
