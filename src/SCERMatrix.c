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
 
/* created: 10/01/2007
   updated: 19/06/2011 */

#include "SCE/renderer/SCERMatrix.h"

/**
 * \file SCERMatrix.c
 * \copydoc glmatrix
 * \brief OpenGL's matrices managment
 * 
 * \file SCERMatrix.h
 * \copydoc glmatrix
 * \brief OpenGL's matrices managment
 */

/**
 * \defgroup glmatrix OpenGL's matrices managment
 * \ingroup renderer-gl
 * \internal
 * \brief Manage OpenGL's matrices
 */

/** @{ */

/**
 * \brief Loads identidy of the active matrix
 * 
 * \see SCE_RSetActiveMatrix()
 */
void SCE_RLoadIdentityMatrix (void)
{
    glLoadIdentity ();
}

/**
 * \brief Calls glPushMatrix()
 */
void SCE_RPushMatrix (void)
{
    glPushMatrix ();
}
/**
 * \brief Calls glPopMatrix()
 */
void SCE_RPopMatrix (void)
{
    glPopMatrix ();
}

/**
 * \brief Sets the active matrix
 * \param matrix the matrix to activate. Can be one of SCE_MAT_MODELVIEW,
 *               SCE_MAT_PROJECTION or SCE_MAT_TEXTURE.
 */
void SCE_RSetActiveMatrix (SCE_RMatrix matrix)
{
    glMatrixMode (matrix);
}
/**
 * \brief Gets the active matrix
 * \return the activate matrix. Can be one of SCE_MAT_MODELVIEW,
 *         SCE_MAT_PROJECTION or SCE_MAT_TEXTURE.
 */
SCE_RMatrix SCE_RGetActiveMatrix (void)
{
    int matrix;
    glGetIntegerv (GL_MATRIX_MODE, &matrix);
    return matrix;
}

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
void SCE_RLoadMatrix (const float *m)
{
    glLoadTransposeMatrixf (m);
}

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
