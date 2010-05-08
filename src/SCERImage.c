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

#include <SCE/utils/SCEUtils.h>
#include "SCE/renderer/SCEGLee.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "SCE/renderer/SCERType.h"
#include "SCE/renderer/SCERSupport.h"
#include "SCE/renderer/SCERImage.h"

/**
 * \file SCERImage.c
 * \copydoc rimage
 * 
 * \file SCERImage.h
 * \copydoc rimage
 */

/**
 * \defgroup rimage Images managment using DevIL library
 * \ingroup renderer
 * \internal
 * \brief Interfacing the DevIL's API and implement some features to be ready
 * for the core textures manager \c coretexture
 */

/** @{ */

static int resource_type = 0;
/*static SCEenum resize_filter = ILU_NEAREST;*/ /* unused */

#define SCE_IMG_FORCE_PONCTUAL 1
#define SCE_IMG_FORCE_PERSISTENT 2

static int rescaleforced = SCE_FALSE;
static float scale_w, scale_h, scale_d;

static int resizeforced = SCE_FALSE;
static int size_w, size_h, size_d;


/**
 * \brief Initializes the images manager
 * \returns SCE_OK on success, SCE_ERROR on error
 */
int SCE_RImageInit (void)
{
    /* initialisation de DevIL */
    ilInit ();
    iluInit ();

    ilEnable (IL_ORIGIN_SET);
    ilOriginFunc (IL_ORIGIN_LOWER_LEFT);
    /* on sauvegarde les donnees compressees */
    ilSetInteger (IL_KEEP_DXTC_DATA, IL_TRUE);

    resource_type = SCE_Resource_RegisterType (SCE_TRUE, NULL, NULL);
    if (resource_type < 0)
        goto fail;
    if (SCE_Media_Register (resource_type,
                            ".bmp .gif .jpg .dds .png .tga .jpeg .ico .mn"
                            "g .pcx .rgb .rgba .tif", SCE_RLoadImage, NULL) < 0)
        goto fail;
    return SCE_OK;
fail:
    SCEE_LogSrc ();
    SCEE_LogSrcMsg ("failed to initialize images manager");
    return SCE_ERROR;
}

/**
 * \brief Quit the images manager
 */
void SCE_RImageQuit (void)
{
    ilShutDown ();
}


int SCE_RGetImageResourceType (void)
{
    return resource_type;
}


static void SCE_RBindImage (SCE_RImage *img)
{
    static SCE_RImage *bound = NULL;
    if (!img)
        ilBindImage (0);
    else if (img != bound) {
        ilBindImage (0);
        ilBindImage (img->id);
        ilActiveMipmap (img->level);
    }
    bound = img;
}


static void SCE_RInitImageData (SCE_RImageData *d)
{
    d->free_data = SCE_FALSE;
    d->data = NULL;
    d->pxf = IL_RGBA;
    d->updated = SCE_FALSE;
    SCE_List_InitIt (&d->it);
    SCE_List_SetData (&d->it, d);
}

static SCE_RImageData* SCE_RCreateImageData (void)
{
    SCE_RImageData *d = NULL;

    d = SCE_malloc (sizeof *d);
    if (!d)
        SCEE_LogSrc ();
    else
        SCE_RInitImageData (d);
    return d;
}

static void SCE_RDeleteImageData (void *data)
{
    if (data) {
        SCE_RImageData *d = data;
        if (d->free_data)
            SCE_free (d->data);
        SCE_free (d);
    }
}


static void SCE_RInitImage (SCE_RImage *img)
{
    img->id = 0;
    SCE_List_Init (&img->mipmaps);
    SCE_List_SetFreeFunc (&img->mipmaps, SCE_RDeleteImageData);
    img->level = 0;
    img->data = NULL;
}

/**
 * \brief Creates a new image
 * \returns a pointer to the new image structure
 */
SCE_RImage* SCE_RCreateImage (void)
{
    SCE_RImage *img = NULL;

    img = SCE_malloc (sizeof *img);
    if (!img) {
        SCEE_LogSrc ();
        return NULL;
    }
    SCE_RInitImage (img);
    ilGenImages (1, &img->id);
    return img;
}
/**
 * \brief Deletes an image
 * \param img image to delete
 */
void SCE_RDeleteImage (SCE_RImage *img)
{
    if (img) {
        ilBindImage (0);
        SCE_List_Clear (&img->mipmaps);
        ilDeleteImages (1, &img->id);
        SCE_free (img);
    }
}


/**
 * \brief Force image resizing at loading
 * \param persistent defines if the resizing is persistent or not, set SCE_TRUE
 * or SCE_FALSE
 * \param w,h,d news dimensions to set
 * \sa SCE_RForceImageRescale()
 * 
 * To avoid resizing, so... I don't know how you do :D
 */
void SCE_RForceImageResize (int persistent, int w, int h, int d)
{
    rescaleforced = SCE_FALSE; /* on desactive l'autre */
    resizeforced = (persistent ? SCE_IMG_FORCE_PERSISTENT :
                    SCE_IMG_FORCE_PONCTUAL);
    size_w = w; size_h = h; size_d = d;
}
/**
 * \brief Force image rescaling at loading
 * \param persistent defines if the rescaling is persistent or not, set SCE_TRUE
 * or SCE_FALSE
 * \param w,h,d news scales to set
 * \sa SCE_RForceImageResize()
 * 
 * To avoid the rescaling, so... I don't know how you do :D
 */
void SCE_RForceImageRescale (int persistent, float w, float h, float d)
{
    resizeforced = SCE_FALSE; /* on desactive l'autre */
    rescaleforced = (persistent ? SCE_IMG_FORCE_PERSISTENT :
                     SCE_IMG_FORCE_PONCTUAL);
    scale_w = w; scale_h = h; scale_d = d;
}

/**
 * \brief Get the number of mipmap levels of an image
 * \param img the image
 * \returns the number of mipmap levels
 */
unsigned int SCE_RGetImageNumMipmaps (SCE_RImage *img)
{
    return SCE_List_GetSize (&img->mipmaps);
}

/**
 * \brief Indicates if an image contains mipmap levels
 * \returns a boolean
 */
int SCE_RHaveImageMipmaps (SCE_RImage *img)
{
    return (SCE_List_GetLength (&img->mipmaps) > 1) ? 1 : 0;
}

/**
 * \brief Get the number of the active mipmap level
 */
unsigned int SCE_RGetImageMipmapLevel (SCE_RImage *img)
{
    return img->level;
}


/* retourne les donnees du niveau de mipmap actif */
static SCE_RImageData* SCE_RGetImageCurrentMipmapData (SCE_RImage *img)
{
    SCE_SListIterator *it = NULL;

    it = SCE_List_GetIterator (&img->mipmaps, img->level);
    if (!it) {
        SCEE_LogSrc ();
        return NULL;
    }
    return SCE_List_GetData (it);
}

/**
 * \brief Set the active mipmap level
 * \param level the mipmap level to activate
 * \returns SCE_OK on success, SCE_ERROR on error
 * \todo fix this fucking hack
 */
int SCE_RSetImageMipmapLevel (SCE_RImage *img, unsigned int level)
{
    int max_level;

    SCE_RBindImage (img);
    /*ilBindImage (0);
      ilBindImage (img->id);*/
    /*max_level = SCE_RGetImageNumMipmaps (img);*/
    ilGetIntegerv (IL_NUM_MIPMAPS, &max_level);
    max_level++;
    if (level >= max_level) {
        SCEE_Log (SCE_INVALID_ARG);
        SCEE_LogMsg ("you can't active this mipmap level (%d), the maximum "
                       "mipmap level for this image is %d", level, max_level);
        return SCE_ERROR;
    }
    img->level = level;
    img->data = SCE_RGetImageCurrentMipmapData (img);
    SCE_RBindImage (img);
    return SCE_OK;
}



static int SCE_RIsCompressedPixelFormat (SCEenum fmt)
{
    if (fmt == IL_DXT1 || fmt == IL_DXT2 ||
        fmt == IL_DXT3 || fmt == IL_DXT4 ||
        fmt == IL_DXT5 || fmt == IL_3DC)
        return SCE_TRUE;
    else
        return SCE_FALSE;
}

/**
 * \brief Updates the active mipmap level of an image
 * \returns SCE_OK on success, SCE_ERROR on error
 */
int SCE_RUpdateImageMipmap (SCE_RImage *img)
{
    SCE_RImageData *d = img->data;

    if (!d) {
        SCEE_Log (SCE_INVALID_OPERATION);
        return SCE_ERROR;
    }

    if (d->updated)
        return SCE_OK;

    if (d->free_data)
        SCE_free (d->data);
    d->data = NULL;

    if (SCE_RGetImageIsCompressed (img)) {
        size_t data_size = SCE_RGetImageDataSize (img);
        d->free_data = 1;
        d->data = SCE_malloc (data_size);
        if (!d->data) {
            SCEE_LogSrc ();
            return SCE_ERROR;
        }
        ilGetDXTCData (d->data, data_size, d->pxf);
    } else {
        d->free_data = 0;
        ilConvertImage (d->pxf, SCE_RGetImageDataType (img));
        d->data = ilGetData ();
    }

    d->updated = SCE_TRUE;

    return SCE_OK;
}


/* met a jour la liste chainee des mipmaps */
/**
 * \todo faire en sorte de conserver les formats de pixel precedemment
 *       envoyes pour chaque niveau de mipmap
 */
static int SCE_RUpdateImageMipmapList (SCE_RImage *img)
{
    SCE_RImageData *d = NULL;
    int num_mipmaps, i;

    /* let's activate the first mipmap level
       (I don't trust ilActiveMipmap (0)) */
    ilBindImage (0);
    ilBindImage (img->id);

    SCE_List_Clear (&img->mipmaps);

    ilGetIntegerv (IL_NUM_MIPMAPS, &num_mipmaps);
    num_mipmaps++;  /* parce qu'aucun mipmap = 0 (et moi je veux au moins 1) */
    for (i = 0; i < num_mipmaps; i++) {
        ilBindImage (0);
        ilBindImage (img->id);
        ilActiveMipmap (i);

        d = SCE_RCreateImageData ();
        if (!d) {
            SCEE_LogSrc ();
            return SCE_ERROR;
        }
        ilGetIntegerv (IL_DXTC_DATA_FORMAT, &d->pxf);
        if (d->pxf == IL_DXT_NO_COMP)
            ilGetIntegerv (IL_IMAGE_FORMAT, &d->pxf);
        SCE_List_Appendl (&img->mipmaps, &d->it);
        d = NULL;
    }

    /* si level est trop grand, on le defini
       au niveau de mipmap le plus petit */
    if (img->level >= num_mipmaps)
        img->level = SCE_List_GetIndex (SCE_List_GetLast (&img->mipmaps));

    /* recuperation du niveau de mipmap */
    if (SCE_RSetImageMipmapLevel (img, img->level) < 0) {
        SCEE_LogSrc ();
        return SCE_ERROR;
    }

    return SCE_OK;
}

/**
 * \brief Updates the mipmap levels list of \p img
 * 
 * Clears the mipmap levels list and rebuild it from the DevIL's image
 * informations. The informations are the number of mipmaps and the pixel format
 * or each level.
 * It updates only the list, doesn't call SCE_RUpdateImageMipmap() for each
 * level.
 */
int SCE_RUpdateImage (SCE_RImage *img)
{
    return SCE_RUpdateImageMipmapList (img);
}


#define SCE_RIMGGET(name, ienum)\
int SCE_RGetImage##name (SCE_RImage *img)\
{\
    SCE_RBindImage (img);\
    return ilGetInteger (ienum);\
}

SCE_RIMGGET (Width,     IL_IMAGE_WIDTH)
SCE_RIMGGET (Height,    IL_IMAGE_HEIGHT)
SCE_RIMGGET (Depth,     IL_IMAGE_DEPTH)
SCE_RIMGGET (Format,    IL_IMAGE_FORMAT)
SCE_RIMGGET (PixelSize, IL_IMAGE_BYTES_PER_PIXEL)
SCE_RIMGGET (DataType,  IL_IMAGE_TYPE)

#undef SCE_RIMGGET


/**
 * \brief Gets the type on an image
 * \returns the image type
 */
int SCE_RGetImageType (SCE_RImage *img)
{
    int type = SCE_TEX_1D;
    if (SCE_RGetImageDepth (img) > 1)
        type = SCE_TEX_3D;
    else if (SCE_RGetImageHeight (img) > 1)
        type = SCE_TEX_2D;
    return type;
}

/* TODO: shall be moved as the Image module goes further in core */
/* converti un format de pixel DevIL en format OpenGL */
static int SCE_RConvertPxfIlToGl (int pxf)
{
    switch (pxf) {
        case IL_DXT1:
            return SCE_PXF_DXT1;
        case IL_DXT2:
        case IL_DXT3:
            return SCE_PXF_DXT3;
        case IL_DXT4:
        case IL_DXT5:
            return SCE_PXF_DXT5;
        case IL_3DC:
            return SCE_PXF_3DC;
        default:
            return pxf;
    }
}
/* converti un format de pixel OpenGL en format DevIL */
static int SCE_RConvertPxfGlToIl (int pxf)
{
    switch (pxf) {
        case SCE_PXF_DXT1:
            return IL_DXT1;
        case SCE_PXF_DXT3:
            return IL_DXT3;
        case SCE_PXF_DXT5:
            return IL_DXT5;
        case SCE_PXF_3DC:
            return IL_3DC;
        default:
            return pxf;
    }
}

/**
 * \brief Gets the image pixel format
 * \returns the pixel format \sa defines de merde
 * \todo améliorer cette doc en ajoutant le support des defines
 */
int SCE_RGetImagePixelFormat (SCE_RImage *img)
{
    return SCE_RConvertPxfIlToGl (img->data->pxf);
}


/**
 * \brief Gets the size of the data of the active mipmap level
 * \returns the size of the data (bytes)
 */
size_t SCE_RGetImageDataSize (SCE_RImage *img)
{
    SCE_RBindImage (img);
    if (SCE_RGetImageIsCompressed (img))
        return ilGetDXTCData (NULL, 0, img->data->pxf);
    else
        return ilGetInteger (IL_IMAGE_SIZE_OF_DATA);
}

/**
 * \brief Indicates if the active mipmap level has a compressed pixel format
 * \returns a boolean
 */
int SCE_RGetImageIsCompressed (SCE_RImage *img)
{
    return SCE_RIsCompressedPixelFormat (img->data->pxf);
}

/**
 * \brief Gets the pointer to the data of te active mipmap level
 * \returns the pointer to the data
 */
void* SCE_RGetImageData (SCE_RImage *img)
{
    /* mise a jour du niveau de mipmap actif */
    SCE_RUpdateImageMipmap (img);
    return img->data->data;
}


/**
 * \brief Resizes an image, take its new dimensions
 * \param w the new width
 * \param h the new height
 * \param d the new depth (only for 3D images)
 * \note Dimensions less than 1 are not modified.
 * \sa SCE_RRescaleImage()
 * \todo add filters managment
 */
void SCE_RResizeImage (SCE_RImage *img, int w, int h, int d)
{
    SCE_RBindImage (img);
    if (w < 1)
        w = SCE_RGetImageWidth (img);
    if (h < 1)
        h = SCE_RGetImageHeight (img);
    if (d < 1)
        d = SCE_RGetImageDepth (img);

    iluScale (w, h, d);
    img->data->updated = SCE_FALSE;
}

/**
 * \brief Rescale an image, take its new scales
 * \param w the new width factor
 * \param h the new height factor
 * \param d the new depth factor (only on 3D images)
 * \note Set parameter at 0 at your own risk
 * \sa SCE_RResizeImage()
 * \todo add filters managment
 */
void SCE_RRescaleImage (SCE_RImage *img, float w, float h, float d)
{
    SCE_RBindImage (img);
    w = SCE_RGetImageWidth (img) * w;
    h = SCE_RGetImageHeight (img) * h;
    d = SCE_RGetImageDepth (img) * d;

    iluScale (w, h, d);
    img->data->updated = SCE_FALSE;
}

/**
 * \brief Flip an image, inverse it from the y axis
 * \todo this function do not works, DevIL can't flip an image (pd)
 */
void SCE_RFlipImage (SCE_RImage *img)
{
    SCE_RBindImage (img);
    iluFlipImage (); /* TODO: seg fault §§ */
    img->data->updated = SCE_FALSE;
}


/* repete une operation pour chaque niveau de mipmap */
#define SCE_RSETALLMIPMAPS(action)\
{\
    unsigned int level = SCE_RGetImageMipmapLevel (img), i, n;  \
    n = SCE_RGetImageNumMipmaps (img);                          \
    for (i = 0; i < n; i++) {                                   \
        if (SCE_RSetImageMipmapLevel (img, i) < 0) {            \
            SCEE_LogSrc ();                                     \
            break;                                              \
        }                                                       \
        action;                                                 \
        i++;                                                    \
    }                                                           \
    SCE_RSetImageMipmapLevel (img, level);                      \
}

/**
 * \brief Sets the pixel format of the active mipmap level of \p img
 * \param fmt the new pixel format
 * 
 * \sa SCE_RSetImageAllPixelFormat()
 */
void SCE_RSetImagePixelFormat (SCE_RImage *img, SCEenum fmt)
{
    img->data->pxf = SCE_RConvertPxfGlToIl (fmt);
    img->data->updated = SCE_FALSE;
}
/**
 * \brief Sets the pixel format of all mipmap levels of \p img
 * \param fmt the new pixel format
 * 
 * \sa SCE_RSetImagePixelFormat()
 */
void SCE_RSetImageAllPixelFormat (SCE_RImage *img, SCEenum fmt)
{
    SCE_RBindImage (img);
    SCE_RSETALLMIPMAPS (SCE_RSetImagePixelFormat (img, fmt))
}


/**
 * \brief Makes mipmap levels for an image based on its first image
 * \returns SCE_ERROR on error, SCE_OK otherwise
 */
int SCE_RBuildImageMipmaps (SCE_RImage *img)
{
    SCE_RBindImage (img);
    /* ilActiveLevel (0) ? */
    if (iluBuildMipmaps () == IL_FALSE) {
        SCEE_Log (SCE_INVALID_OPERATION);
        SCEE_LogMsg ("DevIL fails to build mipmaps: %s",
                     iluErrorString (ilGetError ()));
        return SCE_ERROR;
    }

    /* update mipmaps list */
    SCE_RUpdateImage (img);

    return SCE_OK;
}


/**
 * \brief Loads a new image. This function is the callback for the media manager
 * \returns a new SCE_RImage*
 */
void* SCE_RLoadImage (FILE *fp, const char *fname, void *unused)
{
    SCE_RImage *img = NULL;
    (void)unused;
    img = SCE_RCreateImage ();
    if (!img) {
        SCEE_LogSrc ();
        return NULL;
    }

    SCE_RBindImage (img);

    if (!ilLoadImage ((char*)fname)) {
        SCEE_Log (SCE_INVALID_OPERATION);
        SCEE_LogMsg ("DevIL can't load '%s': %s",
                     fname, iluErrorString (ilGetError ()));
        SCE_RDeleteImage (img);
        return NULL;
    }

    /* application des redimensions */
    if (resizeforced)
        SCE_RResizeImage (img, size_w, size_h, size_d);
    else if (rescaleforced)
        SCE_RRescaleImage (img, scale_w, scale_h, scale_d);

    if (SCE_RUpdateImage (img) < 0) {
        SCEE_LogSrc ();
        SCE_RDeleteImage (img);
        return NULL;
    }

    /* annulation si la demande etait ponctuelle */
    if (resizeforced == SCE_IMG_FORCE_PONCTUAL)
        resizeforced = SCE_FALSE;
    else if (rescaleforced == SCE_IMG_FORCE_PONCTUAL)
        rescaleforced = SCE_FALSE;

    return img;
}

/** @} */
