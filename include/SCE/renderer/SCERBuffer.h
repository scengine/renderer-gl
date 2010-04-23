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
   updated: 02/08/2009 */

#ifndef SCERBUFFER_H
#define SCERBUFFER_H

#include <SCE/utils/SCEUtils.h>
#include "SCE/renderer/SCERVertexArray.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup corebuffers
 * @{
 */

/**
 * \brief Usage modes for a buffer
 */
enum sce_rbufferusage {
    SCE_BUFFER_DEFAULT_USAGE = 0, /* hope that GL enums are non-zero */
    SCE_BUFFER_STREAM_DRAW = GL_STREAM_DRAW,
    SCE_BUFFER_STREAM_READ = GL_STREAM_READ,
    SCE_BUFFER_STREAM_COPY = GL_STREAM_COPY,
    SCE_BUFFER_STATIC_DRAW = GL_STATIC_DRAW,
    SCE_BUFFER_STATIC_READ = GL_STATIC_READ,
    SCE_BUFFER_STATIC_COPY = GL_STATIC_COPY,
    SCE_BUFFER_DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
    SCE_BUFFER_DYNAMIC_READ = GL_DYNAMIC_READ,
    SCE_BUFFER_DYNAMIC_COPY = GL_DYNAMIC_COPY
};
/** \copydoc sce_rbufferusage */
typedef enum sce_rbufferusage SCE_RBufferUsage;

/** \copydoc sce_rbuffer */
typedef struct sce_rbuffer SCE_RBuffer;

/** \copydoc sce_rbufferdata */
typedef struct sce_rbufferdata SCE_RBufferData;
/**
 * \brief Data of a buffer
 */
struct sce_rbufferdata {
    SCEuint first;              /**< Offset of the data in the buffer */
    size_t size;                /**< Bytes of \c data */
    void *data;                 /**< Data, user is the owner of */
    size_t range[2];            /**< Range of modified bytes \c range[0] is the
                                 *   offset and \c range[1] the number of
                                 *   modified bytes */
    SCE_SListIterator it;
    int modified;               /**< Is this buffer data waiting for update? */
    SCE_RBuffer *buf;
    int user;
};

/**
 * \brief A GL buffer
 */
struct sce_rbuffer {
    SCEuint id;                 /**< GL identifier */
    SCEenum target;             /**< GL target (type of the buffer) */
    size_t size;                /**< Total bytes of the buffer */
    SCE_SList data;             /**< Buffer data (SCE_RBufferData) */
    SCE_SList modified;         /**< Data stored here when need update */
    size_t range[2];            /**< Range of modified bytes
                                 *   (min/max of \c modified).
                                 *   Used for glMapBufferRange() */
    void *mapptr;               /**< Buffer address saved here on locking */
    SCE_SListIterator it;       /**< Own iterator for modified buffers list */
};

/** @} */

int SCE_RBufferInit (void);
void SCE_RBufferQuit (void);

void SCE_RInitBufferData (SCE_RBufferData*);
SCE_RBufferData* SCE_RCreateBufferData (void);
void SCE_RClearBufferData (SCE_RBufferData*);
void SCE_RDeleteBufferData (SCE_RBufferData*);

void SCE_RInitBuffer (SCE_RBuffer*);
SCE_RBuffer* SCE_RCreateBuffer (void);
void SCE_RClearBuffer (SCE_RBuffer*);
void SCE_RDeleteBuffer (SCE_RBuffer*);

void SCE_RModifiedBuffer (SCE_RBuffer*, const size_t*);
void SCE_RUnmodifiedBuffer (SCE_RBuffer*);
void SCE_RModifiedBufferData (SCE_RBufferData*, const size_t*);
void SCE_RUnmodifiedBufferData (SCE_RBufferData*);

void SCE_RAddBufferData (SCE_RBuffer*, SCE_RBufferData*);
SCE_RBufferData* SCE_RAddBufferNewData (SCE_RBuffer*, size_t, void*);
void SCE_RRemoveBufferData (SCE_RBufferData*);

void SCE_RBuildBuffer (SCE_RBuffer*, SCEenum, SCE_RBufferUsage);
extern void (*SCE_RUpdateBuffer) (SCE_RBuffer*);
void SCE_RUpdateModifiedBuffers (void);
void SCE_RUseBuffer (SCE_RBuffer*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
