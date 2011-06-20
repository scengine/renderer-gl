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
   updated: 20/06/2011 */

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

SCE_TMatrix4 sce_rmatrices[SCE_NUM_MATRICES] = {
    SCE_MATRIX4_IDENTITY,
    SCE_MATRIX4_IDENTITY,
    SCE_MATRIX4_IDENTITY,
    SCE_MATRIX4_IDENTITY
};

/**
 * \brief Load the specified matrix
 *
 * \param matrix use of the matrix to load
 * \param m a matrix
 */
void SCE_RLoadMatrix (SCE_RMatrix matrix, const SCE_TMatrix4 m)
{
    SCE_Matrix4_Copy (sce_rmatrices[matrix], m);
}

/**
 * \brief Default 'setmatrices' function
 */
static void SCE_RDefaultSetMatrices (void)
{
    SCE_TMatrix4 m;
    SCE_Matrix4_Mul (sce_rmatrices[SCE_MAT_CAMERA],
                     sce_rmatrices[SCE_MAT_OBJECT], m);
    glMatrixMode (GL_MODELVIEW);
    glLoadTransposeMatrixf (m);
    glMatrixMode (GL_PROJECTION);
    glLoadTransposeMatrixf (sce_rmatrices[SCE_MAT_PROJECTION]);
    glMatrixMode (GL_TEXTURE);
    glLoadTransposeMatrixf (sce_rmatrices[SCE_MAT_TEXTURE]);
}

/**
 * \brief Send all matrices to the driver
 */
SCE_RSetMatricesFunc SCE_RSetMatrices = SCE_RDefaultSetMatrices;

/**
 * \brief Sets the 'setmatrices' function
 *
 * In practice this function is used by the shaders manager to map the
 * matrices to shader uniform variables.
 */
void SCE_RMapMatrices (SCE_RSetMatricesFunc fun)
{
    if (fun)
        SCE_RSetMatrices = fun;
    else
        SCE_RSetMatrices = SCE_RDefaultSetMatrices;
}

/**
 * \brief Gets a matrix
 * \param matrix matrix to get
 * \param m retrieved matrix will be stored here
 * 
 * \see SCE_TMatrix4
 */
void SCE_RGetMatrix (SCE_RMatrix matrix, SCE_TMatrix4 m)
{
    SCE_Matrix4_Copy (m, sce_rmatrices[matrix]);
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
