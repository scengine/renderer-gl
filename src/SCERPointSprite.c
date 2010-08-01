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
 
/* created: 24/03/2008
   updated: 13/05/2010 */

#include <GL/glew.h>
#include <SCE/utils/SCEUtils.h>

#include "SCE/renderer/SCERPointSprite.h"

#if 0
/* tampon d'un simple point */
static SCE_RVertexBuffer *vb = NULL;
static SCE_TVector3 pos = {0., 0., 0.};

/* ajoute le 24/03/2008 */
int SCE_RPointSpriteInit (void)
{
    SCE_RVertexDeclaration dec;
#define SCE_POINTASSERT(c)\
    if (c) {\
        SCE_RDeleteVertexBuffer (vb);\
        SCEE_LogSrc ();\
        return SCE_ERROR;\
    }
    SCE_POINTASSERT (!(vb = SCE_RCreateVertexBuffer ()))
    SCE_POINTASSERT (SCE_RAddVertexBufferData (vb, 3 * sizeof *pos, pos) < 0)
    SCE_POINTASSERT (SCE_RAddVertexDeclaration (vb, &dec) < 0)
    SCE_RInitVertexDeclaration (&dec);
    dec.attrib = SCE_POSITION;
    dec.type = SCE_FLOAT;
    dec.size = 3;
    dec.active = SCE_TRUE;
    SCE_RBuildVertexBuffer (vb, GL_STREAM_DRAW);
    return SCE_OK;
}
#endif

void SCE_RInitPointSprite (SCE_RPointSprite *point)
{
    point->size = 1.0f;
    point->smooth = SCE_FALSE;
    point->textured = SCE_FALSE;
    point->att[0] = 1.0f;
    point->att[1] = point->att[2] = 0.0f;
    point->minsize = 1.0f;
    point->maxsize = 64.0f;     /* TODO: check the limit */
}

SCE_RPointSprite* SCE_RCreatePointSprite (void)
{
    SCE_RPointSprite *point = NULL;
    if (!(point = SCE_malloc (sizeof *point)))
        SCEE_LogSrc ();
    else
        SCE_RInitPointSprite (point);
    return point;
}

void SCE_RDeletePointSprite (SCE_RPointSprite *point)
{
    SCE_free (point);
}


void SCE_RSetPointSpriteSize (SCE_RPointSprite *point, float size)
{
    point->size = size;
}
float SCE_RGetPointSpriteSize (SCE_RPointSprite *point)
{
    return point->size;
}

void SCE_RSmoothPointSprite (SCE_RPointSprite *point, int smooth)
{
    point->smooth = smooth;
}
int SCE_RIsPointSpriteSmoothed (SCE_RPointSprite *point)
{
    return point->smooth;
}

void SCE_RActivatePointSpriteTexture (SCE_RPointSprite *point, int activated)
{
    point->textured = activated;
}
void SCE_REnablePointSpriteTexture (SCE_RPointSprite *point)
{
    point->textured = SCE_TRUE;
}
void SCE_RDisablePointSpriteTexture (SCE_RPointSprite *point)
{
    point->textured = SCE_FALSE;
}

void SCE_RSetPointSpriteAttenuations (SCE_RPointSprite *point,
                                      float a, float b, float c)
{
    point->att[0] = a; point->att[1] = b; point->att[2] = c;
}
void SCE_RSetPointSpriteAttenuationsv (SCE_RPointSprite *point, SCE_TVector3 at)
{
    memcpy (point->att, at, 3 * sizeof *point->att);
}


void SCE_RUsePointSprite (SCE_RPointSprite *point)
{
    if (point)
    {
        glPointSize (point->size); 
        /* TODO: manage ARB_point_parameters extensions :
           GL_ARB_point_parameters, GL_ARB_point_sprite,
           GL_EXT_point_parameters */
        glPointParameterfv (GL_POINT_DISTANCE_ATTENUATION, point->att);
        glPointParameterf (GL_POINT_SIZE_MAX, point->maxsize);
        glPointParameterf (GL_POINT_SIZE_MIN, point->minsize);

        if (point->smooth)
            glEnable (GL_POINT_SMOOTH);
        if (point->textured)
        {
            /* exige que la texture ait deja ete appliquee */
            glTexEnvf (GL_POINT_SPRITE, GL_COORD_REPLACE, SCE_TRUE);
            glEnable (GL_POINT_SPRITE);
        }
    }
    else
    {
        glTexEnvf (GL_POINT_SPRITE, GL_COORD_REPLACE, SCE_FALSE);
        glDisable (GL_POINT_SPRITE);
        glDisable (GL_POINT_SMOOTH);
    }
}
