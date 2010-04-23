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
 
/* Cree le : 24/03/2008
   derniere modification : 24/03/2008 */

#ifndef SCERPOINTSPRITE_H
#define SCERPOINTSPRITE_H

#include <SCE/utils/SCEUtils.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sce_rpointsprite SCE_RPointSprite;
struct sce_rpointsprite
{
    float size;    /* taille du point */
    int smooth;    /* point lisse ou non (booleen) */
    int textured;  /* point texture ou non */
    float att[3];  /* facteurs d'attenuation du point */
    float minsize; /* tailles maximales et minimales du point */
    float maxsize;
};

/* initialise le gestionnaire */
int SCE_RPointSpriteInit (void);

/* initialise une sructure */
void SCE_RInitPointSprite (SCE_RPointSprite*);

/* cree un point sprite */
SCE_RPointSprite* SCE_RCreatePointSprite (void);
/* supprime un point sprite */
void SCE_RDeletePointSprite (SCE_RPointSprite*);

/* defini la taille d'un point */
void SCE_RSetPointSpriteSize (SCE_RPointSprite*, float);
float SCE_RGetPointSpriteSize (SCE_RPointSprite*);

/* defini si un point doit etre lisse */
void SCE_RSmoothPointSprite (SCE_RPointSprite*, int);
int SCE_RIsPointSpriteSmoothed (SCE_RPointSprite*);

/* defini si on utilise une texture sur les points */
void SCE_RActivatePointSpriteTexture (SCE_RPointSprite*, int);
void SCE_REnablePointSpriteTexture (SCE_RPointSprite*);
void SCE_RDisablePointSpriteTexture (SCE_RPointSprite*);

/* defini les attenuations de taille d'un point */
void SCE_RSetPointSpriteAttenuations (SCE_RPointSprite*, float, float, float);
void SCE_RSetPointSpriteAttenuationsv (SCE_RPointSprite*, SCE_TVector3);

/* defini les parametres d'un point sprites comme actifs
   pour les prochains rendus de points */
void SCE_RUsePointSprite (SCE_RPointSprite*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
