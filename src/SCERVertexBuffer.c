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
 
/* created: 29/07/2009
   updated: 29/01/2012 */

#include <GL/glew.h>
#include "SCE/renderer/SCERType.h"
#include "SCE/renderer/SCERVertexBuffer.h"

#define SCE_BUFFER_OFFSET(p) ((char*)NULL + (p))

static SCE_RVertexBuffer *vb_bound = NULL;
static SCE_RIndexBuffer *ib_bound = NULL;

void SCE_RInitVertexBufferData (SCE_RVertexBufferData *data)
{
    SCE_RInitBufferData (&data->data);
    SCE_RInitVertexArraySequence (&data->seq);
    SCE_List_Init (&data->arrays);
    SCE_List_CanDeleteIterators (&data->arrays, SCE_TRUE);
    data->stride = 0;
    SCE_List_InitIt (&data->it);
    SCE_List_SetData (&data->it, data);
    data->vb = NULL;
}
SCE_RVertexBufferData* SCE_RCreateVertexBufferData (void)
{
    SCE_RVertexBufferData *data = NULL;
    if (!(data = SCE_malloc (sizeof *data)))
        SCEE_LogSrc ();
    else
        SCE_RInitVertexBufferData (data);
    return data;
}
void SCE_RClearVertexBufferData (SCE_RVertexBufferData *data)
{
    SCE_RRemoveVertexBufferData (data);
    SCE_RDeleteVertexArraySequence (&data->seq);
    SCE_List_Clear (&data->arrays);
    SCE_RClearBufferData (&data->data);
}
void SCE_RDeleteVertexBufferData (SCE_RVertexBufferData *data)
{
    if (data) {
        SCE_RClearVertexBufferData (data);
        SCE_free (data);
    }
}

static void SCE_RFreeVertexBufferData (void *vbd)
{
    /* not useless: CRemove set the vb pointer of vbd to NULL */
    SCE_RRemoveVertexBufferData (vbd);
}
void SCE_RInitVertexBuffer (SCE_RVertexBuffer *vb)
{
    SCE_RInitVertexArraySequence (&vb->seq);
    SCE_RInitBuffer (&vb->buf);
    SCE_List_Init (&vb->data);
    SCE_List_SetFreeFunc (&vb->data, SCE_RFreeVertexBufferData);
    vb->use = NULL;
    vb->rmode = SCE_VA_RENDER_MODE;
    vb->n_vertices = 0;
}
SCE_RVertexBuffer* SCE_RCreateVertexBuffer (void)
{
    SCE_RVertexBuffer *vb = NULL;
    if (!(vb = SCE_malloc (sizeof *vb)))
        SCEE_LogSrc ();
    else
        SCE_RInitVertexBuffer (vb);
    return vb;
}
void SCE_RClearVertexBuffer (SCE_RVertexBuffer *vb)
{
    SCE_List_Clear (&vb->data);
    SCE_RClearBuffer (&vb->buf);
    SCE_RDeleteVertexArraySequence (&vb->seq);
}
void SCE_RDeleteVertexBuffer (SCE_RVertexBuffer *vb)
{
    if (vb) {
        SCE_RClearVertexBuffer (vb);
        SCE_free (vb);
    }
}

void SCE_RInitIndexBuffer (SCE_RIndexBuffer *ib)
{
    SCE_RInitBuffer (&ib->buf);
    SCE_RInitBufferData (&ib->data);
    SCE_RInitIndexArray (&ib->ia);
    ib->ia.data = SCE_BUFFER_OFFSET (0);
    ib->n_indices = 0;
}
SCE_RIndexBuffer* SCE_RCreateIndexBuffer (void)
{
    SCE_RIndexBuffer *ib = NULL;
    if (!(ib = SCE_malloc (sizeof *ib)))
        SCEE_LogSrc ();
    else
        SCE_RInitIndexBuffer (ib);
    return ib;
}
void SCE_RClearIndexBuffer (SCE_RIndexBuffer *ib)
{
    SCE_RClearBufferData (&ib->data);
    SCE_RClearBuffer (&ib->buf);
}
void SCE_RDeleteIndexBuffer (SCE_RIndexBuffer *ib)
{
    if (ib) {
        SCE_RClearIndexBuffer (ib);
        SCE_free (ib);
    }
}


/**
 * \brief Adds a vertex array to a vertex buffer data
 *
 * The structure \p data is given to the vertex array of \p vbd
 * calling SCE_RSetVertexArrayData().
 * \sa SCE_RSetVertexArrayData(), SCE_RDeleteVertexBufferDataArrays()
 * SCE_RAddVertexBufferData
 */
int SCE_RAddVertexBufferDataArray (SCE_RVertexBufferData *vbd,
                                   SCE_RVertexArray *va,
                                   size_t n_vertices)
{
    SCE_SGeometryArrayData *data;
    if (SCE_List_AppendNewl (&vbd->arrays, va) < 0) {
        SCEE_LogSrc ();
        return SCE_ERROR;
    }
    /* get main pointer of the interleaved array */
    data = SCE_RGetVertexArrayData (va);
    if (!vbd->data.data)
        vbd->data.data = data->data;
    else {
        vbd->data.data = (data->data < vbd->data.data ?
                          data->data : vbd->data.data);
    }
    if (!data->data)
        data->data = (void*)vbd->stride;
    if (!data->stride)
        vbd->stride += SCE_Type_Sizeof (data->type) * data->size;
    else
        vbd->stride += data->stride;

    vbd->data.size = vbd->stride * n_vertices;
    return SCE_OK;
}
static void SCE_RFreeDataArray (void *a)
{
    SCE_RDeleteVertexArray (a);
}
/**
 * \brief Deletes the arrays of a vertex buffer data
 * \sa SCE_RAddVertexBufferDataArray()
 */
void SCE_RDeleteVertexBufferDataArrays (SCE_RVertexBufferData *vbd)
{
    SCE_List_SetFreeFunc (&vbd->arrays, SCE_RFreeDataArray);
    SCE_List_Clear (&vbd->arrays);
    SCE_List_SetFreeFunc (&vbd->arrays, NULL);
}

/**
 * \brief Set modified vertices range
 * \param range range of modified vertices, [0] is the first modified vertex
 * and [1] the number of modified vertices, if NULL the whole buffer data will
 * be updated.
 * \note the vertex buffer of \p vbd must be built before calling this function
 * \sa SCE_RModifiedBufferData()
 */
void SCE_RModifiedVertexBufferData (SCE_RVertexBufferData *vbd,
                                    const size_t *range)
{
    if (!range)
        SCE_RModifiedBufferData (&vbd->data, NULL);
    else {
        /* consider all arrays */
        size_t r[2];
        r[0] = range[0] * vbd->stride;
        r[1] = range[1] * vbd->stride;
        SCE_RModifiedBufferData (&vbd->data, r);
    }
}
#if 0
/**
 * \brief Enables the given vertex buffer data for the render
 * \sa SCE_RSetVertexBufferRenderMode()
 */
void SCE_REnableVertexBufferData (SCE_RVertexBufferData *vbd)
{
    SCE_List_Remove (&vbd->it);
    SCE_List_Appendl (&vbd->vb->data, &vbd->it);
}
/**
 * \brief Disables the given vertex buffer data for the render
 * \sa SCE_RSetVertexBufferRenderMode()
 */
void SCE_RDisableVertexBufferData (SCE_RVertexBufferData *vbd)
{
    SCE_List_Remove (&vbd->it);
}
#endif


/**
 * \brief Gets the buffer of a vertex buffer
 */
SCE_RBuffer* SCE_RGetVertexBufferBuffer (SCE_RVertexBuffer *vb)
{
    return &vb->buf;
}

/**
 * \brief Adds data to a vertex buffer
 *
 * The memory of \p d is never freed by module CVertexBuffer.
 * \sa SCE_RSetVertexBufferDataArrayData(), SCE_RAddBufferData()
 */
void SCE_RAddVertexBufferData (SCE_RVertexBuffer *vb, SCE_RVertexBufferData *d)
{
    SCE_RAddBufferData (&vb->buf, &d->data);
    SCE_List_Appendl (&vb->data, &d->it);
    d->vb = vb;
}
/**
 * \brief Removes a vertex buffer data from its buffer
 * \sa SCE_RAddVertexBufferData(), SCE_RClearVertexBufferData(),
 * SCE_RRemoveBufferData()
 */
void SCE_RRemoveVertexBufferData (SCE_RVertexBufferData *data)
{
    if (data->vb) {
        SCE_RRemoveBufferData (&data->data);
        SCE_List_Remove (&data->it);
        data->vb = NULL;
    }
}

/**
 * \brief Sets the number of vertices of a vertex buffer
 */
void SCE_RSetVertexBufferNumVertices (SCE_RVertexBuffer *vb, size_t n)
{
    vb->n_vertices = n;
}

/** \deprecated */
static void SCE_RUseVAMode (SCE_RVertexBuffer *vb)
{
    SCE_SListIterator *it = NULL;
    SCE_List_ForEach (it, &vb->data) {
        SCE_SListIterator *it2;
        SCE_RVertexBufferData *data = SCE_List_GetData (it);
        SCE_List_ForEach (it2, &data->arrays)
            SCE_RUseVertexArray (SCE_List_GetData (it2));
    }
}
static void SCE_RUseVBOMode (SCE_RVertexBuffer *vb)
{
    SCE_SListIterator *it = NULL;
    glBindBuffer (GL_ARRAY_BUFFER, vb->buf.id);
    SCE_List_ForEach (it, &vb->data) {
        SCE_SListIterator *it2;
        SCE_RVertexBufferData *data = SCE_List_GetData (it);
        SCE_List_ForEach (it2, &data->arrays)
            SCE_RUseVertexArray (SCE_List_GetData (it2));
    }
}
static void SCE_RUseVAOMode (SCE_RVertexBuffer *vb)
{
    SCE_RCallVertexArraySequence (vb->seq);
}
/**
 * \brief Builds a vertex buffer
 * \param usage GL usage of the buffer
 * \param mode rendering method to use with the vertex buffer
 *
 * If \p mode is SCE_UNIFIED_VBO_RENDER_MODE, you may call
 * SCE_RUseVertexBuffer()/SCE_RUseIndexBuffer() for each vertex/index buffer
 * you want to link to \p vb and terminate with SCE_REndVertexArraySequence().
 * Then using \p vb will do the same as using one by one each vertex buffer you
 * specified.
 * \sa SCE_RSetVertexBufferRenderMode(), SCE_RBufferRenderMode
 */
void SCE_RBuildVertexBuffer (SCE_RVertexBuffer *vb, SCE_RBufferUsage usage,
                             SCE_RBufferRenderMode mode)
{
    SCE_RVertexBufferData *data = NULL;

    vb->rmode = mode;
    if (usage == SCE_BUFFER_DEFAULT_USAGE)
        usage = SCE_BUFFER_STREAM_DRAW;
    if (mode >= SCE_VBO_RENDER_MODE)
        SCE_RBuildBuffer (&vb->buf, GL_ARRAY_BUFFER, usage);

    SCE_RSetVertexBufferRenderMode (vb, mode);
    if (mode == SCE_VAO_RENDER_MODE) {
        SCE_RBeginVertexArraySequence (&vb->seq);
        SCE_RUseVBOMode (vb);
        SCE_REndVertexArraySequence ();
    }
}

/**
 * \brief Sets up the given render mode
 * \param mode desired render mode
 *
 * Note that if you wish a VAO mode, it has to be specified to
 * SCE_RBuildVertexBuffer().
 * \sa SCE_RBuildVertexBuffer(), SCE_RBufferRenderMode
 */
void SCE_RSetVertexBufferRenderMode (SCE_RVertexBuffer *vb,
                                     SCE_RBufferRenderMode mode)
{
    SCE_FUseVBFunc fun = NULL;
    SCE_SListIterator *it = NULL;

    switch (mode) {
    case SCE_VA_RENDER_MODE:
        fun = SCE_RUseVAMode;
        if (vb->use != fun && vb->use) {
            SCE_List_ForEach (it, &vb->data) {
                SCE_SListIterator *it2;
                SCE_RVertexBufferData *vbd = SCE_List_GetData (it);
                SCE_List_ForEach (it2, &vbd->arrays) {
                    SCE_SGeometryArrayData *data;
                    data = SCE_RGetVertexArrayData (SCE_List_GetData (it2));
#if 1
                    data->data = &((char*)vbd->data.data)[(size_t)data->data];
#else
                    /* TODO: WTF?? */
                    data->data = (char*)vbd->data.data + (char*)data->data;
#endif
                    data->stride = vbd->stride;
                }
            }
        }
        break;
    case SCE_VBO_RENDER_MODE:
        fun = SCE_RUseVBOMode;
    case SCE_VAO_RENDER_MODE:
        if (!fun)
            fun = SCE_RUseVAOMode;
        SCE_List_ForEach (it, &vb->data) {
            SCE_SListIterator *it2;
            SCE_SGeometryArrayData *data = NULL;
            SCE_RVertexBufferData *vbd = SCE_List_GetData (it);
            SCE_List_ForEach (it2, &vbd->arrays) {
                data = SCE_RGetVertexArrayData (SCE_List_GetData (it2));
                data->data = SCE_BUFFER_OFFSET (vbd->data.first
                                                + (char*)data->data
                                                - (char*)vbd->data.data);
                data->stride = vbd->stride;
            }
        }
        break;
    default:
#ifdef SCE_DEBUG
        SCEE_SendMsg ("unknown buffer render mode %d\n", mode);
#endif
    }
    if (fun)
        vb->use = fun;
}

/**
 * \brief Indicates if a vertex buffer is built
 */
int SCE_RIsVertexBufferBuilt (SCE_RVertexBuffer *vb)
{
    return (vb->use ? SCE_TRUE : SCE_FALSE);
}

/**
 * \brief 
 */
void SCE_RUseVertexBuffer (SCE_RVertexBuffer *vb)
{
    vb->use (vb);
    vb_bound = vb;
}


/**
 * \brief 
 */
void SCE_RRenderVertexBuffer (SCE_EPrimitiveType prim)
{
    glDrawArrays (sce_rprimtypes[prim], 0, vb_bound->n_vertices);
}
/**
 * \brief 
 */
void SCE_RRenderVertexBufferInstanced (SCE_EPrimitiveType prim, SCEuint num)
{
    glDrawArraysInstanced (sce_rprimtypes[prim], 0, vb_bound->n_vertices, num);
}


/**
 * \brief Sets the modified range of an index buffer
 * \param range range of modified indices
 * \sa SCE_RModifiedVertexBufferData(), SCE_RModifiedBuffer()
 */
void SCE_RModifiedIndexBuffer (SCE_RIndexBuffer *ib, const size_t *range)
{
    if (!range)
        SCE_RModifiedBuffer (&ib->buf, NULL);
    else {
        size_t r[2];
        size_t size = SCE_Type_Sizeof (ib->ia.type);
        r[0] = range[0] * size;
        r[1] = range[1] * size;
        SCE_RModifiedBuffer (&ib->buf, r);
    }
}
/**
 * \brief Gets the core buffer of an index buffer
 */
SCE_RBuffer* SCE_RGetIndexBufferBuffer (SCE_RIndexBuffer *ib)
{
    return &ib->buf;
}

/**
 * \brief Sets the array data of an index buffer
 *
 * The structure \p ia is copied into \p ib so don't worry about memory
 * management, \p ia can be a static structure.
 * \sa SCE_RSetIndexBufferIndices(), SCE_RSetIndexBufferNumIndices()
 */
void SCE_RSetIndexBufferIndexArray (SCE_RIndexBuffer *ib, SCE_RIndexArray *ia)
{
    ib->data.size = ib->n_indices * SCE_Type_Sizeof (ia->type);
    ib->data.data = ia->data;
    ib->ia.type = ia->type;
    /* don't set ib->ia.data, coz it's just an offset */
}
/**
 * \brief Sets the index array of an index buffer
 *
 * \p indices will never be freed by the vertex buffer module.
 * \sa SCE_RSetIndexBufferIndexArray(), SCE_RSetIndexBufferNumIndices()
 */
void SCE_RSetIndexBufferIndices (SCE_RIndexBuffer *ib, SCEenum type,
                                 void *indices)
{
    ib->data.size = ib->n_indices * SCE_Type_Sizeof (type);
    ib->data.data = indices;
    ib->ia.type = type;
    /* don't set ib->ia.data, coz it's just an offset */
}
/**
 * \brief Sets the number of indices of an index buffer
 * \sa SCE_RSetIndexBufferIndices()
 */
void SCE_RSetIndexBufferNumIndices (SCE_RIndexBuffer *ib, size_t n_indices)
{
    ib->data.size = n_indices * SCE_Type_Sizeof (ib->ia.type);
    ib->n_indices = n_indices;
}

/**
 * \brief 
 */
void SCE_RBuildIndexBuffer (SCE_RIndexBuffer *ib, SCE_RBufferUsage usage)
{
    if (usage == SCE_BUFFER_DEFAULT_USAGE)
        usage = SCE_BUFFER_STATIC_DRAW;
    SCE_RAddBufferData (&ib->buf, &ib->data);
    SCE_RBuildBuffer (&ib->buf, GL_ELEMENT_ARRAY_BUFFER, usage);
}

/**
 * \brief 
 */
void SCE_RUseIndexBuffer (SCE_RIndexBuffer *ib)
{
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ib->buf.id);
    ib_bound = ib;
}

/**
 * \brief 
 */
void SCE_RRenderVertexBufferIndexed (SCE_EPrimitiveType prim)
{
    SCE_RRenderIndexed (prim, &ib_bound->ia, ib_bound->n_indices);
}
/**
 * \brief 
 */
void SCE_RRenderVertexBufferIndexedInstanced (SCE_EPrimitiveType prim,
                                              SCEuint num)
{
    SCE_RRenderIndexedInstanced (prim, &ib_bound->ia, ib_bound->n_indices, num);
}


/**
 * \brief Deactivate rendering states setup by SCE_RUseVertexBuffer() and
 * SCE_RUseIndexBuffer()
 * \note Useless in a pure GL 3 context.. and for Unbind index buffer?
 */
void SCE_RFinishVertexBufferRender (void)
{
    SCE_RFinishVertexArrayRender ();
    if (vb_bound->rmode == SCE_VBO_RENDER_MODE ||
        vb_bound->rmode == SCE_VAO_RENDER_MODE) {
        glBindBuffer (GL_ARRAY_BUFFER, 0);
        /* otherwise there is no vertex buffer object or the VAO already
           deactivated the vertex buffer */
    }
    if (ib_bound) { /* if NULL, no index buffer */
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
        ib_bound = NULL;
    }
    vb_bound = NULL;
}
