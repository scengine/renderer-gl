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
   updated: 12/03/2008 */

#include <SCE/utils/SCEUtils.h>
#include "SCE/renderer/SCERSupport.h"
#include "SCE/renderer/SCERType.h"

#include "SCE/renderer/SCERShader.h"


/* Cg */
#ifdef SCE_USE_CG
/* constantes internes */
/* macros qui serviront a CgManager */
#define SCE_INIT 1
#define SCE_QUIT 2
#define SCE_GET_STATE 3


/* NOTE: truc douteux.. :/ ? */
/* a, b, c.. -> 3
   a, b, c, d, e, f, g.. -> 7
   Cg */
#define SCE_RG_ERROR (SCE_NUM_ERRORS+37)

/* variables statiques necessaires a l'utilisation de Cg */
static CGcontext context;
static CGprofile vs_profile;
static CGprofile ps_profile;


/* fonction callback en cas d'erreur Cg */
static void SCE_RCgOnError (void)
{
    SCEE_Log (SCE_RG_ERROR);
    SCEE_LogMsg ("a Cg error was occured :\n- %s\n- %s",
                   cgGetErrorString (cgGetError()), cgGetLastListing (context));
}


/* fonction de manipulation du runtime Cg */
static int SCE_RCgManager (int action)
{
    static int is_init = SCE_FALSE;

    SCE_btstart ();
    switch (action)
    {
    case SCE_INIT:
        if(!is_init)
        {
            /* envoie de la fonction callback pour la gestion des erreurs */
            cgSetErrorCallback (SCE_RCgOnError);

            /* creation du contexte */
            context = cgCreateContext ();
            if (SCEE_HaveError () && SCEE_GetCode () == SCE_RG_ERROR)
            {
                SCEE_LogSrc ();
                is_init = SCE_ERROR;
                break; /* on sort... */
            }

            /* chargement des 'profiles' */
            vs_profile = cgGLGetLatestProfile (CG_GL_VERTEX);
            ps_profile = cgGLGetLatestProfile (CG_GL_FRAGMENT);
            cgGLSetOptimalOptions (vs_profile);
            cgGLSetOptimalOptions (ps_profile);

            is_init = 1;
        }
        break;

    case SCE_QUIT:
        if (is_init)
        {
            if (context)
                cgDestroyContext (context);

            context = NULL; /* CGcontext n'est qu'un typedef struct* ... */
            vs_profile = CG_PROFILE_UNKNOWN;
            ps_profile = CG_PROFILE_UNKNOWN;

            is_init = 0;
        }
        break;

    /* rien a faire de particulier pour get state */
    case SCE_GET_STATE:
    }

    SCE_btend ();
    return is_init;
}
#endif /* SCE_USE_CG */

/* renvoie un booleen qui vaut true si type est un shader de pixel */
static int SCE_RIsPixelShader (SCEenum type)
{
    return (type == SCE_PIXEL_SHADER);
}


int SCE_RShaderInit (int use_cg)
{
#ifdef SCE_USE_CG
    if (use_cg) {
        if (!SCE_RCgManager (SCE_INIT))
            goto fail;
    }
#else /* pour eviter un warning : unused parameter 'use_cg' */
    use_cg = 0;
#endif

    return SCE_OK;
fail:
    SCEE_LogSrc ();
    SCEE_LogSrcMsg ("failed to initialize core shaders manager");
    return SCE_ERROR;
}
void SCE_RShaderQuit (void)
{
#ifdef SCE_USE_CG
    SCE_RCgManager (SCE_QUIT);
#endif
}


#ifdef SCE_USE_CG
CGprofile SCE_RGetCgProfile (SCEenum type)
{
    return (type == SCE_PIXEL_SHADER) ? ps_profile : vs_profile;
}
#endif


SCE_RShaderGLSL* SCE_RCreateShaderGLSL (SCEenum type)
{
    SCE_RShaderGLSL *shader = NULL;

    SCE_btstart ();
    shader = SCE_malloc (sizeof *shader);
    if (!shader)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return NULL;
    }

    shader->data = NULL;
    shader->compiled = SCE_FALSE;
    shader->is_pixelshader = SCE_RIsPixelShader (type);
    shader->type = (shader->is_pixelshader) ?
                    GL_FRAGMENT_SHADER : GL_VERTEX_SHADER;

    shader->id = glCreateShader (shader->type);
    if (shader->id == 0)
    {
        /* une erreur est survenue lors de la creation du shader */
        SCEE_Log (SCE_ERROR);
        SCEE_LogMsg ("I can't create a shader, what's the fuck ?");
        SCE_free (shader);
        SCE_btend ();
        return NULL;
    }

    SCE_btend ();
    return shader;
}

#ifdef SCE_USE_CG
SCE_RShaderCG* SCE_RCreateShaderCG (SCEenum type)
{
    SCE_RShaderCG *shader = NULL;

    SCE_btstart ();
    shader = SCE_malloc (sizeof *shader);
    if (!shader)
    {
        SCEE_LogSrc();
        SCE_btend ();
        return NULL;
    }

    shader->id = NULL;
    shader->args = NULL;
    shader->data = NULL;
    shader->compiled = SCE_FALSE;
    shader->is_pixelshader = SCE_RIsPixelShader (type);
    shader->type = SCE_RGetCgProfile (type);

    SCE_btend ();
    return shader;
}
#endif

/* *** */

void SCE_RDeleteShaderGLSL (SCE_RShaderGLSL *shader)
{
    SCE_btstart ();
    if (shader)
    {
        if (glIsShader (shader->id))
            glDeleteShader (shader->id);
        SCE_free (shader->data);
        SCE_free (shader);
    }
    SCE_btend ();
}

#ifdef SCE_USE_CG
void SCE_RDeleteShaderCG (SCE_RShaderCG *shader)
{
    SCE_btstart ();
    if (shader)
    {
        if (shader->id)
            cgDestroyProgram (shader->id);
        if (shader->args)
        {
            unsigned int i;
            for (i=0; shader->args[i]; i++)
                SCE_free (shader->args[i]);
            SCE_free (shader->args);
        }
        SCE_free (shader->data);
        SCE_free (shader);
    }
    SCE_btend ();
}
#endif


void SCE_RSetShaderGLSLSource (SCE_RShaderGLSL *shader, char *src)
{
    SCE_free (shader->data);
    shader->data = src;
}
int SCE_RSetShaderGLSLSourceDup (SCE_RShaderGLSL *shader, char *src)
{
    char *new = NULL;

    SCE_btstart ();
    new = SCE_String_Dup (src);
    if (!new)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    SCE_RSetShaderGLSLSource (shader, new);
    SCE_btend ();
    return SCE_OK;
}

#ifdef SCE_USE_CG
void SCE_RSetShaderCGSource (SCE_RShaderCG *shader, char *src)
{
    SCE_free (shader->data);
    shader->data = src;
}
int SCE_RSetShaderCGSourceDup (SCE_RShaderCG *shader, char *src)
{
    char *new = NULL;

    SCE_btstart ();
    new = SCE_String_Dup (src);
    if (!new)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    SCE_RSetShaderCGSource (shader, new);
    SCE_btend ();
    return SCE_OK;
}

void SCE_RSetShaderCGArgs (SCE_RShaderCG *shader, char **args)
{
    SCE_btstart ();
    if (shader->args)
    {
        usigned int i;
        for (i=0; shader->args[i]; i++)
            SCE_free (shader->args[i]);
        SCE_free (shader->args);
    }
    shader->args = args;
    SCE_btend ();
}
int SCE_RSetShaderCGArgsDup (SCE_RShaderCG *shader, char **args)
{
    size_t s = 0, i;
    char **new;

    SCE_btstart ();
    while (args[s])
        s++;
    new = SCE_malloc (s * sizeof *new + 1);
    if (!new)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    for (i=0; i<s; i++)
    {
        if (!(new[i] = SCE_String_Dup (args[i])))
        {
            SCEE_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
    }
    new[s] = NULL;
    SCE_RSetShaderCGArgs (shader, new);
    SCE_btend ();
    return SCE_OK;
}
#endif



/* fonctions generique des erreurs de construction */
static int SCE_RCantRecompile (void)
{
    SCEE_Log (SCE_INVALID_OPERATION);
    SCEE_LogMsg ("you can't re-compile a shader");
    return SCE_ERROR;
}
static int SCE_RNeedCode (void)
{
    SCEE_Log (SCE_INVALID_OPERATION);
    SCEE_LogMsg ("you can't compile a shader without source code");
    return SCE_ERROR;
}

int SCE_RBuildShaderGLSL (SCE_RShaderGLSL *shader)
{
    int compile_status = GL_TRUE;
    int loginfo_size = 0;
    char *loginfo = NULL;  /* journal de compilation (info log) */

    SCE_btstart ();
    /* NOTE: et pourquoi on pourrait pas recompiler un shader ?? */
    if (shader->compiled)
    {
        SCE_btend ();
        return SCE_RCantRecompile ();
    }

    if (!shader->data)
    {
        SCE_btend ();
        return SCE_RNeedCode ();
    }

    /* cast: fucking hack */
    glShaderSource (shader->id, 1, (const GLchar**)&shader->data, NULL);
    glCompileShader (shader->id);

    /* verification du succes de la compilation */
    glGetShaderiv (shader->id, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE)
    {
        /* erreur a la compilation
           recuperation du log d'erreur */

        SCEE_Log (SCE_INVALID_OPERATION);

        glGetShaderiv (shader->id, GL_INFO_LOG_LENGTH, &loginfo_size);
        loginfo = SCE_malloc (loginfo_size + 1);
        if (!loginfo)
        {
            SCEE_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }

        memset (loginfo, '\0', loginfo_size + 1);
        glGetShaderInfoLog (shader->id, loginfo_size, &loginfo_size, loginfo);

        SCEE_LogMsg ("error while compiling GLSL %s shader :\n%s",
                       shader->is_pixelshader ? "pixel":"vertex", loginfo);

        SCE_free (loginfo);
        SCE_btend ();
        return SCE_ERROR;
    }

    shader->compiled = SCE_TRUE;
    SCE_btend ();
    return SCE_OK;
}

#ifdef SCE_USE_CG
int SCE_RBuildShaderCG (SCE_RShaderCG *shader)
{
    SCE_btstart ();
    /* on verifie si Cg a ete initialise */
    if (!SCE_RCgManager (SCE_GET_STATE))
    {
        /* le contexte Cg n'a pas ete ou a ete detrui, arret */
        SCEE_Log (SCE_INVALID_OPERATION);
        SCEE_LogMsg ("you can't build a Cg shader if you have"
                       " not initialized the shaders manager");
        SCE_btend ();
        return SCE_ERROR;
    }

    if (shader->compiled)
    {
        SCE_btend ();
        return SCE_RCantRecompile ();
    }

    if (!shader->data)
    {
        SCE_btend ();
        return SCE_RNeedCode ();
    }

    #define SCE_RG_VERIF()\
    if (SCEE_HaveError () && SCEE_GetCode () == SCE_RG_ERROR)\
    {\
        SCEE_LogSrc ();\
        SCE_btend ();\
        return SCE_ERROR;\
    }

    shader->id = cgCreateProgram (context, CG_SOURCE, shader->data,
                                  shader->type, "main",
                                  (const char**)shader->args);
    SCE_RG_VERIF ()

    /* NOTE: faudra que je voye si c'est bien necessaire tout ca */
    /* compilation du program */
    if (!cgIsProgramCompiled (shader->id))
    {
        /* program non compile -> alors on le fait */
        cgCompileProgram (shader->id);
    }

    /* et on fait CA pour 'charger' le shader ?? */
    cgGLLoadProgram (shader->id);
    SCE_RG_VERIF ()

    #undef SCE_RG_VERIF

    shader->compiled = SCE_TRUE;

    SCE_btend ();
    return SCE_OK;
}
#endif

/* *** */

#ifdef SCE_USE_CG
void SCE_RUseShaderCG (SCE_RShaderCG *shader)
{
    static int is_ps = -1;

    if (shader)
    {
        cgGLBindProgram (shader->id);
        cgGLEnableProfile (shader->type);
        
        is_ps = (is_ps == SCE_TRUE) ? SCE_TRUE : shader->is_pixelshader;
    }
    else if (is_ps == SCE_TRUE)
    {
        cgGLDisableProfile (ps_profile);
        cgGLUnbindProgram (ps_profile);
        is_ps = SCE_FALSE;
    }
    else if (is_ps == SCE_FALSE)
    {
        /* NOTE: verifier l'ordre de desactivation!!! (Unbind & Disable) */
        cgGLDisableProfile (vs_profile);
        cgGLUnbindProgram (vs_profile);
        is_ps = -1;
    }
}
#endif

/* *** */

/** !! ** fonctions reservees aux shaders GLSL ** !! **/

SCE_RProgram* SCE_RCreateProgram (void)
{
    SCE_RProgram *prog = NULL;

    SCE_btstart ();
    prog = SCE_malloc (sizeof *prog);
    if (!prog)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return NULL;
    }

    prog->id = glCreateProgram ();
    /* et on teste pas la valeur de retour ?? */
    prog->compiled = SCE_FALSE;

    SCE_btend ();
    return prog;
}

void SCE_RDeleteProgram (SCE_RProgram *prog)
{
    SCE_btstart ();
    if (prog)
    {
        if (glIsProgram (prog->id))
            glDeleteProgram (prog->id);
        SCE_free (prog);
    }
    SCE_btend ();
}

int SCE_RSetProgramShader (SCE_RProgram *prog, SCE_RShaderGLSL *shader,
                           int attach)
{
    SCE_btstart ();
    if (attach)
        glAttachShader (prog->id, shader->id);
    else
        glDetachShader (prog->id, shader->id);

    /* il faut le recompiler pour appliquer les modifications! */
    prog->compiled = SCE_FALSE;
    SCE_btend ();
    return SCE_OK;
}

int SCE_RBuildProgram (SCE_RProgram *prog)
{
    int status = GL_TRUE;
    int loginfo_size = 0;
    char *loginfo = NULL;  /* chaine du log d'erreur */

    SCE_btstart ();
    /* lie le program a ses shaders 'attaches' */
    glLinkProgram (prog->id);

    /* recuperation du status du liage */
    glGetProgramiv (prog->id, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        /* erreur de 'linkage', recuperation du message d'erreur */
        SCEE_Log (SCE_INVALID_OPERATION);

        glGetProgramiv (prog->id, GL_INFO_LOG_LENGTH, &loginfo_size);
        loginfo = SCE_malloc (loginfo_size + 1);
        if (!loginfo)
        {
            SCEE_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
        memset (loginfo, '\0', loginfo_size + 1);
        glGetProgramInfoLog (prog->id, loginfo_size, &loginfo_size, loginfo);

        SCEE_LogMsg ("can't link program, reason : %s", loginfo);

        SCE_free (loginfo);
        SCE_btend ();
        return SCE_ERROR;
    }
    glValidateProgram (prog->id);
    glGetProgramiv (prog->id, GL_VALIDATE_STATUS, &status);
    if (status != GL_TRUE)
    {
        /* what to do? */
        /* TODO: add program name */
        SCEE_SendMsg ("can't validate program");
    }
    prog->compiled = SCE_TRUE;
    SCE_btend ();
    return SCE_OK;
}

void SCE_RUseProgram (SCE_RProgram *prog)
{
    if (prog)
    {
        glUseProgram (prog->id);
    }
    else
        glUseProgram (0);
}

/** ********************************************* **/


#if 0
void SCE_RDisableShaderGLSL (void)
{
    glUseProgram (0);
}
#ifdef SCE_USE_CG
void SCE_RDisableShaderCG (int type)
{
    if (type == SCE_PIXEL_SHADER)
    {
        cgGLDisableProfile (ps_profile);
        cgGLUnbindProgram (ps_profile);
    }
    else
    {
        cgGLDisableProfile (vs_profile);
        cgGLUnbindProgram (vs_profile);
    }
}
#endif
#endif


SCEint SCE_RGetProgramIndex (SCE_RProgram *prog, const char *name)
{
    return glGetUniformLocation (prog->id, name);
}
#ifdef SCE_USE_CG
CGparameter SCE_RGetShaderCGIndex (SCE_RShaderCG *shader, const char *name)
{
    return cgGetNamedParameter (shader->id, name);
}
#endif
/* ajoute le 12/03/2008 */
SCEint SCE_RGetProgramAttribIndex (SCE_RProgram *prog, const char *name)
{
    return glGetAttribLocation (prog->id, name);
}


/* revise le 14/03/2008 */
void SCE_RSetProgramParam (SCEint idx, int val)
{
    glUniform1i (idx, val);
}
void SCE_RSetProgramParamf (SCEint idx, float val)
{
    glUniform1f (idx, val);
}
void SCE_RSetProgramParam1fv (SCEint idx, size_t size, const float *val)
{
    glUniform1fv (idx, size, val);
}
void SCE_RSetProgramParam2fv (SCEint idx, size_t size, const float *val)
{
    glUniform2fv (idx, size, val);
}
void SCE_RSetProgramParam3fv (SCEint idx, size_t size, const float *val)
{
    glUniform3fv (idx, size, val);
}
void SCE_RSetProgramParam4fv (SCEint idx, size_t size, const float *val)
{
    glUniform4fv (idx, size, val);
}
void SCE_RSetProgramMatrix2 (SCEint idx, size_t size, const float *mat)
{
    glUniformMatrix2fv (idx, size, SCE_TRUE, mat);
}
void SCE_RSetProgramMatrix3 (SCEint idx, size_t size, const float *mat)
{
    glUniformMatrix3fv (idx, size, SCE_TRUE, mat);
}
void SCE_RSetProgramMatrix4 (SCEint idx, size_t size, const float *mat)
{
    glUniformMatrix4fv (idx, size, SCE_TRUE, mat);
}


#ifdef SCE_USE_CG
/* revise le 14/03/2008 */
void SCE_RSetShaderCGParam (CGparameter param, int val)
{
    cgGLSetParameter1f (param, (float)val);
}
void SCE_RSetShaderCGParamf (CGparameter param, float val)
{
    cgGLSetParameter1f (param, val);
}
/* revise le 14/03/2008 */
#define SCE_RG_PARAM_FUNC(n)\
void SCE_RSetShaderCGParam##n##fv (CGparameter param, size_t size, const float *val)\
{\
    if (cgGetParameterType (param) == CG_ARRAY)\
    {\
        int asize = cgGetArraySize (param, 0), i;\
        /* TODO: ne gere theoriquement pas les tableaux tab[][]... */\
        for (i=0; i<asize && i<size; i++)\
            cgGLSetParameter##n##fv (cgGetArrayParameter (param, i), val);\
    }\
    else\
        cgGLSetParameter##n##fv (param, val);\
}
SCE_RG_PARAM_FUNC(1)
SCE_RG_PARAM_FUNC(2)
SCE_RG_PARAM_FUNC(3)
SCE_RG_PARAM_FUNC(4)
#undef SCE_RG_PARAM_FUNC

void SCE_RSetShaderCGMatrix (CGparameter param, const float *mat)
{
    cgGLSetMatrixParameterfc (param, mat);
}
#endif /* SCE_USE_CG */
