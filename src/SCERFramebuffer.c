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

#include <GL/glew.h>
#include <SCE/core/SCECore.h>   /* SCE_STexData */
#include "SCE/renderer/SCERType.h"
#include "SCE/renderer/SCERSupport.h"
#include "SCE/renderer/SCERTexture.h"
#include "SCE/renderer/SCERFramebuffer.h"

/**
 * \file SCERFramebuffer.c
 * \copydoc glframebuffer
 * 
 * \file SCERFramebuffer.h
 * \copydoc glframebuffer
 */

/**
 * \defgroup glframebuffer Frame Buffer Objects and render to texture
 * \ingroup renderer-gl
 * \internal
 *
 * Offers frame buffers' managment functions for render to texture
 */

/** @{ */

static SCEint max_attachement_buffers = 0;

int SCE_RFramebufferInit (void)
{
    if (SCE_RHasCap (SCE_MRT))
        glGetIntegerv (GL_MAX_DRAW_BUFFERS, &max_attachement_buffers);
    return SCE_OK;
}
void SCE_RFramebufferQuit (void)
{
}

/**
 * \brief Initializes a framebuffer structure
 * \param fb the structure to initialize
 */
void SCE_RInitFramebuffer (SCE_RFramebuffer *fb)
{
    unsigned int i;

    fb->id = 0;
    for (i = 0; i < SCE_NUM_RENDER_BUFFERS; i++) {
        fb->buffers[i].id = 0;
        fb->buffers[i].tex = NULL;
        fb->buffers[i].user = SCE_TRUE;
        fb->buffers[i].activated = SCE_FALSE;
    }
    fb->x = fb->y = 0;
    fb->w = fb->h = 1;
}

/**
 * \brief Creates a new framebuffer
 * \returns a pointer to the new framebuffer
 */
SCE_RFramebuffer* SCE_RCreateFramebuffer (void)
{
    SCE_RFramebuffer *fb = NULL;
    if (!(fb = SCE_malloc (sizeof *fb))) {
        SCEE_LogSrc ();
        return NULL;
    }
    SCE_RInitFramebuffer (fb);
    glGenFramebuffersEXT (1, &fb->id);
    return fb;
}

/**
 * \brief Deletes an existing frame buffer
 * \param fb the frame buffer to delete
 */
void SCE_RDeleteFramebuffer (SCE_RFramebuffer *fb)
{
    if (fb) {
        unsigned int i;
        for (i = 0; i < SCE_NUM_RENDER_BUFFERS; i++) {
            if (!fb->buffers[i].user)
                SCE_RDeleteTexture (fb->buffers[i].tex);

            if (fb->buffers[i].id)
                glDeleteRenderbuffersEXT (1, &fb->buffers[i].id);
        }
        glDeleteFramebuffersEXT (1, &fb->id);
        SCE_free (fb);
    }
}


/* converti un simple ID de type d'attachement en type opengl */
static int SCE_RIDToGLBuffer (SCEuint type)
{
    switch (type) {
    case SCE_DEPTH_BUFFER: return GL_DEPTH_ATTACHMENT_EXT;
    case SCE_STENCIL_BUFFER: return GL_STENCIL_ATTACHMENT_EXT;
    default: return GL_COLOR_ATTACHMENT0_EXT + type;
    }
}

static const char* SCE_RGetFramebufferError (SCEenum err)
{
    switch (err) {
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        return "incomplete attachement";
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        return "incomplete draw buffer";
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        return "incomplete read buffer";
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        return "unsupported texture format";
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        return "incomplete formats";
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        return "wrong dimensions";
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        return "incomplete missing attachement";
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        return "no error";
    default:
        return "unknown error code";
    }
}

/**
 * \brief Adds an existing texture as a new render target for \p fb
 * \param fb the frame buffer to which to add the new texture
 * \param id render target's identifier
 * \param target used only for cubemaps, determines the face
 * of the cubemap on which to make the render, can be 0
 * \param tex the texture that is the target of \p id. must be a 2D texture
 * or a cubemap
 * \param mipmap mipmap level on which to make the render (0 is recommanded)
 * \param canfree boolean which indicates if the frame buffer \p fb has right
 * to delete \p tex
 * \returns SCE_ERROR on error, SCE_OK otherwise
 *
 * Two or more calls of this function under one frame buffer will use
 * the Multiple Render Targets OpenGL's extension.
 * After adding a texture as a color buffer, it is recommanded to
 * call SCE_RAddRenderBuffer() to add a depth buffer.
 * This function will adapts the \p fb's viewport to the dimensions of \p tex.
 * \sa SCE_RAddRenderBuffer(), SCE_RAddNewRenderTexture()
 */
int SCE_RAddRenderTexture (SCE_RFramebuffer *fb, SCE_RBufferType id,
                           SCEenum target, SCE_RTexture *tex, int mipmap,
                           int canfree)
{
    int type, status;

    /* TODO: verifier le nombre de render targets encore autorises et veiller
       a ne pas depasser SCE_RGetMaxAttachmentBuffers() */

    type = SCE_RIDToGLBuffer (id);

    /* si la precedente texture n'a pas ete envoyee par l'utilisateur,
       on la supprime */
    if (!fb->buffers[id].user)
        SCE_RDeleteTexture (fb->buffers[id].tex);

    /* si un buffer existe deja, on le supprime */
    if (fb->buffers[id].id) {
        glDeleteRenderbuffersEXT (1, &fb->buffers[id].id);
        fb->buffers[id].id = 0;
    }

    fb->buffers[id].user = !canfree;
    fb->buffers[id].tex = tex;

    /* verification du target */
    if (target < SCE_TEX_POSX || target > SCE_TEX_NEGZ)
        target = SCE_RGetTextureTarget (tex);
    else if (SCE_RGetTextureTarget (tex) != SCE_TEX_CUBE) {
        /* target designant une face de cubemap, mais la texture
           n'est pas de type cubemap : erreur */
        SCEE_Log (SCE_INVALID_ARG);
        SCEE_LogMsg ("invalid target, the texture is not a cubemap");
        return SCE_ERROR;
    }

    if (!SCE_RIsTextureUsingMipmaps (tex)) {
        SCE_RSetTextureParam (tex, GL_TEXTURE_MAX_LEVEL, 0);
        mipmap = SCE_FALSE; /* very important */
    }
    /* TODO: useless only if CTexture manager set some parameters */
    /*
    SCE_RSetTextureParam (tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    SCE_RSetTextureParam (tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SCE_RSetTextureParam (tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    SCE_RSetTextureParam (tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    */
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, fb->id);
    /* TODO: add texture arrays, texture rectangle & 3D textures managment */
#if 0
    if (target == SCE_TEX_RECTANGLE_NV)
    {

    }
    else if (target == SCE_TEX_2D_ARRAY || target == SCE_TEX_3D)
        /* SCE_RIsTextureArray (tex) */
    {
        glFramebufferTextureLayerEXT (GL_FRAMEBUFFER_EXT, type, tex->id,
                                      mipmap, layer);
    }
    else
#endif
    /* et on lui assigne notre jolie render texture */
    glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, type,
                               target, tex->id, mipmap);
    /* si aucune color render texture n'existe, on desactive le tampon */
    if (id == SCE_DEPTH_BUFFER &&
        !fb->buffers[SCE_COLOR_BUFFER0].tex &&
        !fb->buffers[SCE_COLOR_BUFFER1].tex &&
        !fb->buffers[SCE_COLOR_BUFFER2].tex) { /* etc. */
        glDrawBuffer (GL_NONE);
    }
    glReadBuffer (GL_NONE);
    /* TODO: wtf iz dat */
    /*status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);*/
    status = glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        SCEE_Log (status);
        SCEE_LogMsg ("framebuffer check failed: %s",
                       SCE_RGetFramebufferError (status));
        return SCE_ERROR;
    }

    fb->w = SCE_RGetTextureWidth (tex, target, 0);
    fb->h = SCE_RGetTextureHeight (tex, target, 0);
    fb->buffers[id].activated = SCE_TRUE;
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

    return SCE_OK;
}


/**
 * \brief Adds a new render buffer
 * \param fb the frame buffer to which to add the render buffer
 * \param id render buffer's identifier
 * \param fmt the format of the new render buffer, can be SCE_IMAGE_NONE
 * \param w width of the new render buffer
 * \param h height of the new render buffer
 * \note If you called SCE_RAddRenderTexture() previously, \p w
 * and \p h can be set to 0 then the dimensions of the render
 * buffer are automatically set to those of the texture passed to
 * SCE_RAddRenderTexture().
 * \note If you set \p fmt at less than 0, an adapted format is used
 * automatically.
 * \sa SCE_RAddRenderTexture()
 */
int SCE_RAddRenderBuffer (SCE_RFramebuffer *fb, SCE_RBufferType id,
                          SCE_EImageFormat fmt, int w, int h)
{
    int type, status;

    type = SCE_RIDToGLBuffer (id);

    /* assignation des valeurs par defaut */
    if (fmt == SCE_IMAGE_NONE) {
        if (id == SCE_DEPTH_BUFFER)
            fmt = SCE_IMAGE_DEPTH;
        else if (id == SCE_STENCIL_BUFFER)
            fmt = /* GL_STENCIL_INDEX8_EXT */SCE_IMAGE_RGB; /* TODO: lol */
    }

    if (w <= 0)
        w = fb->w;
    else
        fb->w = w;
    if (h <= 0)
        h = fb->h;
    else
        fb->h = h;

    /* si une texture existait deja, on la supprime */
    if (!fb->buffers[id].user) {
        SCE_RDeleteTexture (fb->buffers[id].tex);
        fb->buffers[id].tex = NULL;  /* ! important ! */
    }

    /* I guess that it can be achieved later
       (like just before FramebufferRenderbuffer()) */
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, fb->id);

    /* si le buffer n'existe pas, il faut le creer */
    if (!glIsRenderbufferEXT (fb->buffers[id].id))
        glGenRenderbuffersEXT (1, &fb->buffers[id].id);

    /* creation du render buffer */
    glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, fb->buffers[id].id);
    glRenderbufferStorageEXT (GL_RENDERBUFFER_EXT,
                              SCE_RSCEImgFormatToGL (fmt), w, h);
    glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, 0);

    /* on l'ajoute au FBO */
    glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, type, GL_RENDERBUFFER_EXT,
                                  fb->buffers[id].id);

    status = glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        SCEE_Log (status);
        SCEE_LogMsg ("framebuffer check failed: %s",
                       SCE_RGetFramebufferError (status));
        return SCE_ERROR;
    }

    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
    fb->buffers[id].activated = SCE_TRUE;

    return SCE_OK;
}


/**
 * \brief Creates a new render texture and add it as a new render target to the
 * given frame buffer
 * \param fb the frame buffer to which to add the new render texture
 * \param id render target's identifier
 * \param pxf internal pixel format of the new texture, can be SCE_PXF_NONE
 * \param fmt format of the new texture, can be SCE_IMAGE_NONE
 * \param type data type of the new texture (SCE_UNSIGNED_BYTE, ...)
 * can be SCE_NONE_TYPE
 * \param w width of the new texture
 * \param h height of the new texture
 * \returns the new texture, or NULL on error. Note that the returned texture
 * does not belong to the framebuffer \p fb so you'll have to handle it yourself
 * If \p pxf and/or \p fmt is set to SCE_PXF_NONE/SCE_IMAGE_NONE then an
 * adapted value is automatically set.
 * This function makes only a call of SCE_RAddRenderTexture() like this:
 * SCE_RAddRenderTexture (\p fb, \p id, SCE_TEX_2D, newtex, 0, SCE_FALSE)
 * where 'newtex' is the new texture created from the given informations.
 * \sa SCE_RAddRenderTexture()
 */
SCE_RTexture*
SCE_RAddNewRenderTexture (SCE_RFramebuffer *fb, SCE_RBufferType id,
                          SCE_EPixelFormat pxf, SCE_EImageFormat fmt,
                          SCE_EType type, int w, int h)
{
    SCE_RTexture *tex = NULL;
    SCE_STexData data;

    if (!(tex = SCE_RCreateTexture (SCE_TEX_2D))) {
        SCEE_LogSrc ();
        return NULL;
    }

    SCE_TexData_Init (&data);

    if (pxf == SCE_PXF_NONE) {
        if (id == SCE_DEPTH_BUFFER)
            pxf = SCE_PXF_DEPTH24;
        else
            pxf = SCE_PXF_RGBA;
    }
    if (fmt == SCE_PXF_NONE) {
        if (id == SCE_DEPTH_BUFFER)
            fmt = SCE_IMAGE_DEPTH;
        else
            fmt = SCE_IMAGE_RGBA;
    }
    if (type == SCE_NONE_TYPE)
        type = SCE_UNSIGNED_BYTE;

    SCE_TexData_SetDimensions (&data, w, h, 0);
    SCE_TexData_SetDataType (&data, type);
    SCE_TexData_SetPixelFormat (&data, pxf);
    SCE_TexData_SetDataFormat (&data, fmt);

    if (!SCE_RAddTextureTexDataDup (tex, 0, &data)) {
        SCEE_LogSrc ();
        SCE_RDeleteTexture (tex);
        return NULL;
    }
    if (SCE_RBuildTexture (tex, 0, 0) < 0) {
        SCEE_LogSrc ();
        SCE_RDeleteTexture (tex);
        return NULL;
    }

    SCE_RAddRenderTexture (fb, id, SCE_TEX_2D, tex, 0, SCE_FALSE);

    return tex;
}

/**
 * \brief Gets one of the render textures of a frame buffer
 * \param fb the frame buffer to which get the texture
 * \param id the render target's identifier
 * \returns the render texture targeted by \p id
 */
SCE_RTexture* SCE_RGetRenderTexture (SCE_RFramebuffer *fb, SCE_RBufferType id)
{
    return fb->buffers[id].tex;
}


/**
 * \brief Activate a render buffer
 * \param fb a framebuffer
 * \param id the render target's identifier
 * \param activated boolean which indicates the status of \p id
 */
void SCE_RActivateRenderBuffer (SCE_RFramebuffer *fb, SCE_RBufferType id,
                                int activated)
{
    fb->buffers[id].activated = activated;
}
/**
 * \brief Equivalent to SCE_RActivateRenderBuffer (\p fb, \p id, SCE_TRUE)
 * \sa SCE_RActivateRenderBuffer() SCE_RDisableRenderBuffer()
 */
void SCE_REnableRenderBuffer (SCE_RFramebuffer *fb, SCE_RBufferType id)
{
    fb->buffers[id].activated = SCE_TRUE;
}
/**
 * \brief Equivalent to SCE_RActivateRenderBuffer (\p fb, \p id, SCE_FALSE)
 * \sa SCE_RActivateRenderBuffer() SCE_REnableRenderBuffer()
 */
void SCE_RDisableRenderBuffer (SCE_RFramebuffer *fb, SCE_RBufferType id)
{
    fb->buffers[id].activated = SCE_FALSE;
}

/**
 * \brief Gets the maximum buffers supported for multiple render targets
 */
unsigned int SCE_RGetMaxAttachmentBuffers (void)
{
    return max_attachement_buffers;
}

static void SCE_RSetDrawBuffers (SCE_RFramebuffer *fb)
{
    unsigned int i, j = 0;
    SCEenum drawids[SCE_MAX_ATTACHMENT_BUFFERS] = {0};

    for (i = 0; i < max_attachement_buffers; i++) {
        if (fb->buffers[i].activated)
            drawids[j++] = GL_COLOR_ATTACHMENT0_EXT + i;
    }
    glDrawBuffers (j, drawids);
}

/**
 * \brief Using a frame buffer instead of the default OpenGL's render buffer.
 * If \p fb is not NULL, then \p fb is the target of all the further renders,
 * otherwise the default OpenGL's render buffer is positioned back
 * \param fb the frame buffer on make the further renders,
 * set as NULL to set up the default OpenGL's render buffer
 * \param r gives the new viewport for the renders, can be NULL
 *
 * Calling this function when none frame buffer is used saves the viewport,
 * then the first call of this function with a null frame buffer will restore
 * the viewport.
 */
void SCE_RUseFramebuffer (SCE_RFramebuffer *fb, SCE_SIntRect *r)
{
    static int viewport[4];        /* save previous viewport */
    static int bound = SCE_FALSE;

    if (fb) {
        int *p = SCE_Rectangle_GetBottomLeftPoint (r);
        /* on recupere d'abord le viewport, pour ensuite le restituer
           (uniquement si aucun fbo n'est deja binde,
            sinon on prendrait son viewport) */
        if (!bound)
            glGetIntegerv (GL_VIEWPORT, viewport);

        glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, fb->id);
        if (r)
            glViewport (p[0], p[1], SCE_Rectangle_GetWidth (r),
                        SCE_Rectangle_GetHeight (r));
        else
            glViewport (fb->x, fb->y, fb->w, fb->h);

        if (SCE_RHasCap (SCE_MRT))
            SCE_RSetDrawBuffers (fb);

        bound = SCE_TRUE;
    } else if (bound) {
        glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
        glViewport (viewport[0], viewport[1], viewport[2], viewport[3]);
        bound = SCE_FALSE;
    }
}

/** @} */
