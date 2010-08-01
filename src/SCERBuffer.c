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
 
/* created: 10/01/2007
   updated: 21/08/2009 */

#include <string.h>             /* memcpy */
#include <GL/glew.h>

#include <SCE/utils/SCEUtils.h>  /* MIN/MAX */
#include "SCE/renderer/SCERSupport.h"
#include "SCE/renderer/SCERBuffer.h"

/**
 * \file SCECBuffer.c
 * \copydoc
 * \file SCECBuffer.h
 * \copydoc
 */

/**
 * \defgroup corebuffers GL buffers
 * \ingroup core
 * \internal
 * \brief OpenGL buffers generic management
 * @{
 */

static SCE_SList modified;      /* all modified buffers */

void (*SCE_RUpdateBuffer) (SCE_RBuffer*);
static void SCE_RUpdateBufferMapClassic (SCE_RBuffer*);
static void SCE_RUpdateBufferMapRange (SCE_RBuffer*);

int SCE_RBufferInit (void)
{
    /* add LockBuffer() too */
    if (SCE_RIsSupported ("GL_ARB_map_buffer_range")) {
        SCE_RUpdateBuffer = SCE_RUpdateBufferMapRange;
    } else {
        SCE_RUpdateBuffer = SCE_RUpdateBufferMapClassic;
    }
    SCE_List_Init (&modified);
    return SCE_OK;
}
void SCE_RBufferQuit (void)
{
    SCE_List_Flush (&modified);
}

void SCE_RInitBufferData (SCE_RBufferData *data)
{
    data->first = 0;
    data->size = 0;
    data->data = NULL;
    data->range[0] = data->range[1] = 0;
    SCE_List_InitIt (&data->it);
    SCE_List_SetData (&data->it, data);
    data->modified = SCE_FALSE;
    data->buf = NULL;
    data->user = SCE_TRUE;
}
SCE_RBufferData* SCE_RCreateBufferData (void)
{
    SCE_RBufferData *data = NULL;
    if (!(data = SCE_malloc (sizeof *data)))
        SCEE_LogSrc ();
    else
        SCE_RInitBufferData (data);
    return data;
}
void SCE_RClearBufferData (SCE_RBufferData *data)
{
    SCE_RRemoveBufferData (data);
}
void SCE_RDeleteBufferData (SCE_RBufferData *data)
{
    if (data) {
        SCE_RClearBufferData (data);
        SCE_free (data);
    }
}

static void SCE_RFreeBufferBufferData (void *d)
{
    SCE_RBufferData *data = d;
    if (data->user)
        SCE_RRemoveBufferData (data);
    else
        SCE_RDeleteBufferData (data);
}
void SCE_RInitBuffer (SCE_RBuffer *buf)
{
    glGenBuffers (1, &buf->id);
    buf->target = GL_ARRAY_BUFFER;
    buf->size = 0;
    SCE_List_Init (&buf->data);
    SCE_List_SetFreeFunc (&buf->data, SCE_RFreeBufferBufferData);
    SCE_List_Init (&buf->modified);
    SCE_List_SetFreeFunc (&buf->modified, SCE_RFreeBufferBufferData);
    buf->range[0] = buf->range[1] = 0;
    buf->mapptr = NULL;
    SCE_List_InitIt (&buf->it);
    SCE_List_SetData (&buf->it, buf);
}
SCE_RBuffer* SCE_RCreateBuffer (void)
{
    SCE_RBuffer *buf = NULL;
    if (!(buf = SCE_malloc (sizeof *buf)))
        SCEE_LogSrc ();
    else
        SCE_RInitBuffer (buf);
    return buf;
}
void SCE_RClearBuffer (SCE_RBuffer *buf)
{
    glDeleteBuffers (1, &buf->id);
    buf->id = 0;
    SCE_List_Clear (&buf->modified);
    SCE_List_Clear (&buf->data);
    SCE_List_Remove (&buf->it);
}
void SCE_RDeleteBuffer (SCE_RBuffer *buf)
{
    if (buf) {
        SCE_RClearBuffer (buf);
        SCE_free (buf);
    }
}


/* prout */
static void SCE_RResetBufferRange (SCE_RBuffer *buf)
{
    buf->range[0] = buf->size;
    buf->range[1] = 0;
}
/* only used into CModifiedBuffer() */
static void SCE_RUpdateBufferRange (SCE_RBuffer *buf, const size_t *range)
{
    /* offset from the main buffer */
    size_t offset = range[0];
    buf->range[0] = MIN (buf->range[0], offset);
    buf->range[1] = MAX (buf->range[1], offset + range[1]);
}
/**
 * \brief Sets the modified range of an entire buffer, useful for index buffers
 */
void SCE_RModifiedBuffer (SCE_RBuffer *buf, const size_t *range)
{
    size_t r[2];
    if (!range) {
        r[0] = 0;
        r[1] = buf->size;
        range = r;
    }
    SCE_RUpdateBufferRange (buf, range);
}
/**
 * \brief Mark a buffer as unmodified: will not be updated by
 * SCE_RUpdateModifiedBuffers()
 * \sa SCE_RModifiedBuffer(), SCE_RModifiedBufferData()
 */
void SCE_RUnmodifiedBuffer (SCE_RBuffer *buf)
{
    SCE_RResetBufferRange (buf);
    SCE_List_Remove (&buf->it);
}
/**
 * \brief Defines a buffer data as modified
 * \param range modified range in bytes, NULL means 'all'
 * \sa SCE_RUpdateModifiedBuffers(), SCE_RUpdateBuffer(),
 * SCE_RUnmodifiedBufferData()
 */
void SCE_RModifiedBufferData (SCE_RBufferData *data, const size_t *range)
{
    SCE_List_Removel (&data->it);
    SCE_List_Appendl (&data->buf->modified, &data->it);
    if (range) {
        /* if already modified, get the largest range */
        if (data->modified) {
            data->range[0] = MIN (data->range[0], range[0]);
            data->range[1] = MAX (data->range[1], range[1]);
        } else {
            data->range[0] = range[0];
            data->range[1] = range[1];
        }
    } else {
        data->range[0] = 0;
        data->range[1] = data->size;
    }
    {
        size_t r[2];
        r[0] = data->range[0] + data->first;
        r[1] = data->range[1];
        SCE_RModifiedBuffer (data->buf, r);
    }
    data->modified = SCE_TRUE;
    /* add the buffer to the modified buffers list */
    SCE_List_Remove (&data->buf->it);
    SCE_List_Appendl (&modified, &data->buf->it);
}
/**
 * \brief Defines a buffer data as unmodified (will not be updated)
 * \sa SCE_RModifiedBufferData()
 */
void SCE_RUnmodifiedBufferData (SCE_RBufferData *data)
{
    SCE_List_Removel (&data->it);
    SCE_List_Appendl (&data->buf->data, &data->it);
    data->modified = SCE_FALSE;
}

/**
 * \brief Adds a buffer data to a buffer
 * \sa SCE_RAddBufferNewData(), SCE_RRemoveBufferData()
 */
void SCE_RAddBufferData (SCE_RBuffer *buf, SCE_RBufferData *data)
{
    SCE_List_Appendl (&buf->data, &data->it);
    data->buf = buf;
    data->first = buf->size;
    buf->size += data->size;
}
/**
 * \brief Create and adds a new buffer data
 * \param s size of the new buffer data in bytes
 * \param p pointer to the data of the buffer data
 * \sa SCE_RAddBufferData(), SCE_RRemoveBufferData()
 */
SCE_RBufferData* SCE_RAddBufferNewData (SCE_RBuffer *buf, size_t s, void *p)
{
    SCE_RBufferData *data = NULL;
    if (!(data = SCE_RCreateBufferData ()))
        SCEE_LogSrc ();
    else {
        data->user = SCE_FALSE;
        data->size = s;
        data->data = p;
        SCE_RAddBufferData (buf, data);
    }
    return data;
}
/**
 * \brief Removes a buffer data from its buffer
 * \sa SCE_RAddBufferData(), SCE_RAddBufferNewData()
 */
void SCE_RRemoveBufferData (SCE_RBufferData *data)
{
    if (data->buf) {
        SCE_List_Removel (&data->it);
        data->buf = NULL;
    }
}

/**
 * \brief Builds a buffer
 * \param target type of the buffer (todo: use an enum)
 * \param usage buffer usage
 */
void SCE_RBuildBuffer (SCE_RBuffer *buf, SCEenum target, SCE_RBufferUsage usage)
{
    SCE_RBufferData *data = NULL;
    SCE_SListIterator *it = NULL;
    glBindBuffer (target, buf->id);
    glBufferData (target, buf->size, NULL, usage);
    SCE_List_ForEach (it, &buf->data) {
        data = SCE_List_GetData (it);
        glBufferSubData (target, data->first, data->size, data->data);
    }
    glBindBuffer (target, 0);
    buf->target = target;
    SCE_RResetBufferRange (buf);
}

#if 0
void* SCE_RLockBufferClassic (SCE_RBuffer *buf, size_t *range,
                              SCE_RLockBufferFlags flags)
{
    SCEenum mode;
    if (flags & SCE_BUFFER_WRITE_LOCK && flags & SCE_BUFFER_READ_LOCK)
        mode = GL_READ_WRITE;
    else if (flags & SCE_BUFFER_WRITE_LOCK)
        mode = GL_WRITE_ONLY;
    else if (flags & SCE_BUFFER_READ_LOCK)
        mode = GL_READ_ONLY;
    glBindBuffer (buf->target, buf->id);
    glMapBuffer (buf->target, mode);
}
void* SCE_RLockBufferRange (SCE_RBuffer *buf, size_t *range,
                            SCE_RLockBufferFlags flags)
{
    SCEbitfield mode = 0;
    if (flags & SCE_BUFFER_WRITE_LOCK)
        mode |= GL_MAP_WRITE_BIT;
    if (flags & SCE_BUFFER_READ_LOCK)
        mode |= GL_MAP_READ_BIT;
    glBindBuffer (buf->target, buf->id);
    glMapBuffer (buf->target, mode);
}
void SCE_RUnlockBuffer (SCE_RBuffer *buf)
{
    glBindBuffer (buf->target, buf->id);
    glUnmapBuffer (buf->target);
}
#endif

static void SCE_RUpdateBufferMapClassic (SCE_RBuffer *buf)
{
    void *ptr = NULL;
    SCE_SListIterator *pro = NULL, *it = NULL;
    SCEenum target = buf->target;
    /* TODO: do it all in one? */
    glBindBuffer (target, buf->id);
    ptr = glMapBuffer (target, GL_WRITE_ONLY);
#ifdef SCE_DEBUG
    if (!ptr) {
        /* TODO: make a 'shortcut' for this code */
        SCEE_Log (SCE_GL_ERROR);
        SCEE_LogMsg ("GL error on glMapBuffer()");
        return;                 /* lonlz */
    }
#endif
    SCE_List_ForEachProtected (pro, it, &buf->modified) {
        SCE_RBufferData *bd = SCE_List_GetData (it);
        memcpy (&((char*)ptr)[bd->range[0] + bd->first],
                &((char*)bd->data)[bd->range[0]],
                bd->range[1]);
        SCE_RUnmodifiedBufferData (bd);
    }
    glUnmapBuffer (target);
    glBindBuffer (target, 0);
    SCE_RResetBufferRange (buf);
}
static void SCE_RUpdateBufferMapRange (SCE_RBuffer *buf)
{
    void *ptr = NULL;
    SCE_SListIterator *pro = NULL, *it = NULL;
    SCEenum target = buf->target;
    /* TODO: do it all in one? */
    glBindBuffer (target, buf->id);
    ptr = glMapBufferRange (target, buf->range[0], buf->range[1],
                            GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
    /* errors generated by glMapBufferRange() are user-errors, except
       GL_OUT_OF_MEMORY, which can occur in this function */
#ifdef SCE_DEBUG
    if (!ptr) {
        /* TODO: shortcut */
        SCEE_Log (SCE_GL_ERROR);
        SCEE_LogMsg ("GL error on glMapBufferRange()");
        return;                 /* lonlz */
    }
#endif
    SCE_List_ForEachProtected (pro, it, &buf->modified) {
        size_t offset, length;
        SCE_RBufferData *bd = SCE_List_GetData (it);
        offset = bd->range[0] + bd->first - buf->range[0];
        length = bd->range[1];
        memcpy (&((char*)ptr)[offset],
                &((char*)bd->data)[bd->range[0]],
                length);
        SCE_RUnmodifiedBufferData (bd);
        /* give to the GL the modified subrange */
        glFlushMappedBufferRange (target, offset, length);
    }
    glUnmapBuffer (target);
    glBindBuffer (target, 0);
    SCE_RResetBufferRange (buf);
}

/**
 * \brief Updates all buffers containing modified buffer data
 * \sa SCE_RModifiedBufferData(), SCE_RUpdateBuffer()
 */
void SCE_RUpdateModifiedBuffers (void)
{
    SCE_SListIterator *it;
    SCE_List_ForEach (it, &modified)
        SCE_RUpdateBuffer (SCE_List_GetData (it));
    SCE_List_Flush (&modified);
}

/**
 * \brief Sets up a buffer as activated
 */
void SCE_RUseBuffer (SCE_RBuffer *buf)
{
    glBindBuffer (buf->target, buf->id);
}

/** @} */
