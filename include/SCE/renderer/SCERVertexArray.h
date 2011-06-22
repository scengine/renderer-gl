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
 
/* created: 26/07/2009
   updated: 22/06/2011 */

#ifndef SCERVERTEXARRAY_H
#define SCERVERTEXARRAY_H

#include <SCE/utils/SCEUtils.h>
#include <SCE/core/SCECore.h>   /* SCEGeometry */
#include "SCE/renderer/SCERType.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup vertexarray
 * @{
 */

typedef void (*SCE_FSetVA)(SCE_SGeometryArrayData*);

/** \copydoc sce_rvertexarray */
typedef struct sce_rvertexarray SCE_RVertexArray;
/**
 * \brief A vertex array
 */
struct sce_rvertexarray {
    SCE_FSetVA set, unset;      /**< Functions to set/unset the vertex array */
    SCE_SGeometryArrayData data;
    SCE_SListIterator it;       /**< Own iterator */
};

typedef struct sce_rindexarray SCE_RIndexArray;
/**
 * \brief An index array
 */
struct sce_rindexarray {
    SCEenum type;
    void *data;
};

/** \copydoc sce_rvertexarraysequence */
typedef struct sce_rvertexarraysequence SCE_RVertexArraySequence;
/**
 * \brief Vertex array setup sequence using GL vertex array objects
 */
struct sce_rvertexarraysequence {
    SCEuint id;                 /**< Teh GL ID */
};

#define SCE_NUM_VERTEX_ATTRIBUTES_MAPPINGS 16
typedef SCEuint SCE_RVertexAttributesMap[SCE_NUM_VERTEX_ATTRIBUTES_MAPPINGS];

/** @} */

int SCE_RVertexArrayInit (void);
void SCE_RVertexArrayQuit (void);

void SCE_RInitVertexArray (SCE_RVertexArray*);
SCE_RVertexArray* SCE_RCreateVertexArray (void);
void SCE_RClearVertexArray (SCE_RVertexArray*);
void SCE_RDeleteVertexArray (SCE_RVertexArray*);

void SCE_RInitIndexArray (SCE_RIndexArray*);
SCE_RIndexArray* SCE_RCreateIndexArray (void);
void SCE_RDeleteIndexArray (SCE_RIndexArray*);

void SCE_RInitVertexArraySequence (SCE_RVertexArraySequence*);

void SCE_RInitVertexAttributesMap (SCE_RVertexAttributesMap);

SCE_SGeometryArrayData* SCE_RGetVertexArrayData (SCE_RVertexArray*);
void SCE_RSetVertexArrayData (SCE_RVertexArray*, SCE_SGeometryArrayData*);
void SCE_RSetVertexArrayNewData (SCE_RVertexArray*, SCE_EVertexAttribute,
                                 SCEenum, SCEsizei, SCEint, void*);

void SCE_RUseVertexAttributesMap (SCE_RVertexAttributesMap);
void SCE_RDisableVertexAttributesMap (void);

extern void (*SCE_RUseVertexArray) (SCE_RVertexArray*);
void SCE_RRender (SCE_EPrimitiveType, SCEuint);
void SCE_RRenderInstanced (SCE_EPrimitiveType, SCEuint, SCEuint);
void SCE_RRenderIndexed (SCE_EPrimitiveType, SCE_RIndexArray*, SCEuint);
void SCE_RRenderIndexedInstanced (SCE_EPrimitiveType, SCE_RIndexArray*,
                                  SCEuint, SCEuint);
void SCE_RFinishVertexArrayRender (void);

/* bonus API for GL VAO */
void SCE_RBeginVertexArraySequence (SCE_RVertexArraySequence*);
void SCE_RCallVertexArraySequence (SCE_RVertexArraySequence);
void SCE_REndVertexArraySequence (void);
void SCE_RDeleteVertexArraySequence (SCE_RVertexArraySequence*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
