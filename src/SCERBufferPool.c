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

/* created: 14/03/2012
   updated: 14/03/2012 */

#include <GL/glew.h>
#include <SCE/utils/SCEUtils.h>

#include "SCE/renderer/SCERBufferPool.h"

void SCE_RInitBufferPool (SCE_RBufferPool *pool)
{
    size_t i;
    for (i = 0; i < 32; i++)
        SCE_Array_Init (&pool->buffers[i]);
    pool->target = GL_ARRAY_BUFFER;
    pool->usage = SCE_BUFFER_STREAM_DRAW;
}
void SCE_RClearBufferPool (SCE_RBufferPool *pool)
{
    size_t i, size;
    for (i = 0; i < 32; i++) {
        size = SCE_Array_GetSize (&pool->buffers[i]) / sizeof (SCEuint);
        glDeleteBuffers (size, SCE_Array_Get (&pool->buffers[i]));
        SCE_Array_Clear (&pool->buffers[i]);
    }
}

void SCE_RSetBufferPoolTarget (SCE_RBufferPool *pool, SCEenum target)
{
    pool->target = target;
}
void SCE_RSetBufferPoolUsage (SCE_RBufferPool *pool, SCE_RBufferUsage usage)
{
    pool->usage = usage;
}

static SCEuint getindex (unsigned int size)
{
    return 1 + (SCEuint)(log ((double)size) / log (2.0));
}


int SCE_RFlushBufferPool (SCE_RBufferPool *pool, SCEuint max)
{
    size_t i, size;
    SCE_SArray *a = NULL;

    max *= sizeof (SCEuint);
    for (i = 0; i < 32; i++) {
        a = &pool->buffers[i];
        size = SCE_Array_GetSize (a);
        if (size > max) {
            SCEubyte *ptr = SCE_Array_Get (a);
            glDeleteBuffers (size - max, &ptr[max]);
            if (SCE_Array_PopBack (a, size - max) < 0) {
                SCEE_LogSrc ();
                return SCE_ERROR;
            }
        }
    }

    return SCE_OK;
}

static SCEuint SCE_RNewBuffer (SCE_RBufferPool *pool, size_t size)
{
    SCEuint id;
    glGenBuffers (1, &id);
    glBindBuffer (pool->target, id);
    glBufferData (pool->target, size, NULL, pool->usage);
    glBindBuffer (pool->target, 0);
    return id;
}

/**
 * \brief Adds a buffer to a pool
 * \param pool a pool
 * \param id a buffer ID, if 0 the function simply returns SCE_OK
 * \return SCE_ERROR on error, SCE_OK otherwise
 */
int SCE_RSetBufferPoolBuffer (SCE_RBufferPool *pool, SCEuint id)
{
    GLint size;

    if (id == 0)
        return SCE_OK;

    glBindBuffer (pool->target, id);
    glGetBufferParameteriv (pool->target, GL_BUFFER_SIZE, &size);
    glBindBuffer (pool->target, 0);
    if (!SCE_Math_PowerOfTwo (size))
        glDeleteBuffers (1, &id);
    else {
        SCEuint index = getindex (size);
        if (SCE_Array_Append (&pool->buffers[index], &id, sizeof id) < 0) {
            SCEE_LogSrc ();
            return SCE_ERROR;
        }
    }

    return SCE_OK;
}

/**
 * \brief Retrieve a buffer from a pool
 * \param pool a pool
 * \param size size of the desired buffer
 * \return the identifier of a buffer of size at least \p size,
 * SCE_ERROR on error
 */
long SCE_RGetBufferPoolBuffer (SCE_RBufferPool *pool, size_t size)
{
    SCEuint id;
    SCEuint index = getindex (size);

    if (size == 0)
        return 0;

    if (!SCE_Array_GetSize (&pool->buffers[index])) {
        /* pool is empty */
        id = SCE_RNewBuffer (pool, 1 << index);
    } else {
        id = *((SCEuint*)SCE_Array_Get (&pool->buffers[index]));
        if (SCE_Array_PopFront (&pool->buffers[index], sizeof id) < 0) {
            SCEE_LogSrc ();
            return SCE_ERROR;
        }
    }

    return id;
}


int SCE_RReallocBufferPoolBuffer (SCE_RBufferPool *pool, SCE_RBuffer *buf,
                                  size_t size)
{
    long id;
    if (SCE_RSetBufferPoolBuffer (pool, buf->id) < 0)
        goto fail;
    if ((id = SCE_RGetBufferPoolBuffer (pool, size)) < 0)
        goto fail;
    buf->id = id;
    buf->size = SCE_Math_NextPowerOfTwo (size);
    return SCE_OK;
fail:
    SCEE_LogSrc ();
    return SCE_ERROR;
}


static size_t get_buffer_size (SCEenum target, SCEuint id)
{
    GLint size = 0;
    if (id) {
        glBindBuffer (target, id);
        glGetBufferParameteriv (target, GL_BUFFER_SIZE, &size);
        glBindBuffer (target, 0);
    }
    return (size_t)size;
}

size_t SCE_RGetBufferPoolSize (const SCE_RBufferPool *pool)
{
    size_t i, j, n, size = 0;
    SCEuint *ptr = NULL;

    for (i = 0; i < 32; i++) {
        n = SCE_Array_GetSize (&pool->buffers[i]) / sizeof (SCEuint);
        ptr = SCE_Array_Get (&pool->buffers[i]);
        for (j = 0; j < n; j++)
            size += get_buffer_size (pool->target, ptr[j]);
    }

    return size;
}
