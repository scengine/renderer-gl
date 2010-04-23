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
 
/* Cree le : 31 mai 2007
   derniere modification : 21/02/2008 */

#ifndef SCERMATERIAL_H
#define SCERMATERIAL_H

#include "SCE/renderer/SCERType.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* types de couleurs */
#define SCE_DIFFUSE_COLOR 0
#define SCE_EMISSIVE_COLOR 1
#define SCE_SPECULAR_COLOR 2
#define SCE_AMBIENT_COLOR 3
#define SCE_SHININESS_COLOR 4
/* nombre de types de couleur */
#define SCE_NUM_COLORS 5


typedef struct sce_rmaterial SCE_RMaterial;
struct sce_rmaterial
{
    float colors[SCE_NUM_COLORS][4]; /* couleurs du materiau */
    int use_blending;                /* utilisation du blending ? */
    SCEenum src, dst;                /* source et destination du blending */
};


/* cree un materiau */
SCE_RMaterial* SCE_RCreateMaterial (void);
/* supprime un materiau */
void SCE_RDeleteMaterial (SCE_RMaterial*);

/* assigne une couleur a un materiau */
void SCE_RSetMaterialColor (SCE_RMaterial*, SCEenum, float, float, float, float);
/* version vectorielle */
void SCE_RSetMaterialColorv (SCE_RMaterial*, SCEenum, float*);
/* renvoie le tableau de la couleur specifie */
float* SCE_RGetMaterialColor (SCE_RMaterial*, SCEenum);
void SCE_RGetMaterialColorv (SCE_RMaterial*, SCEenum, float*);

/* active/desactive le blending */
void SCE_RActiveMaterialBlending (SCE_RMaterial*, int);
void SCE_REnableMaterialBlending (SCE_RMaterial*);
void SCE_RDisableMaterialBlending (SCE_RMaterial*);

/* defini le mode de blending */
void SCE_RSetMaterialBlending (SCE_RMaterial*, SCEenum, SCEenum);

/* defini le materiau actif */
void SCE_RUseMaterial (SCE_RMaterial*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
