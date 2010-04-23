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
 
/* created: 23/03/2008
   updated: 07/04/2008 */

#include <SCE/utils/SCEUtils.h>
#include "SCE/renderer/SCEGLee.h"
#include "SCE/renderer/SCERType.h"
#include "SCE/renderer/SCERSupport.h"
#include "SCE/renderer/SCEROcclusionQuery.h"

/**
 * \file SCECOcclusionQuery.c
 * \copydoc occlusionquery
 * \brief Occlusion queries managment
 * 
 * \file SCECOcclusionQuery.h
 * \copydoc occlusionquery
 * \brief Occlusion queries managment
 */

/**
 * \defgroup occlusionquery Occlusion queries managment
 * \ingroup core
 * \brief Occlusion queries managment
 */

/** @{ */

static SCEuint queryid = 0;
static int drawpixels = 0;
static int funcid = 1;

typedef void (*SCE_FOcclusionQueryFunc)(int);
static void SCE_ROcclusionQueryCallback (int);
static void SCE_ROcclusionQueryNull (int);

static SCE_FOcclusionQueryFunc funcs[] =
{
    SCE_ROcclusionQueryCallback, SCE_ROcclusionQueryNull
};

/**
 * \internal
 */
int SCE_ROcclusionQueryInit (void)
{
    if (SCE_RHasCap (SCE_OCCLUSION_QUERY)) {
        glGenQueries (1, &queryid);
        funcid = 0;
    } else
        funcid = 1;
    return SCE_OK;
}
/**
 * \internal
 */
void SCE_ROcclusionQueryQuit (void)
{
    if (SCE_RHasCap (SCE_OCCLUSION_QUERY))
        glDeleteQueries (1, &queryid);
    queryid = 0; drawpixels = 0;
}

static void SCE_ROcclusionQueryCallback (int begin)
{
    if (begin)
        glBeginQuery (GL_SAMPLES_PASSED, queryid);
    else {
        glEndQuery (GL_SAMPLES_PASSED);
        glGetQueryObjectiv (queryid, GL_QUERY_RESULT, &drawpixels);
    }
}
/**
 * \internal
 * \brief Dummy callback to use if occlusion queries aren't supported by the
 *        hardware
 */
static void SCE_ROcclusionQueryNull (int begin)
{
}

/**
 * \brief Starts or ends an occulsion query
 * \param begin SCE_TRUE to begin an occulsion query or SCE_FALSE to end the
 *        current one
 * 
 * \note This function doesn't do anything if occlusion queries aren't supported
 *       in hardware.
 * \note You can check if the hardware supports occlusion queries by calling
 *       SCE_RHasCap(SCE_OCCLUSION_QUERY)
 * 
 * \see SCE_RHasCap()
 */
void SCE_ROcclusionQuery (int begin)
{
    funcs[funcid] (begin);
}

/**
 * \brief Gets result of an occlusion query
 * \returns the number of drawn pixels
 * 
 * \note This function always returns 0 if occlusion queries aren't supported by
 *       in hardware.
 * \note You can check if the hardware supports occlusion queries by calling
 *       SCE_RHasCap(SCE_OCCLUSION_QUERY)
 * 
 * \see SCE_ROcclusionQuery()
 * \see SCE_RHasCap()
 */
int SCE_RGetQueryResult (void)
{
    return drawpixels;
}

/** @} */
