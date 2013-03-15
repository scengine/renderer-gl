/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2013  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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
 
/* created: 29/07/2009
   updated: 15/03/2013 */

#ifndef SCERVERTEXBUFFER_H
#define SCERVERTEXBUFFER_H

#include <SCE/utils/SCEUtils.h>
#include "SCE/renderer/SCERBuffer.h"
#include "SCE/renderer/SCERVertexArray.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup vertexbuffer
 * @{
 */

/**
 * \brief Render modes for a vertex buffer
 */
enum sce_rbufferrendermode {
    SCE_VA_RENDER_MODE,  /**< Use simple vertex arrays */
    SCE_VBO_RENDER_MODE, /**< Use vertex buffer objects */
    SCE_VAO_RENDER_MODE  /**< Use one single vertex array object for the entire
                          * vertex buffer. It disables the ability of
                          * enable/disable vertex arrays of the vertex buffer
                          * (see SCE_REnableVertexBufferData()) but improves
                          * performances when rendering it. */
};
/** \copydoc sce_rbufferrendermode */
typedef enum sce_rbufferrendermode SCE_RBufferRenderMode;

/** \copydoc sce_rvertexbuffer  */
typedef struct sce_rvertexbuffer SCE_RVertexBuffer;

/** \copydoc sce_rvertexbufferdata */
typedef struct sce_rvertexbufferdata SCE_RVertexBufferData;
/**
 * \brief Data of a vertex buffer
 */
struct sce_rvertexbufferdata {
    SCE_RBufferData data;       /**< Buffer data */
    SCE_RVertexArraySequence seq; /**< Global setup sequence (VAO) */
    SCE_SList arrays;           /**< Vertex arrays (many means interleaved) */
    size_t stride;              /**< Final stride */
    SCE_SListIterator it;       /**< Used to store the structure into a vertex
                                 * buffer */
    SCE_RVertexBuffer *vb;      /**< The vertex buffer using this structure */
};

typedef void (*SCE_FUseVBFunc)(SCE_RVertexBuffer*);
/**
 * \brief A vertex buffer
 */
struct sce_rvertexbuffer {
    SCE_RVertexArraySequence seq; /**< Global setup sequence (VAO) */
    SCE_RBuffer buf;            /**< Core buffer */
    SCE_SList data;             /**< RVertexBufferData, memory managed by the
                                 * vertex buffer */
    SCE_FUseVBFunc use;         /**< Setup function */
    SCE_RBufferRenderMode rmode;/**< Render mode set when built */
    unsigned int n_vertices;    /**< Number of vertices in the vertex buffer */
};
/** \copydoc sce_rindexbuffer */
typedef struct sce_rindexbuffer SCE_RIndexBuffer;
/**
 * \brief An index buffer
 */
struct sce_rindexbuffer {
    SCE_RBuffer buf;            /**< Core buffer */
    SCE_RBufferData data;
    SCE_RIndexArray ia;
    unsigned int n_indices;     /**< Number of indices */
};

/** @} */

void SCE_RInitVertexBufferData (SCE_RVertexBufferData*);
SCE_RVertexBufferData* SCE_RCreateVertexBufferData (void);
void SCE_RClearVertexBufferData (SCE_RVertexBufferData*);
void SCE_RDeleteVertexBufferData (SCE_RVertexBufferData*);

void SCE_RInitVertexBuffer (SCE_RVertexBuffer*);
SCE_RVertexBuffer* SCE_RCreateVertexBuffer (void);
void SCE_RClearVertexBuffer (SCE_RVertexBuffer*);
void SCE_RDeleteVertexBuffer (SCE_RVertexBuffer*);

void SCE_RInitIndexBuffer (SCE_RIndexBuffer*);
SCE_RIndexBuffer* SCE_RCreateIndexBuffer (void);
void SCE_RClearIndexBuffer (SCE_RIndexBuffer*);
void SCE_RDeleteIndexBuffer (SCE_RIndexBuffer*);

int SCE_RAddVertexBufferDataArray (SCE_RVertexBufferData*,
                                   SCE_RVertexArray*, size_t);
void SCE_RDeleteVertexBufferDataArrays (SCE_RVertexBufferData*);
void SCE_RModifiedVertexBufferData (SCE_RVertexBufferData*, const size_t*);
#if 0
void SCE_REnableVertexBufferData (SCE_RVertexBufferData*);
void SCE_RDisableVertexBufferData (SCE_RVertexBufferData*);
#endif

SCE_RBuffer* SCE_RGetVertexBufferBuffer (SCE_RVertexBuffer*);
void SCE_RAddVertexBufferData (SCE_RVertexBuffer*, SCE_RVertexBufferData*);
void SCE_RRemoveVertexBufferData (SCE_RVertexBufferData*);
void SCE_RSetVertexBufferNumVertices (SCE_RVertexBuffer*, size_t);

void SCE_RInstantVertexBufferUpdate (SCE_RVertexBuffer*, const void*, size_t,
                                     size_t);
void SCE_RInstantIndexBufferUpdate (SCE_RIndexBuffer*, const void*, size_t,
                                    size_t);

size_t SCE_RGetVertexBufferUsedVRAM (const SCE_RVertexBuffer*);
size_t SCE_RGetIndexBufferUsedVRAM (const SCE_RIndexBuffer*);
size_t SCE_RGetVertexBufferSize (const SCE_RVertexBuffer*);
size_t SCE_RGetIndexBufferSize (const SCE_RIndexBuffer*);

void SCE_RBuildVertexBuffer (SCE_RVertexBuffer*, SCE_RBufferUsage,
                             SCE_RBufferRenderMode);
void SCE_RSetVertexBufferRenderMode (SCE_RVertexBuffer*, SCE_RBufferRenderMode);
int SCE_RIsVertexBufferBuilt (SCE_RVertexBuffer*);
void SCE_RUseVertexBuffer (SCE_RVertexBuffer*);
void SCE_RRenderVertexBuffer (SCE_EPrimitiveType);
void SCE_RRenderVertexBufferInstanced (SCE_EPrimitiveType, SCEuint);

void SCE_RModifiedIndexBuffer (SCE_RIndexBuffer*, const size_t*);
SCE_RBuffer* SCE_RGetIndexBufferBuffer (SCE_RIndexBuffer*);
void SCE_RSetIndexBufferIndexArray (SCE_RIndexBuffer*, SCE_RIndexArray*);
void SCE_RSetIndexBufferIndices (SCE_RIndexBuffer*, SCEenum, void*);
void SCE_RSetIndexBufferNumIndices (SCE_RIndexBuffer*, size_t);
void SCE_RBuildIndexBuffer (SCE_RIndexBuffer*, SCE_RBufferUsage);
void SCE_RUseIndexBuffer (SCE_RIndexBuffer*);
void SCE_RRenderVertexBufferIndexed (SCE_EPrimitiveType);
void SCE_RRenderVertexBufferIndexedInstanced (SCE_EPrimitiveType, SCEuint);

void SCE_RFinishVertexBufferRender (void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
