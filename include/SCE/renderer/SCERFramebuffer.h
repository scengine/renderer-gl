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
 
/* created: 02/07/2007
   updated: 17/11/2011 */

#ifndef SCEFRAMEBUFFER_H
#define SCEFRAMEBUFFER_H

#include <SCE/utils/SCEUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup framebuffer
 * @{
 */

typedef enum {
    SCE_COLOR_BUFFER0 = 0,
    SCE_COLOR_BUFFER1,
    SCE_COLOR_BUFFER2,
    SCE_COLOR_BUFFER3,
    SCE_COLOR_BUFFER4,
    SCE_COLOR_BUFFER5,
    SCE_COLOR_BUFFER6,
    SCE_COLOR_BUFFER7,
    SCE_DEPTH_BUFFER,
    SCE_STENCIL_BUFFER,
    SCE_NUM_RENDER_BUFFERS
} SCE_RBufferType;

/* nombre maximum de buffers pouvant etre attaches */
#define SCE_MAX_ATTACHMENT_BUFFERS 8 /* TODO: a mettre a jour regulierement */

#define SCE_COLOR_BUFFER SCE_COLOR_BUFFER0


/**
 * \copydoc sce_rrenderbuffer
 */
typedef struct sce_rrenderbuffer SCE_RRenderBuffer;
/**
 * \brief A render buffer structure
 *
 * This structure stores a texture or a OpenGL render buffer.
 */
struct sce_rrenderbuffer {
    SCEuint id;        /**< OpenGL render buffer's identifier */
    SCE_RTexture *tex; /**< The render texture */
    int user;          /**< Is user the owner of \c tex ? */
    int activated;     /**< Is render buffer activated ? */
};

/**
 * \copydoc sce_rframebuffer
 */
typedef struct sce_rframebuffer SCE_RFramebuffer;
/**
 * \brief A frame buffer structure
 *
 * This is a FBO.
 */
struct sce_rframebuffer {
    SCEuint id;             /**< OpenGL identifier of the FBO */
    /** All the targets of \c id, used for Multiple Render Targets extension */
    SCE_RRenderBuffer buffers[SCE_NUM_RENDER_BUFFERS];
    int x, y, w, h; /* TODO: use rectangle ? */
};

/** @} */

int SCE_RFramebufferInit (void);
void SCE_RFramebufferQuit (void);

void SCE_RInitFramebuffer (SCE_RFramebuffer*);
SCE_RFramebuffer* SCE_RCreateFramebuffer (void);
void SCE_RDeleteFramebuffer (SCE_RFramebuffer*);

int SCE_RAddRenderTexture (SCE_RFramebuffer*, SCE_RBufferType, SCEenum,
                           SCE_RTexture*, int, int);

int SCE_RAddRenderBuffer (SCE_RFramebuffer*, SCE_RBufferType,
                          SCE_EImageFormat, int, int);

SCE_RTexture* SCE_RAddNewRenderTexture (SCE_RFramebuffer*, SCE_RBufferType,
                                        SCE_EPixelFormat, SCE_EImageFormat,
                                        SCE_EType, int, int);

SCE_RTexture* SCE_RGetRenderTexture (SCE_RFramebuffer*, SCE_RBufferType);

void SCE_RActivateRenderBuffer (SCE_RFramebuffer*, SCE_RBufferType, int);
void SCE_REnableRenderBuffer (SCE_RFramebuffer*, SCE_RBufferType);
void SCE_RDisableRenderBuffer (SCE_RFramebuffer*, SCE_RBufferType);

void SCE_RClearRenderbuffer (SCE_RFramebuffer*, SCEuint, int);

unsigned int SCE_RGetMaxAttachmentBuffers (void);

void SCE_RUseFramebuffer (SCE_RFramebuffer*, SCE_SIntRect*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
