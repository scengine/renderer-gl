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
   updated: 18/06/2011 */

#include <SCE/utils/SCEUtils.h>
#include "SCE/renderer/SCERSupport.h"
#include "SCE/renderer/SCERType.h"
#include "SCE/renderer/SCERVertexArray.h" /* SCE_RVertexAttributesMap */

#include "SCE/renderer/SCERShader.h"

static SCEenum sce_gltype[3] = {
    GL_VERTEX_SHADER,
    GL_FRAGMENT_SHADER,
    GL_GEOMETRY_SHADER
};

static const char *sce_typename[3] = {
    "vertex",
    "pixel",
    "geometry"
};


int SCE_RShaderInit (void)
{
    return SCE_OK;
}
void SCE_RShaderQuit (void)
{
}


SCE_RShaderGLSL* SCE_RCreateShaderGLSL (SCE_RShaderType type)
{
    SCE_RShaderGLSL *shader = NULL;

    shader = SCE_malloc (sizeof *shader);
    if (!shader) {
        SCEE_LogSrc ();
        return NULL;
    }

    shader->data = NULL;
    shader->compiled = SCE_FALSE;
    shader->type = type;
    shader->gltype = sce_gltype[type];

    shader->id = glCreateShader (shader->gltype);
    if (shader->id == 0) {
        SCEE_Log (SCE_ERROR);
        SCEE_LogMsg ("I can't create a shader, what's the fuck?");
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
                     sce_typename[shader->type], loginfo);
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
    prog->linked = SCE_FALSE;
    SCE_RInitVertexAttributesMap (prog->map);
    prog->map_built = SCE_FALSE;
    prog->use_map = SCE_FALSE;

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

    /* program need to be relinked in order to apply the changes */
    prog->linked = SCE_FALSE;
    return SCE_OK;
}

int SCE_RBuildProgram (SCE_RProgram *prog)
{
    int status = GL_TRUE;
    int loginfo_size = 0;
    char *loginfo = NULL;

    if (prog->linked)
      return SCE_OK;

    glLinkProgram (prog->id);

    glGetProgramiv (prog->id, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        SCEE_Log (SCE_INVALID_OPERATION);

        glGetProgramiv (prog->id, GL_INFO_LOG_LENGTH, &loginfo_size);
        loginfo = SCE_malloc (loginfo_size + 1);
        if (!loginfo) {
            SCEE_LogSrc ();
            return SCE_ERROR;
        }
        memset (loginfo, '\0', loginfo_size + 1);
        glGetProgramInfoLog (prog->id, loginfo_size, &loginfo_size, loginfo);

        SCEE_LogMsg ("can't link program, reason: %s", loginfo);

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

    prog->linked = SCE_TRUE;

    /* if the map was previously built, rebuild it */
    if (prog->map_built)
      SCE_RSetupProgramAttributesMapping (prog);

    return SCE_OK;
}

/**
 * \brief Construct vertex attributes map of the given program
 * \param prog a program
 *
 * Retrieve locations of the named vertex attributes into the vertex
 * shader of \p prog according to the names defined by the
 * SCE_*_ATTRIB_NAME constants and bind them to the associated
 * SCE_EVertexAttribute using a SCE_RVertexAttributesMap. Note that
 * the map is only constructed and not used. If you wish to use it you
 * will have to call SCE_RActivateProgramAttributesMapping(). If \p
 * prog is not yet built then the map construction will be proceeded
 * automatically when SCE_RBuildProgram() is called.
 */
void SCE_RSetupProgramAttributesMapping (SCE_RProgram *prog)
{
    int loc;
#define SCE_VATTRIB_MAP(a) do {                                 \
        loc = glGetAttribLocation (prog->id, a##_ATTRIB_NAME);  \
        if (loc != -1)                                          \
            prog->map[a] = loc;                                 \
    } while (0)

    if (prog->linked) {
        SCE_VATTRIB_MAP (SCE_POSITION);
        SCE_VATTRIB_MAP (SCE_COLOR);
        SCE_VATTRIB_MAP (SCE_NORMAL);
        SCE_VATTRIB_MAP (SCE_TANGENT);
        SCE_VATTRIB_MAP (SCE_BINORMAL);
        SCE_VATTRIB_MAP (SCE_TEXCOORD0);
        SCE_VATTRIB_MAP (SCE_TEXCOORD1);
        SCE_VATTRIB_MAP (SCE_TEXCOORD2);
        SCE_VATTRIB_MAP (SCE_TEXCOORD3);
        SCE_VATTRIB_MAP (SCE_TEXCOORD4);
        SCE_VATTRIB_MAP (SCE_TEXCOORD5);
        SCE_VATTRIB_MAP (SCE_TEXCOORD6);
        SCE_VATTRIB_MAP (SCE_TEXCOORD7);
    }

#undef SCE_VATTRIB_MAP

    prog->map_built = SCE_TRUE;
}

/**
 * \brief Set attributes mapping state
 *
 * \param prog a program
 * \param activate boolean for whether or not to activate the attributes mapping
 */
void SCE_RActivateProgramAttributesMapping (SCE_RProgram *prog, int activate)
{
    prog->use_map = activate;
}


void SCE_RUseProgram (SCE_RProgram *prog)
{
    if (prog) {
        glUseProgram (prog->id);
        /* useless if statement in a full GL3 renderer */
        if (prog->use_map)
            SCE_RUseVertexAttributesMap (prog->map);
    } else {
        glUseProgram (0);
        /* useless call in a full GL3 renderer */
        SCE_RDisableVertexAttributesMap ();
    }
}


#if 0
void SCE_RDisableShaderGLSL (void)
{
    glUseProgram (0);
}
#endif


static SCEenum SCE_RAdjacentPrim (SCEenum prim)
{
  switch (prim) {
  case GL_LINES: return GL_LINES_ADJACENCY;
  case GL_TRIANGLES: return GL_TRIANGLES_ADJACENCY;
  default: return prim;    /* adjacency not supported for other primitives */
  }
}

/**
 * \brief Setup input primitive type for the geometry shader
 *
 * \param prog a GLSL program
 * \param prim desired input primitive type
 * \param adj set adjacent primitives available into the geometry shader
 * \returns SCE_ERROR on error, SCE_OK otherwise
 */
int SCE_RSetProgramInputPrimitive (SCE_RProgram *prog,
                                   SCE_EPrimitiveType prim, int adj)
{
    SCEenum p = sce_rprimtypes[prim];
    if (adj)
        p = SCE_RAdjacentPrim (p);
    glProgramParameteri (prog->id, GL_GEOMETRY_INPUT_TYPE_EXT, p);
    if (prog->linked) {
        /* automatic relink if the shader was already linked */
        prog->linked = SCE_FALSE;
        return SCE_RBuildProgram (prog);
    }
    return SCE_OK;
}
/**
 * \brief Setup output primitive type for the geometry shader
 *
 * \param prog a GLSL program
 * \param prim desired output primitive type
 * \returns SCE_ERROR on error, SCE_OK otherwise
 */
int SCE_RSetProgramOutputPrimitive (SCE_RProgram *prog,
                                    SCE_EPrimitiveType prim)
{
    SCEenum p = sce_rprimtypes[prim];
    glProgramParameteri (prog->id, GL_GEOMETRY_OUTPUT_TYPE_EXT, p);
    if (prog->linked) {
        /* automatic relink if the shader was already linked */
        prog->linked = SCE_FALSE;
        return SCE_RBuildProgram (prog);
    }
    return SCE_OK;
}


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

/**
 * \brief Specify the value of a uniform 2x2 matrix of a shader
 * 
 * \param idx the uniform matrix index
 * \param size the number of matrices if the uniform is an array, 1 otherwise
 * \param mat pointer to the matrix
 */
void SCE_RSetProgramMatrix2 (SCEint idx, size_t size, const float *mat)
{
    glUniformMatrix2fv (idx, size, SCE_TRUE, mat);
}
/**
 * \brief Specify the value of a uniform 3x3 matrix of a shader
 * 
 * \param idx the uniform matrix index
 * \param size the number of matrices if the uniform is an array, 1 otherwise
 * \param mat pointer to the matrix
 */
void SCE_RSetProgramMatrix3 (SCEint idx, size_t size, const float *mat)
{
    glUniformMatrix3fv (idx, size, SCE_TRUE, mat);
}
/**
 * \brief Specify the value of a uniform 4x4 matrix of a shader
 * 
 * \param idx the uniform matrix index
 * \param size the number of matrices if the uniform is an array, 1 otherwise
 * \param mat pointer to the matrix
 */
void SCE_RSetProgramMatrix4 (SCEint idx, size_t size, const float *mat)
{
    glUniformMatrix4fv (idx, size, SCE_TRUE, mat);
}
