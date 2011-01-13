/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2011  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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
   updated: 13/01/2011 */

#include <SCE/utils/SCEUtils.h>
#include "SCE/renderer/SCERSupport.h"
#include "SCE/renderer/SCERType.h"

#include "SCE/renderer/SCERShader.h"

static int SCE_RIsPixelShader (SCEenum type)
{
    return (type == SCE_PIXEL_SHADER);
}


int SCE_RShaderInit (void)
{
    return SCE_OK;
}
void SCE_RShaderQuit (void)
{
}


SCE_RShaderGLSL* SCE_RCreateShaderGLSL (SCEenum type)
{
    SCE_RShaderGLSL *shader = NULL;
    shader = SCE_malloc (sizeof *shader);
    if (!shader) {
        SCEE_LogSrc ();
        return NULL;
    }

    shader->data = NULL;
    shader->compiled = SCE_FALSE;
    shader->is_pixelshader = SCE_RIsPixelShader (type);
    shader->type = (shader->is_pixelshader) ?
                    GL_FRAGMENT_SHADER : GL_VERTEX_SHADER;

    shader->id = glCreateShader (shader->type);
    if (shader->id == 0) {
        SCEE_Log (SCE_ERROR);
        SCEE_LogMsg ("I can't create a shader, what's the fuck ?");
        SCE_free (shader);
        return NULL;
    }

    return shader;
}
void SCE_RDeleteShaderGLSL (SCE_RShaderGLSL *shader)
{
    if (shader) {
        if (glIsShader (shader->id))
            glDeleteShader (shader->id);
        SCE_free (shader->data);
        SCE_free (shader);
    }
}


void SCE_RSetShaderGLSLSource (SCE_RShaderGLSL *shader, char *src)
{
    SCE_free (shader->data);
    shader->data = src;
}
int SCE_RSetShaderGLSLSourceDup (SCE_RShaderGLSL *shader, char *src)
{
    char *new = NULL;

    new = SCE_String_Dup (src);
    if (!new) {
        SCEE_LogSrc ();
        return SCE_ERROR;
    }
    SCE_RSetShaderGLSLSource (shader, new);
    return SCE_OK;
}

int SCE_RBuildShaderGLSL (SCE_RShaderGLSL *shader)
{
    int compile_status = GL_TRUE;
    int loginfo_size = 0;
    char *loginfo = NULL;

    glShaderSource (shader->id, 1, (const GLchar**)&shader->data, NULL);
    glCompileShader (shader->id);

    glGetShaderiv (shader->id, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        SCEE_Log (SCE_INVALID_OPERATION);
        glGetShaderiv (shader->id, GL_INFO_LOG_LENGTH, &loginfo_size);
        loginfo = SCE_malloc (loginfo_size + 1);
        if (!loginfo) {
            SCEE_LogSrc ();
            return SCE_ERROR;
        }

        memset (loginfo, '\0', loginfo_size + 1);
        glGetShaderInfoLog (shader->id, loginfo_size, &loginfo_size, loginfo);

        SCEE_LogMsg ("error while compiling GLSL %s shader :\n%s",
                       shader->is_pixelshader ? "pixel":"vertex", loginfo);
        SCE_free (loginfo);
        return SCE_ERROR;
    }

    shader->compiled = SCE_TRUE;
    return SCE_OK;
}

SCE_RProgram* SCE_RCreateProgram (void)
{
    SCE_RProgram *prog = NULL;
    prog = SCE_malloc (sizeof *prog);
    if (!prog) {
        SCEE_LogSrc ();
        return NULL;
    }

    prog->id = glCreateProgram ();
    /* et on teste pas la valeur de retour ?? */
    prog->compiled = SCE_FALSE;

    return prog;
}

void SCE_RDeleteProgram (SCE_RProgram *prog)
{
    if (prog) {
        if (glIsProgram (prog->id))
            glDeleteProgram (prog->id);
        SCE_free (prog);
    }
}

int SCE_RSetProgramShader (SCE_RProgram *prog, SCE_RShaderGLSL *shader,
                           int attach)
{
    if (attach)
        glAttachShader (prog->id, shader->id);
    else
        glDetachShader (prog->id, shader->id);

    /* il faut le recompiler pour appliquer les modifications! */
    prog->compiled = SCE_FALSE;
    return SCE_OK;
}

int SCE_RBuildProgram (SCE_RProgram *prog)
{
    int status = GL_TRUE;
    int loginfo_size = 0;
    char *loginfo = NULL;  /* chaine du log d'erreur */

    /* lie le program a ses shaders 'attaches' */
    glLinkProgram (prog->id);

    /* recuperation du status du liage */
    glGetProgramiv (prog->id, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        /* erreur de 'linkage', recuperation du message d'erreur */
        SCEE_Log (SCE_INVALID_OPERATION);

        glGetProgramiv (prog->id, GL_INFO_LOG_LENGTH, &loginfo_size);
        loginfo = SCE_malloc (loginfo_size + 1);
        if (!loginfo) {
            SCEE_LogSrc ();
            return SCE_ERROR;
        }
        memset (loginfo, '\0', loginfo_size + 1);
        glGetProgramInfoLog (prog->id, loginfo_size, &loginfo_size, loginfo);

        SCEE_LogMsg ("can't link program, reason : %s", loginfo);

        SCE_free (loginfo);
        return SCE_ERROR;
    }
    glValidateProgram (prog->id);
    glGetProgramiv (prog->id, GL_VALIDATE_STATUS, &status);
    if (status != GL_TRUE) {
        /* what to do? */
        /* TODO: add program name */
        SCEE_SendMsg ("can't validate program");
    }
    prog->compiled = SCE_TRUE;
    return SCE_OK;
}

void SCE_RUseProgram (SCE_RProgram *prog)
{
    if (prog)
        glUseProgram (prog->id);
    else
        glUseProgram (0);
}


#if 0
void SCE_RDisableShaderGLSL (void)
{
    glUseProgram (0);
}
#endif


SCEint SCE_RGetProgramIndex (SCE_RProgram *prog, const char *name)
{
    return glGetUniformLocation (prog->id, name);
}
SCEint SCE_RGetProgramAttribIndex (SCE_RProgram *prog, const char *name)
{
    return glGetAttribLocation (prog->id, name);
}


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
