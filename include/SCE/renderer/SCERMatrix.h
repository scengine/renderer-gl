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
 
/* Cree le : 10 janvier 2007
   derniere modification le 02/07/2007 */

#ifndef SCERMATRIX_H
#define SCERMATRIX_H

#include <SCE/utils/SCEUtils.h>
#include "SCE/renderer/SCERType.h"

#ifdef __cplusplus
extern "C" {
#endif

/* constante symbolique representant la matrice active */
#define SCE_RURRENT_MATRIX 0

/* charge la matrice d'identite */
void SCE_RLoadIdentityMatrix (void);

void SCE_RPushMatrix (void);
void SCE_RPopMatrix (void);

/* selectionne la matrice specifiee en argument comme matrice active */
void SCE_RSetActiveMatrix (SCEenum);
/* renvoie le mode de la matrice active */
GLint SCE_RGetActiveMatrix (void);

/* multiplie la matrice active par celle specifiee */
void SCE_RMultMatrix (const float*);
/* charge la matrice specifiee */
void SCE_RLoadMatrix (const float*);

/* retourne la matrice demandee */
void SCE_RGetMatrix (int, float*);


/* translation */
void SCE_RTranslateMatrix (float, float, float) SCE_GNUC_DEPRECATED;

/* rotation */
void SCE_RRotateMatrix (float, float, float, float) SCE_GNUC_DEPRECATED;

/* echelle */
void SCE_RScaleMatrix (float, float, float) SCE_GNUC_DEPRECATED;

/* versions vectorielles */
void SCE_RTranslateMatrixv (float*) SCE_GNUC_DEPRECATED;
void SCE_RRotateMatrixv (float, float*) SCE_GNUC_DEPRECATED;
void SCE_RScaleMatrixv (float*) SCE_GNUC_DEPRECATED;
/***/

#if 0
/* definit un point de vue, une position et un axe vertical */
void SCE_RLook3D (
  float, float, float,
  float, float, float,
  float, float, float);

/* version vectorielle */
void SCE_RLook3Dv (const float*, const float*, const float*);
#endif

/* cree une matrice de frustum de vision en perspective */
void SCE_RSetFrustumMatrix (
  GLdouble, GLdouble,
  GLdouble, GLdouble,
  GLdouble, GLdouble);

/* cree une matrice de projection en perspective cavaliere */
void SCE_RSetOrthoMatrix (
  GLdouble, GLdouble,
  GLdouble, GLdouble,
  GLdouble, GLdouble);

/* definit le cadrage actif dans lequel dessiner */
void SCE_RViewport (int, int, int, int);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
