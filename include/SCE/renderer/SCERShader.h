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
 
/* created: 11/02/2007
   updated: 14/03/2008 */

#ifndef SCERSHADER_H
#define SCERSHADER_H

#include "SCE/renderer/SCERType.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constantes propres a CShader */
#define SCE_VERTEX_SHADER 1
#define SCE_PIXEL_SHADER 2

/* structure d'un shader GLSL */
typedef struct sce_rshaderglsl SCE_RShaderGLSL;
struct sce_rshaderglsl
{
    SCEuint id;     /* identifiant opengl */
    SCEenum type;   /* type du shader (vertex ou fragment) */
    SCEchar *data;  /* donnes */
    int is_pixelshader;
    int compiled;
};

#ifdef SCE_USE_CG
/* structure d'un shader Cg */
typedef struct sce_rshadercg SCE_RShaderCG;
struct sce_rshadercg
{
    CGprogram id;   /* identifiant cg */
    CGprofile type; /* type du shader (vertex / fragment), ou 'profile' pour Cg */
    char *data;     /* donnes */
    char **args;    /* arguments du shader */
    int is_pixelshader;
    int compiled;
};
#endif


/* structure d'un programme regroupant des shaders GLSL */
typedef struct sce_rprogram SCE_RProgram;
struct sce_rprogram
{
    GLuint id;    /* identifiant opengl */

    /* booleen indiquant si le programme a ete lie/relie
       depuis la dernire assignation de shader au program */
    int compiled;
    /* eventuellement, rajouter des donnees intelligentes pour le stockage
       intelligent de Uniform* ... */
};


/* initialise le gestionnaire de shaders
   le parametre est un booleen indiquant si l'on souhaite ou non utiliser Cg */
int SCE_RShaderInit (int);
/* quitte le gestionnaire de shaders */
void SCE_RShaderQuit (void);

#ifdef SCE_USE_CG
/* renvoie le profile par defaut du type (pixel/vertex) demande */
CGprofile SCE_RGetCgProfile (SCEenum);
#endif

/* cree un objet de shader */
SCE_RShaderGLSL* SCE_RCreateShaderGLSL (SCEenum);
#ifdef SCE_USE_CG
SCE_RShaderCG* SCE_RCreateShaderCG(SCEenum);
#endif

/* supprime un shader */
void SCE_RDeleteShaderGLSL (SCE_RShaderGLSL*);
#ifdef SCE_USE_CG
void SCE_RDeleteShaderCG (SCE_RShaderCG*);
#endif

/* associe un code source au shader specifie */
void SCE_RSetShaderGLSLSource (SCE_RShaderGLSL*, char*);
int SCE_RSetShaderGLSLSourceDup (SCE_RShaderGLSL*, char*);
#ifdef SCE_USE_CG
void SCE_RSetShaderCGSource (SCE_RShaderCG*, char*);
int SCE_RSetShaderCGSourceDup (SCE_RShaderCG*, char*);
/* petite specialisation pour Cg, la gestion des arguments */
void SCE_RSetShaderCGArgs (SCE_RShaderCG*, char**);
int SCE_RSetShaderCGArgsDup (SCE_RShaderCG*, char**);
#endif

/* construit et compile le shader specifie */
int SCE_RBuildShaderGLSL (SCE_RShaderGLSL*);
#ifdef SCE_USE_CG
int SCE_RBuildShaderCG (SCE_RShaderCG*);
#endif

/* definit le shader selectionne comme shader actif pour le rendu */
#ifdef SCE_USE_CG
void SCE_RUseShaderCG (SCE_RShaderCG*);
#endif


/** !! ** fonctions pour les shaders GLSL uniquement ** !! **/

/* cree un objet de programme */
SCE_RProgram* SCE_RCreateProgram (void);

/* supprime un programme */
void SCE_RDeleteProgram (SCE_RProgram*);

/* associe un shader a un programme */
int SCE_RSetProgramShader (SCE_RProgram*, SCE_RShaderGLSL*, int);

/* lie un programme a ses shaders associes (le construit) */
int SCE_RBuildProgram (SCE_RProgram*);

/* defini le programme (GLSL) actif */
void SCE_RUseProgram (SCE_RProgram*);

/** !! ** ****************************************** ** !! **/


/* fonctions de gestion des etats des shaders
   (activation/desactivation manuelle) */
#if 0
void SCE_RDisableShaderGLSL (void);
#ifdef SCE_USE_CG
void SCE_RDisableShaderCG (int);
#endif
#endif

/* fonctions d'envoie des parametres.
   attention: pour les shaders GLSL, le mot 'Shader' est remplace par 'Program',
   car les modifications s'effectuent sur un program et non sur un shader. */

/* fonctions renvoyant l'index du parametre dont on donne le nom */
SCEint SCE_RGetProgramIndex (SCE_RProgram*, const char*);
#ifdef SCE_USE_CG
CGparameter SCE_RGetShaderCGIndex (SCE_RShaderCG*, const char*);
#endif

/* fonction de renvoie de l'index d'attribut */
SCEint SCE_RGetProgramAttribIndex (SCE_RProgram*, const char*);
/* TODO: trouver de quoi satisfaire les autres types de shaders... */

/* fonctions d'envoie de parametres aux shaders */
/* GLSL */
void SCE_RSetProgramParam (SCEint, int);
void SCE_RSetProgramParamf (SCEint, float);
void SCE_RSetProgramParam1fv (SCEint, size_t, const float*);
void SCE_RSetProgramParam2fv (SCEint, size_t, const float*);
void SCE_RSetProgramParam3fv (SCEint, size_t, const float*);
void SCE_RSetProgramParam4fv (SCEint, size_t, const float*);
void SCE_RSetProgramMatrix2 (SCEint, size_t, const float*);
void SCE_RSetProgramMatrix3 (SCEint, size_t, const float*);
void SCE_RSetProgramMatrix4 (SCEint, size_t, const float*);

/* Cg */
#ifdef SCE_USE_CG
void SCE_RSetShaderCGParam (CGparameter, int);
void SCE_RSetShaderCGParamf (CGparameter, float);
void SCE_RSetShaderCGParam1fv (CGparameter, size_t, const float*);
void SCE_RSetShaderCGParam2fv (CGparameter, size_t, const float*);
void SCE_RSetShaderCGParam3fv (CGparameter, size_t, const float*);
void SCE_RSetShaderCGParam4fv (CGparameter, size_t, const float*);
void SCE_RSetShaderCGMatrix (CGparameter, const float*);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
