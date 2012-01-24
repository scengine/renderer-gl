/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2012  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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
 
/* created: 14/01/2007
   updated: 23/01/2012 */

#ifndef SCERTEXTURE_H
#define SCERTEXTURE_H

#include <stdarg.h>
#include <SCE/utils/SCEUtils.h>
#include <SCE/core/SCECore.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup coretexture
 * @{
 */

/**
 * \brief GL texture units enum
 */
enum sce_rtexunit {
    SCE_TEX0 = GL_TEXTURE0,
    SCE_TEX1 = GL_TEXTURE1,
    SCE_TEX2 = GL_TEXTURE2,
    SCE_TEX3 = GL_TEXTURE3,
    SCE_TEX4 = GL_TEXTURE4,
    SCE_TEX5 = GL_TEXTURE5,
    SCE_TEX6 = GL_TEXTURE6,
    SCE_TEX7 = GL_TEXTURE7,
    SCE_TEX8 = GL_TEXTURE8,
    SCE_TEX9 = GL_TEXTURE9,
    SCE_TEX10 = GL_TEXTURE10,
    SCE_TEX11 = GL_TEXTURE11
};
typedef enum sce_rtexunit SCE_RTexUnit;

typedef enum {
    SCE_TEX_1D = GL_TEXTURE_1D,
    SCE_TEX_2D = GL_TEXTURE_2D,
    SCE_TEX_3D = GL_TEXTURE_3D,
    SCE_TEX_CUBE = GL_TEXTURE_CUBE_MAP
} SCE_RTexType;

/**
 * \brief Faces for a cube map texture
 */
enum sce_rtexcubeface {
    SCE_TEX_POSX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    SCE_TEX_NEGX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    SCE_TEX_POSY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    SCE_TEX_NEGY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    SCE_TEX_POSZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    SCE_TEX_NEGZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};
typedef enum sce_rtexcubeface SCE_RTexCubeFace;

/**
 * \brief Filters for textures
 */
enum sce_rtexfilter {
    SCE_TEX_NEAREST = GL_NEAREST,
    SCE_TEX_LINEAR = GL_LINEAR,
    SCE_TEX_BILINEAR = GL_LINEAR_MIPMAP_NEAREST,
    SCE_TEX_TRILINEAR = GL_LINEAR_MIPMAP_LINEAR
};
typedef enum sce_rtexfilter SCE_RTexFilter;


/** \copydoc sce_rtexture */
typedef struct sce_rtexture SCE_RTexture;
/**
 * \brief A texture
 * \todo incomplete documentation
 */
struct sce_rtexture {
    SCEuint id;         /**< OpenGL identifier */
    SCEenum target;     /**< Target (type) of the texture */
    SCE_SList data[6];  /**< Data of the texture, one list represent mipmap
                         *   levels, all the 6 lists are used for cubemaps */
    int have_data;      /**< Boolean, true if have data */

    int use_mipmap;     /**< Do we use mipmapping ? */
    int hw_mipmap;      /**< Do we use hardware for mipmaps generation ? */
    SCEfloat aniso_level;       /**< Anisotropic filtering level */

    enum SCE_ETexType {
        SCE_TEXTYPE_1D, SCE_TEXTYPE_2D, SCE_TEXTYPE_3D, SCE_TEXTYPE_CUBE
    } type;             /**< Dunno olol */
};

/** @} */

int SCE_RTextureInit (void);
void SCE_RTextureQuit (void);

int SCE_RGetTextureResourceType (void);

void SCE_RSetTextureAnisotropic (SCE_RTexture*, SCEfloat);
float SCE_RGetTextureMaxAnisotropic (void);

SCE_RTexture* SCE_RCreateTexture (SCE_RTexType);
void SCE_RDeleteTexture (SCE_RTexture*);

int SCE_RGetMaxTextureUnits (void);
int SCE_RGetMaxTextureSize (void);
int SCE_RGetMaxTextureCubeSize (void);
int SCE_RGetMaxTexture3DSize (void);

void SCE_RSetTextureFilter (SCE_RTexture*, SCE_RTexFilter);
void SCE_RPixelizeTexture (SCE_RTexture*, int);

void SCE_RSetTextureParam (SCE_RTexture*, SCEenum, int) SCE_GNUC_DEPRECATED;
void SCE_RSetTextureParamf (SCE_RTexture*, SCEenum, float) SCE_GNUC_DEPRECATED;

void SCE_RSetTextureGen (SCE_RTexture*, SCEenum, SCEenum, int) SCE_GNUC_DEPRECATED;
void SCE_RSetTextureGenf (SCE_RTexture*, SCEenum, SCEenum, float) SCE_GNUC_DEPRECATED;
void SCE_RSetTextureGenfv (SCE_RTexture*, SCEenum, SCEenum, float*) SCE_GNUC_DEPRECATED;

void SCE_RForceTexturePixelFormat (int, SCE_EPixelFormat);
void SCE_RForceTextureType (int, SCE_EType);
void SCE_RForceTextureFormat (int, SCE_EImageFormat);

SCEenum SCE_RGetTextureTarget (SCE_RTexture*);

SCE_STexData* SCE_RGetTextureTexData (SCE_RTexture*, int, int);

int SCE_RHasTextureData (SCE_RTexture*);

int SCE_RIsTextureUsingMipmaps (SCE_RTexture*);
int SCE_RGetTextureNumMipmaps (SCE_RTexture*, int);
int SCE_RGetTextureWidth (SCE_RTexture*, int, int);
int SCE_RGetTextureHeight (SCE_RTexture*, int, int);

int SCE_RGetTextureValidSize (int, int);

void SCE_RResizeTextureImage (SCE_SImage*, int, int, int);

int SCE_RAddTextureImage (SCE_RTexture*, int, SCE_SImage*, int);

void SCE_RAddTextureTexData (SCE_RTexture*, int, SCE_STexData*);
SCE_STexData* SCE_RAddTextureTexDataDup (SCE_RTexture*, int, SCE_STexData*);

SCE_SImage* SCE_RRemoveTextureImage (SCE_RTexture*, int, int);
void SCE_REraseTextureImage (SCE_RTexture*, int, int);

SCE_STexData* SCE_RRemoveTextureTexData (SCE_RTexture*, int, int);
void SCE_REraseTextureTexData (SCE_RTexture*, int, int);

SCE_RTexture* SCE_RLoadTexturev (int, int, int, int, int, const char**);
SCE_RTexture* SCE_RLoadTexture (int, int, int, int, int, ...);

int SCE_RBuildTexture (SCE_RTexture*, int, int);

int SCE_RUpdateTexture (SCE_RTexture*, int, int);

void SCE_RSetActiveTextureUnit (unsigned int);

void SCE_RUseTexture (SCE_RTexture*, int);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
