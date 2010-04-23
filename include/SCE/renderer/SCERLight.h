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

#ifndef SCERLIGHT_H
#define SCERLIGHT_H

#include <SCE/utils/SCEUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sce_rlight SCE_RLight;
struct sce_rlight {
    float color[4];   /**< Light's colors */
    SCE_TVector4 pos; /**< Light's position */
    SCE_TVector3 dir; /**< Light's spot direction */
    float angle;      /**< Light's spot angle */
    float gat;        /**< Global attenuation */
    float lat;        /**< Linear attenuation */
    float qat;        /**< Quadratic attenuation */
    int ccontrol;     /**< What is the color control ? */
};

void SCE_RInitLight (SCE_RLight*);

SCE_RLight* SCE_RCreateLight (void);
void SCE_RDeleteLight (SCE_RLight*);

void SCE_RSetLightColor (SCE_RLight*, float, float, float);
void SCE_RSetLightColorv (SCE_RLight*, float*);
float* SCE_RGetLightColor (SCE_RLight*);
void SCE_RGetLightColorv (SCE_RLight*, float*);

void SCE_RSetLightPosition (SCE_RLight*, float, float, float);
void SCE_RSetLightPositionv (SCE_RLight*, SCE_TVector3);
float* SCE_RGetLightPosition (SCE_RLight*);
void SCE_RGetLightPositionv (SCE_RLight*, SCE_TVector3);

void SCE_RInfiniteLight (SCE_RLight*, int);
int SCE_RIsInfiniteLight (SCE_RLight*);

void SCE_RSetLightIndependantSpecular (SCE_RLight*, int);
int SCE_RIsLightIndependantSpecular (SCE_RLight*);

void SCE_RSetLightDirection (SCE_RLight*, float, float, float);
void SCE_RSetLightDirectionv (SCE_RLight*, SCE_TVector3);
float* SCE_RGetLightDirection (SCE_RLight*);
void SCE_RGetLightDirectionv (SCE_RLight*, SCE_TVector3);

void SCE_RSetLightAngle (SCE_RLight*, float);
float SCE_RGetLightAngle (SCE_RLight*);

void SCE_RSetLightGlobalAtt (SCE_RLight*, float);
float SCE_RGetLightGlobalAtt (SCE_RLight*);
void SCE_RSetLightLinearAtt (SCE_RLight*, float);
float SCE_RGetLightLinearAtt (SCE_RLight*);
void SCE_RSetLightQuadraticAtt (SCE_RLight*, float);
float SCE_RGetLightQuadraticAtt (SCE_RLight*);

void SCE_RActivateLighting (int);

void SCE_RUseLight (SCE_RLight*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
