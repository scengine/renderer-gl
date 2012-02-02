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

#include <GL/glew.h>
#include <SCE/utils/SCEUtils.h>
#include <SCE/core/SCECore.h>   /* SCE_EPrimitiveType */
#include "SCE/renderer/SCERenderer.h"   /* SCE_RGetError() */

#include "SCE/renderer/SCERBuffer.h"
#include "SCE/renderer/SCERFeedback.h"

static void SCE_RInitFeedbackStream (SCE_RFeedbackStream *fs)
{
    fs->buf = NULL;
    fs->range[0] = fs->range[1] = -1; /* -1 means "all" the buffer */
    fs->query = 0;
}
static void SCE_RClearFeedbackStream (SCE_RFeedbackStream *fs)
{
    if (fs->query)
        glDeleteQueries (1, &fs->query);
}


void SCE_RInitFeedback (SCE_RFeedback *fb)
{
    size_t i;
    fb->id = 0;
    for (i = 0; i < SCE_MAX_FEEDBACK_STREAMS; i++)
        SCE_RInitFeedbackStream (&fb->streams[i]);
    fb->n_streams = 0;
}
void SCE_RClearFeedback (SCE_RFeedback *fb)
{
    size_t i;
    if (fb->id)
        glDeleteTransformFeedbacks (1, &fb->id);
    for (i = 0; i < SCE_MAX_FEEDBACK_STREAMS; i++)
        SCE_RClearFeedbackStream (&fb->streams[i]);
}

static SCE_RFeedbackStream*
SCE_RLocateStream (SCE_RFeedback *fb, const SCE_RBuffer *buf)
{
    size_t i;
    for (i = 0; i < fb->n_streams; i++) {
        if (fb->streams[i].buf == buf)
            return &fb->streams[i];
    }
    return NULL;
}

/**
 * \brief Add or edit an existing stream as a feedback target
 * \param fb feedback object
 * \param buf stream to add/edit
 * \param range array of 2 values specifying the range of the buffer to render
 * to, values must be multiple of 4 or -1 to specify the whole buffer. Passing NULL
 * also specifies the whole buffer.
 * \returns the stream ID \p buf has been bound to
 *
 * This function adds a new stream target for feedback rendering. The stream
 * will be filled in the specified range \p range next time you process
 * a feedback rendering using \p fb. You can specify up to
 * SCE_MAX_FEEDBACK_STREAMS different output buffers.
 *
 * If \p buf is already in \p fb's stream list, its range is updated
 * with \p range.
 * \sa SCE_RDetachFeedbackStream(), SCE_RClearFeedbackStreams()
 */
int SCE_RAddFeedbackStream (SCE_RFeedback *fb, const SCE_RBuffer *buf,
                            const int *range)
{
    SCE_RFeedbackStream *s = NULL;
    /* search for existing stream */
    if ((s = SCE_RLocateStream (fb, buf))) {
        /* found, edit range */
        s->range[0] = range[0];
        s->range[1] = range[1];
        return;
    }
    /* none found, add new stream */
    if (fb->n_streams < SCE_MAX_FEEDBACK_STREAMS) {
        fb->streams[fb->n_streams].buf = buf;
        if (range) {
            fb->streams[fb->n_streams].range[0] = range[0];
            fb->streams[fb->n_streams].range[1] = range[1];
        } else {
            fb->streams[fb->n_streams].range[0] = -1;
            fb->streams[fb->n_streams].range[1] = -1;
        }
        fb->n_streams++;
    }
}
/**
 * \brief Removes a stream target
 * \param fb feedback object
 * \param buf stream
 * 
 * Stream \p buf will no longer be a target of \p fb. If \p buf wasn't
 * in \p fb's stream list, this function has none effect.
 * \sa SCE_RAttachFeedbackStream(), SCE_RClearFeedbackStreams()
 */
void SCE_RRemoveFeedbackStream (SCE_RFeedback *fb, const SCE_RBuffer *buf)
{
    SCE_RFeedbackStream *s = NULL;
    if ((s = SCE_RLocateStream (fb, buf))) {
        fb->n_streams--;
        /* TODO: this changes the stream index */
        *s = fb->streams[fb->n_streams];
        SCE_RClearFeedbackStream (&fb->streams[fb->n_streams]);
        SCE_RInitFeedbackStream (&fb->streams[fb->n_streams]);
    }
}
/**
 * \brief Removes all stream targets
 * \param fb feedback object
 * \sa SCE_RAttachFeedbackStream(), SCE_RDetachFeedbackStreams()
 */
void SCE_RClearFeedbackStreams (SCE_RFeedback *fb)
{
    size_t i;
    for (i = 0; i < fb->n_streams; i++) {
        SCE_RClearFeedbackStream (&fb->streams[i]);
        SCE_RInitFeedbackStream (&fb->streams[i]);
    }
    fb->n_streams = 0;
}


/**
 * \brief Begin rendering to vertex buffers
 * \param fb a feedback object
 * \param prim only SCE_POINTS, SCE_LINES and SCE_TRIANGLES values are accepted
 */
void SCE_RBeginFeedback (SCE_RFeedback *fb, SCE_EPrimitiveType prim)
{
    SCEuint i;

    /* bind target streams */
    /* TODO: is it necessary to bind them each time we want to proceed a
       feedback rendering or does the object remind them? */
    for (i = 0; i < fb->n_streams; i++) {
        SCE_RFeedbackStream *s = &fb->streams[i];
        SCEuint id = SCE_RGetBufferID (s->buf);
        if (s->range[0] >= 0)
            glBindBufferRange (GL_TRANSFORM_FEEDBACK_BUFFER, i, id,
                               s->range[0], s->range[1]);
        else
            glBindBufferBase (GL_TRANSFORM_FEEDBACK_BUFFER, i, id);
#if 0
        if (s->counting) {
            glBeginQueryIndexed (GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
                                 i, s->query);
        }
#endif
    }

    /* lol derp */
    if (fb->streams[0].counting)
        glBeginQuery (GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
                      fb->streams[0].query);

#if 0
    if (!fb->id)
        glGenTransformFeedbacks (1, &fb->id);
    glBindTransformFeedback (GL_TRANSFORM_FEEDBACK, fb->id);
#endif
    glBeginTransformFeedback (sce_rprimtypes_true[prim]);
}
void SCE_REndFeedback (SCE_RFeedback *fb)
{
    size_t i;

    glEndTransformFeedback ();

    glEndQuery (GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
    glGetQueryObjectiv (fb->streams[0].query, GL_QUERY_RESULT,
                        &fb->streams[0].n_primitives);

    /* glBindTransformFeedback (GL_TRANSFORM_FEEDBACK, 0); */
    for (i = 0; i < fb->n_streams; i++) {
        SCE_RFeedbackStream *s = &fb->streams[i];
#if 0
        if (s->counting) {
            /* retrieve count result and unbind the query object */
            glGetQueryIndexediv (GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
                                 i, GL_CURRENT_QUERY, &s->n_primitives);
            glEndQueryIndexed (GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, i);
        }
#endif
        /* TODO: check/ask whether this is useless */
        glBindBufferBase (GL_TRANSFORM_FEEDBACK_BUFFER, i, 0);
    }
}

void SCE_RPauseFeedback (void)
{
    glPauseTransformFeedback ();
}
void SCE_RResumeFeedback (void)
{
    glResumeTransformFeedback ();
}

void SCE_REnableFeedbackCounting (SCE_RFeedback *fb, const SCE_RBuffer *buf)
{
    SCE_RFeedbackStream *s = NULL;
    if ((s = SCE_RLocateStream (fb, buf))) {
        if (!s->query)
            glGenQueries (1, &s->query);
        s->counting = SCE_TRUE;
    }
}
void SCE_RDisbleFeedbackCounting (SCE_RFeedback *fb, const SCE_RBuffer *buf)
{
    SCE_RFeedbackStream *s = NULL;
    if ((s = SCE_RLocateStream (fb, buf)))
        s->counting = SCE_FALSE;
}
int SCE_RGetFeedbackNumPrimitives (const SCE_RFeedback *fb,
                                   const SCE_RBuffer *buf)
{
    SCE_RFeedbackStream *s = NULL;
    if ((s = SCE_RLocateStream ((SCE_RFeedback*)fb, buf)))
        return s->n_primitives;
    return 0;
}
