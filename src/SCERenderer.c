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
 
/* created: 15/12/2006
   updated: 03/04/2012 */

#include <pthread.h>
#include "SCE/renderer/SCERenderer.h"

/**
 * \file SCERenderer.c
 * \copydoc renderer
 * 
 * \file SCERenderer.h
 * \copydoc renderer
 */

/**
 * \defgroup renderer Renderer, makes 3D API calls
 * \ingroup SCEngine
 * @{
 */

static pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
static unsigned int init_n = 0;


static int SCE_RInitGlew (void)
{
    SCEenum err;
    err = glewInit ();
    if (err != GLEW_OK) {
        SCEE_Log (6724);
        SCEE_LogMsg ("failed to initialize GLEW: %s", glewGetErrorString (err));
        return SCE_ERROR;
    }
    return SCE_OK;
}

int SCE_RInit (FILE *outlog, SCEbitfield flags)
{
    int ret = SCE_OK;
    if (pthread_mutex_lock (&init_mutex) != 0) {
        SCE_Init_Error (stderr); /* hm. */
        SCEE_Log (42);
        SCEE_LogMsg ("failed to lock initialization mutex");
        return SCE_ERROR;
    }
    init_n++;
    if (init_n == 1) {
        if (SCE_Init_Core (outlog, flags) < 0 ||
            SCE_RInitGlew () < 0 ||
            SCE_RTypeInit () < 0 ||
            SCE_RSupportInit () < 0 ||
            SCE_RBufferInit () < 0 ||
            SCE_RVertexArrayInit () < 0 ||
            SCE_RTextureInit () < 0 ||
            SCE_RFramebufferInit () < 0 ||
            SCE_RShaderInit () < 0 ||
            SCE_ROcclusionQueryInit () < 0) {
            ret = SCE_ERROR;
        } else {
            SCE_RUseMaterial (NULL);

            /* enabling some default states */
            /* TODO: it sucks and it is no longer in the core profile */
            glEnable (GL_NORMALIZE);
            glEnable (GL_CULL_FACE);
            glEnable (GL_DEPTH_TEST);
            ret = SCE_OK;
        }
    }
    pthread_mutex_unlock (&init_mutex);
    if (ret == SCE_ERROR) {
        SCE_RQuit ();
        SCEE_LogSrc ();
        SCEE_LogSrcMsg ("failed to initialize the GL renderer");
    }
    return ret;
}

void SCE_RQuit (void)
{
    if (pthread_mutex_lock (&init_mutex) != 0) {
        SCEE_Log (42);
        SCEE_LogMsg ("failed to lock initialization mutex");
    } else {
        init_n--;
        if (init_n < 0) {
            init_n = 0;         /* user made an useless call */
        } else if (init_n == 0) {
            SCE_ROcclusionQueryQuit ();
            SCE_RShaderQuit ();
            SCE_RFramebufferQuit ();
            SCE_RTextureQuit ();
            SCE_RVertexArrayQuit ();
            SCE_RBufferQuit ();
            SCE_RSupportQuit ();
            SCE_Quit_Core ();
        }
        pthread_mutex_unlock (&init_mutex);
    }
}

const char* SCE_RGetError (void)
{
    SCEenum err = glGetError ();

    switch (err) {
    case GL_NO_ERROR: return NULL;
    case GL_INVALID_ENUM: return "invalid enum";
    case GL_INVALID_VALUE: return "invalid value";
    case GL_INVALID_OPERATION: return "invalid operation";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "invalid framebuffer operation";
    case GL_OUT_OF_MEMORY: return "out of memory";
    }

    return "mysterious error happened";
}

/**
 * \brief Defines the color used for cleared the color buffer
 * \todo move these functions somewhere else
 */
void SCE_RClearColor (float r, float g, float b, float a)
{
    glClearColor (r, g, b, a);
}
/**
 * \brief Defines the signed integer color used for cleared the color buffer
 */
void SCE_RClearColori (int r, int g, int b, int a)
{
    glClearColorIiEXT (r, g, b, a);
}
/**
 * \brief Defines the unsigned integer color used for cleared the color buffer
 */
void SCE_RClearColorui (SCEuint r, SCEuint g, SCEuint b, SCEuint a)
{
    glClearColorIuiEXT (r, g, b, a);
}
/**
 * \brief Defines the value used for cleared the depth buffer
 */
void SCE_RClearDepth (float d)
{
    glClearDepth (d);
}
void SCE_RClearStencil (SCEuint s)
{
    glClearStencil (s);
}

/**
 * \brief Clears the specified buffers
 *
 * This function calls glClear(\p mask).
 */
void SCE_RClear (const GLbitfield mask)
{
    glClear (mask);
}

void SCE_RFlush (void)
{
    glFlush ();
}


void SCE_RSetState (SCEenum state, int active)
{
    if (active)
        glEnable (state);
    else
        glDisable (state);
}

void SCE_RSetState2 (SCEenum state, SCEenum state2, int active)
{
    if (active) {
        glEnable (state);
        glEnable (state2);
    } else {
        glDisable (state);
        glDisable (state2);
    }
}

void SCE_RSetState3 (SCEenum state, SCEenum state2, SCEenum state3, int active)
{
    if (active) {
        glEnable (state);
        glEnable (state2);
        glEnable (state3);
    } else {
        glDisable (state);
        glDisable (state2);
        glDisable (state3);
    }
}

void SCE_RSetState4 (SCEenum state, SCEenum state2,
                     SCEenum state3, SCEenum state4, int active)
{
    if (active) {
        glEnable (state);
        glEnable (state2);
        glEnable (state3);
        glEnable (state4);
    } else {
        glDisable (state);
        glDisable (state2);
        glDisable (state3);
        glDisable (state4);
    }
}

void SCE_RSetState5 (SCEenum state, SCEenum state2, SCEenum state3,
                     SCEenum state4, SCEenum state5, int active)
{
    if (active) {
        glEnable (state);
        glEnable (state2);
        glEnable (state3);
        glEnable (state4);
        glEnable (state5);
    } else {
        glDisable (state);
        glDisable (state2);
        glDisable (state3);
        glDisable (state4);
        glDisable (state5);
    }
}

void SCE_RSetState6 (SCEenum state, SCEenum state2, SCEenum state3,
                     SCEenum state4, SCEenum state5, SCEenum state6, int active)
{
    if (active) {
        glEnable (state);
        glEnable (state2);
        glEnable (state3);
        glEnable (state4);
        glEnable (state5);
        glEnable (state6);
    } else {
        glDisable (state);
        glDisable (state2);
        glDisable (state3);
        glDisable (state4);
        glDisable (state5);
        glDisable (state6);
    }
}

void SCE_RSetBlending (SCEenum src, SCEenum dst)
{
    glBlendFunc (src, dst);
}
void SCE_RSetAlphaFunc (SCEenum func, float ref)
{
    glAlphaFunc (func, ref);
}

/**
 * \brief Enables depth test
 */
void SCE_REnableDepthTest (void)
{
    glEnable (GL_DEPTH_TEST);
}
/**
 * \brief Disables depth test
 */
void SCE_RDisableDepthTest (void)
{
    glDisable (GL_DEPTH_TEST);
}
/**
 * \brief Enables stencil test
 */
void SCE_REnableStencilTest (void)
{
    glEnable (GL_STENCIL_TEST);
}
/**
 * \brief Disables stencil test
 */
void SCE_RDisableStencilTest (void)
{
    glDisable (GL_STENCIL_TEST);
}


/**
 * Enables/Disables color buffer writing
 * \param a can be SCE_TRUE or SCE_FALSE
 */
void SCE_RActivateColorBuffer (int a)
{
    glColorMask (a, a, a, a);
}
/**
 * Enables/Disables depth buffer writing
 * \param a can be SCE_TRUE or SCE_FALSE
 */
void SCE_RActivateDepthBuffer (int a)
{
    glDepthMask (a);
}
/**
 * \brief Scale depth values in the depth buffer
 * \param a minimum
 * \param b maximum
 * Given an input depth d in [0, 1], the final depth will be
 * \p a + d * (\p b - \p a)
 * This function calls glDepthFunc(\p a, \p b).
 * \sa SCE_RSetValidPixels(), SCE_RActivateDepthBuffer()
 */
void SCE_RDepthRange (SCEdouble a, SCEdouble b)
{
    glDepthRange (a, b);
}

/**
 * \brief Defines which faces will be culled by the backface culling
 * \param mode can be SCE_FRONT or SCE_BACK
 *
 * This function calls glCullFace(\p mode).
 */
void SCE_RSetCulledFaces (SCEenum mode)
{
    glCullFace (mode);
}

/**
 * \brief Defines which faces will be culled by the backface culling
 * \param mode can be SCE_LESS, SCE_LEQUAL, SCE_GREATER, SCE_GEQUAL
 * or SCE_NOTEQUAL
 *
 * This function calls glDepthFunc(\p mode).
 * \sa SCE_RDepthRange()
 */
void SCE_RSetValidPixels (SCEenum mode)
{
    glDepthFunc (mode);
}

/**
 * \brief Binding of glStencilOp()
 * \sa SCE_RSetStencilFunc()
 */
void SCE_RSetStencilOp (SCEenum sfail, SCEenum dpfail, SCEenum dppass)
{
    glStencilOp (sfail, dpfail, dppass);
}
/**
 * \brief Binding of glStencilFunc()
 * \sa SCE_RSetStencilOp()
 */
void SCE_RSetStencilFunc (SCEenum func, SCEuint ref, SCEuint mask)
{
    glStencilFunc (func, ref, mask);
}

void SCE_RVertexAttrib4fv (SCEuint index, const SCEfloat *data)
{
    glVertexAttrib4fv (index, data);
}

/** @} */
