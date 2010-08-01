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
 
/* created: 28/07/2007
   updated: 18/04/2010 */

#ifndef SCERIMAGE_H
#define SCERIMAGE_H

#include <GL/glew.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <SCE/utils/SCEUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Types of texture
 * \todo usage of GL macros in a header
 */
enum sce_rtextype {
    SCE_TEX_1D = GL_TEXTURE_1D,
    SCE_TEX_2D = GL_TEXTURE_2D,
    SCE_TEX_3D = GL_TEXTURE_3D,
    SCE_TEX_CUBE = GL_TEXTURE_CUBE_MAP
};
typedef enum sce_rtextype SCE_RTexType;

enum sce_rpixelformat {
    SCE_PXF_LUMINANCE = GL_LUMINANCE,
    SCE_PXF_LUMINANCE_ALPHA = GL_LUMINANCE_ALPHA,
    SCE_PXF_RGB = GL_RGB,
    SCE_PXF_RGBA = GL_RGBA,
    SCE_PXF_BGR = GL_BGR,
    SCE_PXF_BGRA = GL_BGRA,
    SCE_PXF_DXT1 = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
    SCE_PXF_DXT3 = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
    SCE_PXF_DXT5 = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
};
typedef enum sce_rpixelformat SCE_RPixelFormat;

/* TODO: tmp hack, actually GLee didn't support the extension */
#define SCE_PXF_3DC /*GL_COMPRESSED_RGB_3DC_ATI*/ 0

/**
 * \brief An image data
 * 
 * Contains the data of one image's mipmap level.
 */
typedef struct sce_rimagedata SCE_RImageData;
struct sce_rimagedata {
    int free_data;  /**< Boolean, true = can free \c data */
    void *data;     /**< Raw data */
    int pxf;        /**< Pixel format */
    int updated;    /**< Boolean, true when \c data is up to date */
    SCE_SListIterator it;
};

/**
 * \brief An SCE image
 * 
 * Contains an image with his own data stored into mipmap levels. It stores
 * too a DevIL identifier to manage all the image's data.
 */
typedef struct sce_rimage SCE_RImage;
struct sce_rimage {
    ILuint id;            /**< DevIL's identifier */
    SCE_SList mipmaps;    /**< All mipmap levels \sa SCE_RImageData  */
    unsigned int level;   /**< Activated mipmap level */
    SCE_RImageData *data; /**< Activated mipmap level's data */
};


int SCE_RImageInit (void);
void SCE_RImageQuit (void);

int SCE_RGetImageResourceType (void);

SCE_RImage* SCE_RCreateImage (void);
void SCE_RDeleteImage (SCE_RImage*);

void SCE_RForceImageResize (int, int, int, int);
void SCE_RForceImageRescale (int, float, float, float);

unsigned int SCE_RGetImageNumMipmaps (SCE_RImage*);

int SCE_RHaveImageMipmaps (SCE_RImage*);

unsigned int SCE_RGetImageMipmapLevel (SCE_RImage*);

int SCE_RSetImageMipmapLevel (SCE_RImage*, unsigned int);

int SCE_RUpdateImageMipmap (SCE_RImage*);

int SCE_RUpdateImage (SCE_RImage*);

int SCE_RGetImageWidth (SCE_RImage*);
int SCE_RGetImageHeight (SCE_RImage*);
int SCE_RGetImageDepth (SCE_RImage*);
int SCE_RGetImageFormat (SCE_RImage*);
int SCE_RGetImagePixelSize (SCE_RImage*);
int SCE_RGetImageDataType (SCE_RImage*);

int SCE_RGetImageType (SCE_RImage*);
int SCE_RGetImagePixelFormat (SCE_RImage*);
size_t SCE_RGetImageDataSize (SCE_RImage*);

int SCE_RGetImageIsCompressed (SCE_RImage*);

void* SCE_RGetImageData (SCE_RImage*);

/* what define?? */
#define SCE_IMAGE_DO_NOT_CHANGE 0
void SCE_RResizeImage (SCE_RImage*, int, int, int);
void SCE_RRescaleImage (SCE_RImage*, float, float, float);

void SCE_RFlipImage (SCE_RImage*);

void SCE_RSetImagePixelFormat (SCE_RImage*, SCEenum);
void SCE_RSetImageAllPixelFormat (SCE_RImage*, SCEenum);

int SCE_RBuildImageMipmaps (SCE_RImage*);

void* SCE_RLoadImage (FILE*, const char*, void*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
