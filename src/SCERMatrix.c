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
 
/* created: 10/01/2007
   updated: 09/03/2008 */

#include "SCE/renderer/SCERMatrix.h"

/**
 * \file SCECMatrix.c
 * \copydoc corematrix
 * \brief OpenGL's matrices managment
 * 
 * \file SCECMatrix.h
 * \copydoc corematrix
 * \brief OpenGL's matrices managment
 */

/**
 * \defgroup corematrix OpenGL's matrices managment
 * \ingroup core
 * \internal
 * \brief Manage OpenGL's matrices
 */

/** @{ */


/* ajoute le 02/07/2007 */
/* revise le 09/03/2008 */
/**
 * \brief Loads identidy of the active matrix
 * 
 * \see SCE_RSetActiveMatrix()
 */
void SCE_RLoadIdentityMatrix (void)
{
    glLoadIdentity ();
}

/* revise le 09/03/2008 */
/**
 * \brief Calls glPushMatrix()
 */
void SCE_RPushMatrix (void)
{
    glPushMatrix ();
}
/* revise le 09/03/2008 */
/**
 * \brief Calls glPopMatrix()
 */
void SCE_RPopMatrix (void)
{
    glPopMatrix ();
}

/* revise le 09/03/2008 */
/**
 * \brief Sets the active matrix
 * \param matrix the matrix to activate. Can be one of SCE_MAT_MODELVIEW,
 *               SCE_MAT_PROJECTION or SCE_MAT_TEXTURE.
 */
void SCE_RSetActiveMatrix (SCEenum matrix)
{
    glMatrixMode (matrix);
}
/* revise le 09/03/2008 */
/**
 * \brief Gets the active matrix
 * \return the activate matrix. Can be one of SCE_MAT_MODELVIEW,
 *         SCE_MAT_PROJECTION or SCE_MAT_TEXTURE.
 */
int SCE_RGetActiveMatrix (void)
{
    int matrix;
    glGetIntegerv (GL_MATRIX_MODE, &matrix);
    return matrix;
}

/* revise le 09/03/2008 */
/**
 * \brief Multiplies the active matrix
 * \param m a 4*4 matrix to multily with
 * 
 * This function multiplies the active matrix by the given one.
 */
void SCE_RMultMatrix (const float *m)
{
    glMultTransposeMatrixf (m);
}
/* revise le 09/03/2008 */
/** 
 * 
 */
void SCE_RLoadMatrix (const float *m)
{
    glLoadTransposeMatrixf (m);
}

/* revise le 09/03/2008 */
/**
 * \brief Gets a matrix
 * \param matrix whitch matrix to get. Can be one of SCE_MAT_MODELVIEW,
 *               SCE_MAT_PROJECTION, SCE_MAT_TEXTURE or SCE_RURRENT_MATRIX.
 * \param m pointer to a 16 or more float array where write the queried matrix.
 * 
 * \see SCE_TMatrix4
 */
void SCE_RGetMatrix (int matrix, float *m)
{
    if (matrix == SCE_RURRENT_MATRIX)
        glGetIntegerv (GL_MATRIX_MODE, &matrix);
    switch (matrix)
    {
    case GL_MODELVIEW:  glGetFloatv (GL_TRANSPOSE_MODELVIEW_MATRIX,  m); break;
    case GL_PROJECTION: glGetFloatv (GL_TRANSPOSE_PROJECTION_MATRIX, m); break;
    case GL_TEXTURE:    glGetFloatv (GL_TRANSPOSE_TEXTURE_MATRIX,    m);
    }
}

/**
 * \brief Translates the active matrix
 * \param x Translation factor on X
 * \param y Translation factor on Y
 * \param z Translation factor on Z
 * 
 * \deprecated
 */
void SCE_RTranslateMatrix (float x, float y, float z)
{
    glTranslatef(x, y, z);
}
/**
 * \brief Rotates the active matrix
 * \param angle rotation factor in degrees
 * \param x X part of the vector rotation axis
 * \param y Y part of the vector rotation axis
 * \param z Z part of the vector rotation axis
 * 
 * \deprecated
 */
void SCE_RRotateMatrix (float angle, float x, float y, float z)
{
    glRotatef(angle, x, y, z);
}
/**
 * \brief Scales the active matrix
 * \param x X axis scaling factor
 * \param y Y axis scaling factor
 * \param z Z axis scaling factor
 * 
 * \deprecated
 */
void SCE_RScaleMatrix (float x, float y, float z)
{
    glScalef(x, y, z);
}
/**
 * \brief Vectorial version of SCE_RTranslateMatrix()
 * \see SCE_RTranslateMatrix()
 * 
 * \deprecated
 */
void SCE_RTranslateMatrixv (float *v)
{
    glTranslatef(v[0], v[1], v[2]);
}
/**
 * \brief Vectorial version of SCE_RRotateMatrix()
 * \see SCE_RRotateMatrix()
 * 
 * \deprecated
 */
void SCE_RRotateMatrixv (float angle, float *v)
{
    glRotatef(angle, v[0], v[1], v[2]);
}
/**
 * \brief Vectorial version of SCE_RScaleMatrix()
 * \see SCE_RScaleMatrix()
 * 
 * \deprecated
 */
void SCE_RScaleMatrixv (float *v)
{
    glScalef(v[0], v[1], v[2]);
}
/**/

#if 0
void SCE_RLook3D(
  float p0, float p1, float p2,
  float v0, float v1, float v2,
  float i0, float i1, float i2)
{
    gluLookAt(p0, p1, p2, v0, v1, v2, i0, i1, i2);
}
void SCE_RLook3Dv(const float *p, const float *v, const float *i)
{
    gluLookAt(p[0], p[1], p[2], v[0], v[1], v[2], i[0], i[1], i[2]);
}
#endif

/**
 * \brief Calls glFrustum()
 */
void SCE_RSetFrustumMatrix (
  GLdouble left, GLdouble right,
  GLdouble bottom, GLdouble top,
  GLdouble near, GLdouble far)
{
    glFrustum (left, right, bottom, top, near, far);
}

/**
 * \brief Calls glOrtho()
 */
void SCE_RSetOrthoMatrix (
  GLdouble left, GLdouble right,
  GLdouble bottom, GLdouble top,
  GLdouble near, GLdouble far)
{
    glOrtho (left, right, bottom, top, near, far);
}


/**
 * \brief Sets the render viewport
 * \param x x origin
 * \param y y origin
 * \param w width
 * \param h height
 * 
 * This function sets the render viewport by calling glViewport().
 */
void SCE_RViewport (int x, int y, int w, int h)
{
    glViewport (x, y, w, h);
}

/** @} */
