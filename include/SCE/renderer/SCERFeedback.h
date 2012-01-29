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

/* created: 27/01/2012
   updated: 27/01/2012 */

#ifndef SCERFEEDBACK_H
#define SCERFEEDBACK_H

#include <SCE/core/SCECore.h>   /* SCE_EPrimitiveType */
#include "SCE/renderer/SCERBuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SCE_MAX_FEEDBACK_STREAMS 4

typedef struct sce_rfeebackstream SCE_RFeedbackStream;
struct sce_rfeebackstream {
    SCEuint query;
    int counting;
    int n_primitives;
    const SCE_RBuffer *buf;     /**< Output buffer */
    int range[2];               /**< Range of the buffer to bind */
};

typedef struct sce_rfeedback SCE_RFeedback;
struct sce_rfeedback {
    SCEuint id;
    /** Attached streams */
    SCE_RFeedbackStream streams[SCE_MAX_FEEDBACK_STREAMS];
    size_t n_streams;
};

void SCE_RInitFeedback (SCE_RFeedback*);
void SCE_RClearFeedback (SCE_RFeedback*);

int SCE_RAddFeedbackStream (SCE_RFeedback*, const SCE_RBuffer*, const int[2]);
void SCE_RRemoveFeedbackStream (SCE_RFeedback*, const SCE_RBuffer*);
void SCE_RClearFeedbackStreams (SCE_RFeedback*);

void SCE_RBeginFeedback (SCE_RFeedback*, SCE_EPrimitiveType);
void SCE_REndFeedback (SCE_RFeedback*);

void SCE_RPauseFeedback (void);
void SCE_RResumeFeedback (void);

void SCE_REnableFeedbackCounting (SCE_RFeedback*, const SCE_RBuffer*);
void SCE_RDisbleFeedbackCounting (SCE_RFeedback*, const SCE_RBuffer*);
int SCE_RGetFeedbackNumPrimitives (const SCE_RFeedback*, const SCE_RBuffer*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
