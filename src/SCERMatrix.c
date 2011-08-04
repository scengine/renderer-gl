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
    SCE_MATRIX4_IDENTITY,
    SCE_MATRIX4_IDENTITY
};

static void SCE_RSetModelviewMatrix (void)
{
    glMatrixMode (GL_MODELVIEW);
    glLoadTransposeMatrixf (sce_rmatrices[SCE_MAT_MODELVIEW]);
}
static void SCE_RSetProjectionMatrix (void)
{
    glMatrixMode (GL_PROJECTION);
    glLoadTransposeMatrixf (sce_rmatrices[SCE_MAT_PROJECTION]);
}
static void SCE_RSetTextureMatrix (void)
{
    glMatrixMode (GL_TEXTURE);
    glLoadTransposeMatrixf (sce_rmatrices[SCE_MAT_TEXTURE]);
}

/* default are common GL functions */
static SCE_RSetMatrixFunc sce_setmatrix_funs[SCE_NUM_MATRICES] = {
    SCE_RSetModelviewMatrix,
    SCE_RSetModelviewMatrix,
    SCE_RSetProjectionMatrix,
    SCE_RSetTextureMatrix,
    SCE_RSetModelviewMatrix
};

static SCE_RSetMatrixFunc *setmatrix = sce_setmatrix_funs;

/**
 * \brief Load the specified matrix
 *
 * \param matrix use of the matrix to load
 * \param m a matrix
 */
void SCE_RLoadMatrix (SCE_RMatrix matrix, const SCE_TMatrix4 m)
{
    SCE_Matrix4_Copy (sce_rmatrices[matrix], m);
    if (matrix == SCE_MAT_OBJECT || matrix == SCE_MAT_CAMERA) {
        SCE_Matrix4_Mul (sce_rmatrices[SCE_MAT_CAMERA],
                         sce_rmatrices[SCE_MAT_OBJECT],
                         sce_rmatrices[SCE_MAT_MODELVIEW]);
        setmatrix[SCE_MAT_MODELVIEW] ();
    }
    /* when no shader is active are we are in a full GL3 context, this call
       should be removed */
    setmatrix[matrix] ();
}

/**
 * \brief Sets the 'SCE_RLoadMatrix()' function
 * \param funs must be a pointer to an array of size SCE_NUM_MATRICES,
 * or NULL to restore the default state
 *
 * In practice this function is used by the shaders manager to map the
 * matrices to shader uniform variables.
 */
void SCE_RMapMatrices (SCE_RSetMatrixFunc *funs)
{
    int i;
    if (funs)
        setmatrix = funs;
    else
        setmatrix = sce_setmatrix_funs;
    /* refresh matrix state */
    for (i = 0; i < SCE_NUM_MATRICES; i++)
        setmatrix[i] ();
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
