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
 
/* created: 08/03/2008
   updated: 19/10/2008 */

#include <SCE/utils/SCEUtils.h>
#include "SCE/renderer/SCERType.h"
#include "SCE/renderer/SCERLight.h"

static int use_lighting = SCE_FALSE;

void SCE_RInitLight (SCE_RLight *light)
{
    light->color[0] = light->color[1] = light->color[2] = light->color[3] =1.0f;
    SCE_Vector4_Set (light->pos, 0.0, 0.0, 0.0, 1.0);
    SCE_Vector3_Set (light->dir, 0.0, 0.0, -1.0);
    light->angle = 180.0f;
    light->gat = 1.0f;
    light->lat = 0.0f;
    light->qat = 0.0f;
    light->ccontrol = GL_SINGLE_COLOR;
}

SCE_RLight* SCE_RCreateLight (void)
{
    SCE_RLight *light = NULL;

    if (!(light = SCE_malloc (sizeof *light)))
        SCEE_LogSrc ();
    else
        SCE_RInitLight (light);
    return light;
}

void SCE_RDeleteLight (SCE_RLight *light)
{
    SCE_free (light);
}


void SCE_RSetLightColor (SCE_RLight *light, float r, float g, float b)
{
    light->color[0] = r;
    light->color[1] = g;
    light->color[2] = b;
}
void SCE_RSetLightColorv (SCE_RLight *light, float *c)
{
    memcpy (light->color, c, 3 * sizeof *c);
}
float* SCE_RGetLightColor (SCE_RLight *light)
{
    return light->color;
}
void SCE_RGetLightColorv (SCE_RLight *light, float *c)
{
    memcpy (c, light->color, 3 * sizeof *c);
}


void SCE_RSetLightPosition (SCE_RLight *light, float x, float y, float z)
{
    SCE_Vector3_Set (light->pos, x, y, z);
}
void SCE_RSetLightPositionv (SCE_RLight *light, SCE_TVector3 pos)
{
    SCE_Vector3_Copy (light->pos, pos);
}
float* SCE_RGetLightPosition (SCE_RLight *light)
{
    return light->pos;
}
void SCE_RGetLightPositionv (SCE_RLight *light, SCE_TVector3 pos)
{
    SCE_Vector3_Copy (pos, light->pos);
}

void SCE_RInfiniteLight (SCE_RLight *light, int inf)
{
    light->pos[3] = (inf ? 0.0f : 1.0f);
}
int SCE_RIsInfiniteLight (SCE_RLight *light)
{
    return (light->pos[3] == 0.0f); /* TODO: bad */
}

void SCE_RSetLightIndependantSpecular (SCE_RLight *light, int separate)
{
    if (separate)
        light->ccontrol = GL_SEPARATE_SPECULAR_COLOR;
    else
        light->ccontrol = GL_SINGLE_COLOR;
}
int SCE_RIsLightIndependantSpecular (SCE_RLight *light)
{
    return (!(light->ccontrol == GL_SINGLE_COLOR));
}

void SCE_RSetLightDirection (SCE_RLight *light, float x, float y, float z)
{
    SCE_Vector3_Set (light->dir, x, y, z);
}
void SCE_RSetLightDirectionv (SCE_RLight *light, SCE_TVector3 dir)
{
    SCE_Vector3_Copy (light->dir, dir);
}
float* SCE_RGetLightDirection (SCE_RLight *light)
{
    return light->dir;
}
void SCE_RGetLightDirectionv (SCE_RLight *light, SCE_TVector3 dir)
{
    SCE_Vector3_Copy (dir, light->dir);
}

void SCE_RSetLightAngle (SCE_RLight *light, float angle)
{
    light->angle = angle;
}
float SCE_RGetLightAngle (SCE_RLight *light)
{
    return light->angle;
}

void SCE_RSetLightGlobalAtt (SCE_RLight *light, float att)
{
    light->gat = att;
}
float SCE_RGetLightGlobalAtt (SCE_RLight *light)
{
    return light->gat;
}
void SCE_RSetLightLinearAtt (SCE_RLight *light, float att)
{
    light->lat = att;
}
float SCE_RGetLightLinearAtt (SCE_RLight *light)
{
    return light->lat;
}
void SCE_RSetLightQuadraticAtt (SCE_RLight *light, float att)
{
    light->qat = att;
}
float SCE_RGetLightQuadraticAtt (SCE_RLight *light)
{
    return light->qat;
}


void SCE_RActivateLighting (int activated)
{
    use_lighting = activated;
    SCE_RSetState (GL_LIGHTING, activated);
}

void SCE_RUseLight (SCE_RLight *light)
{
    static unsigned int num = 0;

    if (light && use_lighting) {
        if (num < SCE_RGetInteger (GL_MAX_LIGHTS)) {
            glEnable  (GL_LIGHT0+num);
            glLightfv (GL_LIGHT0+num, GL_POSITION, light->pos);
            glLightfv (GL_LIGHT0+num, GL_DIFFUSE,  light->color);
            glLightfv (GL_LIGHT0+num, GL_SPECULAR, light->color);

            glLightfv (GL_LIGHT0+num, GL_SPOT_DIRECTION, light->dir);
            glLightf  (GL_LIGHT0+num, GL_SPOT_CUTOFF,    light->angle);

            glLightf (GL_LIGHT0+num, GL_CONSTANT_ATTENUATION,  light->gat);
            glLightf (GL_LIGHT0+num, GL_LINEAR_ATTENUATION,    light->lat);
            glLightf (GL_LIGHT0+num, GL_QUADRATIC_ATTENUATION, light->qat);

            glLightModeli (GL_LIGHT_MODEL_COLOR_CONTROL, light->ccontrol);
            /* force good specular computation */
            glLightModeli (GL_LIGHT_MODEL_LOCAL_VIEWER, SCE_TRUE);
            num++;
        }
    } else while (num > 0)
        glDisable (GL_LIGHT0 + (--num));
}
