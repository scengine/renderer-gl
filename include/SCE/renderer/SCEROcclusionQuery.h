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
 
/* Cree le : 23/03/2008
   derniere modification : 24/03/2008 */

#ifndef SCEROCCLUSIONQUERY_H
#define SCEROCCLUSIONQUERY_H

#ifdef __cplusplus
extern "C"
{
#endif

/* initialise le gestionnaire */
int SCE_ROcclusionQueryInit (void);
/* quitte le gestionnaire */
void SCE_ROcclusionQueryQuit (void);

/* fait une requete d'occlusion sur les prochains rendus
   (ou pas, selon son parametre (booleen)) */
void SCE_ROcclusionQuery (int);

/* retourne le resultat du test d'occlusion (nombre de pixel dessines) */
int SCE_RGetQueryResult (void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
