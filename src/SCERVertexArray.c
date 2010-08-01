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
 
/* created: 26/07/2009
   updated: 21/08/2009 */

#include <GL/glew.h>
#include "SCE/renderer/SCERType.h"
#include "SCE/renderer/SCERTexture.h" /* CGetMaxTextureUnits() */
#include "SCE/renderer/SCERVertexArray.h"

/**
 * \file SCECVertexArray.c
 * \copydoc
 * \file SCECVertexArray.h
 * \copydoc
 */

/**
 * \defgroup vertexarray GL vertex arrays
 * \ingroup core
 * \internal
 * \brief OpenGL vertex arrays
 * @{
 */

static SCE_SList vaused;
static int vao_used = SCE_FALSE;

int SCE_RVertexArrayInit (void)
{
    SCE_List_Init (&vaused);
    return SCE_OK;
}
void SCE_RVertexArrayQuit (void)
{
    SCE_List_Flush (&vaused);
}

void SCE_RInitVertexArrayData (SCE_SGeometryArrayData *data)
{
    data->attrib = SCE_POSITION;
    data->type = SCE_FLOAT;
    data->stride = 0;
    data->size = 3;
    data->data = NULL;
}
SCE_SGeometryArrayData* SCE_RCreateVertexArrayData (void)
{
    SCE_SGeometryArrayData *data = NULL;
    if (!(data = SCE_malloc (sizeof *data)))
        SCEE_LogSrc ();
    else
        SCE_RInitVertexArrayData (data);
    return data;
}
void SCE_RDeleteVertexArrayData (SCE_SGeometryArrayData *data)
{
    if (data) {
        SCE_free (data);
    }
}

/* TODO: These functions are no longer in GL 3.1 */
static void SCE_RSetVAPos (SCE_SGeometryArrayData *data)
{
    glEnableClientState (GL_VERTEX_ARRAY);
    glVertexPointer (data->size, sce_rgltypes[data->type], data->stride,
                     data->data);
}
static void SCE_RUnsetVAPos (SCE_SGeometryArrayData *data)
{
    glDisableClientState (GL_VERTEX_ARRAY);
}
static void SCE_RSetVANor (SCE_SGeometryArrayData *data)
{
    glEnableClientState (GL_NORMAL_ARRAY);
    glNormalPointer (sce_rgltypes[data->type], data->stride, data->data);
}
static void SCE_RUnsetVANor (SCE_SGeometryArrayData *data)
{
    glDisableClientState (GL_NORMAL_ARRAY);
}
static void SCE_RSetVACol (SCE_SGeometryArrayData *data)
{
    glEnableClientState (GL_COLOR_ARRAY);
    glColorPointer (data->size, sce_rgltypes[data->type], data->stride,
                    data->data);
}
static void SCE_RUnsetVACol (SCE_SGeometryArrayData *data)
{
    glDisableClientState (GL_COLOR_ARRAY);
}
static void SCE_RSetVATex (SCE_SGeometryArrayData *data)
{
    glClientActiveTexture (GL_TEXTURE0 + data->attrib - SCE_TEXCOORD0);
    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer (data->size, sce_rgltypes[data->type],
                       data->stride, data->data);
}
static void SCE_RUnsetVATex (SCE_SGeometryArrayData *data)
{
    glClientActiveTexture (GL_TEXTURE0 + data->attrib - SCE_TEXCOORD0);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
}
/****/
static void SCE_RSetVAAtt (SCE_SGeometryArrayData *data)
{
    /* hope that data->attrib isn't too large */
    int attrib = data->attrib - SCE_ATTRIB0;
    glEnableVertexAttribArray (attrib);
    glVertexAttribPointer (attrib, data->size, sce_rgltypes[data->type], 0,
                           data->stride, data->data);
}
static void SCE_RUnsetVAAtt (SCE_SGeometryArrayData *data)
{
    glDisableVertexAttribArray (data->attrib - SCE_ATTRIB0);
}

/**
 * \brief
 */
void SCE_RInitVertexArray (SCE_RVertexArray *va)
{
    va->set = SCE_RSetVAPos;
    va->unset = SCE_RUnsetVAPos;
    SCE_RInitVertexArrayData (&va->data);
    SCE_List_InitIt (&va->it);
    SCE_List_SetData (&va->it, va);
}
/**
 * \brief
 */
SCE_RVertexArray* SCE_RCreateVertexArray (void)
{
    SCE_RVertexArray *va = NULL;
    if (!(va = SCE_malloc (sizeof *va)))
        SCEE_LogSrc ();
    else
        SCE_RInitVertexArray (va);
    return va;
}
void SCE_RClearVertexArray (SCE_RVertexArray *va)
{
    (void)va;                   /* ptdr. */
    /* NOTE: cannot remove the iterator safely, so NEVER clear a vertex array
       on rendering stage */
}
/**
 * \brief
 */
void SCE_RDeleteVertexArray (SCE_RVertexArray *va)
{
    if (va) {
        SCE_RClearVertexArray (va);
        SCE_free (va);
    }
}

/**
 * \brief
 */
void SCE_RInitIndexArray (SCE_RIndexArray *ia)
{
    ia->type = SCE_UNSIGNED_BYTE;
    ia->data = NULL;
}
/**
 * \brief
 */
SCE_RIndexArray* SCE_RCreateIndexArray (void)
{
    SCE_RIndexArray *ia = NULL;
    if (!(ia = SCE_malloc (sizeof *ia)))
        SCEE_LogSrc ();
    else
        SCE_RInitIndexArray (ia);
    return ia;
}
/**
 * \brief
 */
void SCE_RDeleteIndexArray (SCE_RIndexArray *ia)
{
    if (ia) {
        SCE_free (ia);
    }
}

void SCE_RInitVertexArraySequence (SCE_RVertexArraySequence *seq)
{
    seq->id = 0;
}

/**
 * \brief Gets \p &va->data
 * \sa SCE_RSetVertexArrayData(), SCE_RSetVertexArrayNewData()
 */
SCE_SGeometryArrayData* SCE_RGetVertexArrayData (SCE_RVertexArray *va)
{
    return &va->data;
}
/**
 * \brief Defines data for a vertex array
 *
 * Copies \p data into \p va->data, so \p data can be a static structure.
 * \sa SCE_RSetVertexArrayNewData(), SCE_RGetVertexArrayData()
 */
void SCE_RSetVertexArrayData (SCE_RVertexArray *va, SCE_SGeometryArrayData *data)
{
    va->data = *data;
    switch (data->attrib) {
    case SCE_POSITION:
        va->set = SCE_RSetVAPos;
        va->unset = SCE_RUnsetVAPos;
        break;
    case SCE_NORMAL:
        va->set = SCE_RSetVANor;
        va->unset = SCE_RUnsetVANor;
        break;
    case SCE_ROLOR:
        va->set = SCE_RSetVACol;
        va->unset = SCE_RUnsetVACol;
        break;
    default:
        if (data->attrib >= SCE_TEXCOORD0 &&
            data->attrib <= SCE_RGetMaxTextureUnits () + SCE_TEXCOORD0) {
            va->set = SCE_RSetVATex;
            va->unset = SCE_RUnsetVATex;
        } else if (data->attrib >= SCE_ATTRIB0) {
            va->set = SCE_RSetVAAtt;
            va->unset = SCE_RUnsetVAAtt;
        }
#ifdef SCE_DEBUG
        else {
            SCEE_SendMsg ("Unknown attrib type %d", data->attrib);
        }
#endif
    }
}
/**
 * \brief Like SCE_RSetVertexArrayData() but creates a static
 * SCE_SGeometryArrayData structure based on given parameters
 * \sa SCE_RSetVertexArrayData(), SCE_RGetVertexArrayData()
 */
void SCE_RSetVertexArrayNewData (SCE_RVertexArray *va,
                                 SCE_EVertexAttribute attrib, SCEenum type,
                                 SCEsizei stride, SCEint size, void *p)
{
    SCE_SGeometryArrayData data;
    data.attrib = attrib;
    data.type = type;
    data.stride = stride;
    data.size = size;
    data.data = p;
    SCE_RSetVertexArrayData (va, &data);
}

/**
 * \brief GL calls to make the given vertex array active for the render
 */
void SCE_RUseVertexArray (SCE_RVertexArray *va)
{
    va->set (&va->data);
    SCE_List_Appendl (&vaused, &va->it);
}
void SCE_RRender (SCE_EPrimitiveType prim, SCEuint n_vertices)
{
    glDrawArrays (sce_rprimtypes[prim], 0, n_vertices);
}
void SCE_RRenderInstanced (SCE_EPrimitiveType prim, SCEuint n_vertices,
                           SCEuint n_inst)
{
    glDrawArraysInstanced (sce_rprimtypes[prim], 0, n_vertices, n_inst);
}
void SCE_RRenderIndexed (SCE_EPrimitiveType prim, SCE_RIndexArray *ia,
                         SCEuint n_indices)
{
    glDrawElements (sce_rprimtypes[prim], n_indices,
                    sce_rgltypes[ia->type], ia->data);
}
void SCE_RRenderIndexedInstanced (SCE_EPrimitiveType prim, SCE_RIndexArray *ia,
                                  SCEuint n_indices, SCEuint n_instances)
{
    glDrawElementsInstanced (sce_rprimtypes[prim], n_indices,
                             sce_rgltypes[ia->type], ia->data, n_instances);
}

/**
 * \brief Call this function when the render of a group of vertex arrays is done
 * \sa SCE_RCallVertexArraySequence(), SCE_REndVertexArraySequence()
 */
void SCE_RFinishVertexArrayRender (void)
{
    SCE_SListIterator *it = NULL;
    if (vao_used) {
        vao_used = SCE_FALSE;
        glBindVertexArray (0);
    }
    /*else*/
    SCE_List_ForEach (it, &vaused) {
        SCE_RVertexArray *va = SCE_List_GetData (it);
        va->unset (&va->data);
    }
    SCE_List_Flush (&vaused);
}


/* bonus API */
/**
 * \brief Mark the beginning of a vertex array setup sequence using the
 * OpenGL Vertex Array Objects
 * \sa SCE_REndVertexArraySequence(), SCE_RCallVertexArraySequence()
 */
void SCE_RBeginVertexArraySequence (SCE_RVertexArraySequence *seq)
{
    if (seq->id != 0)
        glDeleteVertexArrays (1, &seq->id); /* reset and create new */
    glGenVertexArrays (1, &seq->id);
    glBindVertexArray (seq->id);
}
/**
 * \brief Calls the given sequence
 * \sa SCE_RBeginVertexArraySequence(), SCE_REndVertexArraySequence()
 */
void SCE_RCallVertexArraySequence (SCE_RVertexArraySequence seq)
{
    glBindVertexArray (seq.id);
    vao_used = SCE_TRUE;
}
/**
 * \brief Ends a sequence setup or a sequence call
 * \sa SCE_RBeginVertexArraySequence(), SCE_RCallVertexArraySequence()
 */
void SCE_REndVertexArraySequence (void)
{
    glBindVertexArray (0);
    vao_used = SCE_FALSE;
}
/**
 * \brief Destroys a vertex array object
 */
void SCE_RDeleteVertexArraySequence (SCE_RVertexArraySequence *seq)
{
    glDeleteVertexArrays (1, &seq->id);
    seq->id = 0;
}

/** @} */
