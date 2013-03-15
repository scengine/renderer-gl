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

#ifndef SCERBUFFERPOOL_H
#define SCERBUFFERPOOL_H

#include <SCE/utils/SCEUtils.h>
#include "SCE/renderer/SCERBuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sce_rbufferpool SCE_RBufferPool;
struct sce_rbufferpool {
    SCE_SArray buffers[32];
    SCEenum target;
    SCE_RBufferUsage usage;
};

void SCE_RInitBufferPool (SCE_RBufferPool*);
void SCE_RClearBufferPool (SCE_RBufferPool*);

void SCE_RSetBufferPoolTarget (SCE_RBufferPool*, SCEenum);
void SCE_RSetBufferPoolUsage (SCE_RBufferPool*, SCE_RBufferUsage);

int SCE_RFlushBufferPool (SCE_RBufferPool*, SCEuint);

int SCE_RSetBufferPoolBuffer (SCE_RBufferPool*, SCEuint);
long SCE_RGetBufferPoolBuffer (SCE_RBufferPool*, size_t);

int SCE_RReallocBufferPoolBuffer (SCE_RBufferPool*, SCE_RBuffer*, size_t);

size_t SCE_RGetBufferPoolSize (const SCE_RBufferPool*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
