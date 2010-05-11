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
 
/* created: 14/01/2007
   updated: 18/04/2010 */

#include <string.h>
#include <SCE/utils/SCEUtils.h>
#include "SCE/renderer/SCERType.h"
#include "SCE/renderer/SCERSupport.h"
#include "SCE/renderer/SCERTexture.h"

/**
 * \file SCERTexture.c
 * \copydoc rtexture
 * 
 * \file SCERTexture.h
 * \copydoc rtexture
 */

/**
 * \defgroup rtexture Texture
 * \ingroup renderer
 * \internal
 * \brief Functions that gives a full support of textures managment
 */

/** @{ */

static int resource_type = 0;

#if 0
static unsigned int nbatchs = 0;

unsigned int SCE_RGetTextureNumBatchs (void)
{
    unsigned int n = nbatchs;
    nbatchs = 0;
    return n;
}
#endif

/* TODO: french spotted. */
/* booleen: true = reduction des images lors d'une redimension automatique */
static int sce_tex_reduce = SCE_TRUE;

static int texsub = 0;  /* indique glTexImage (0) ou glTexSubImage (1) */

/* stocke les textures utilisees (via Use) */
static SCE_RTexture **texused = NULL;

static int max_tex_units = 0; /* nombre maximum d'unites de texture */

static int max_dimensions = 64; /* dimensions maximales des textures 2D et 1D */
static int max_cube_dimensions = 16;
static int max_3d_dimensions = 16;
static int max_mipmap_level = 1; /* niveau de mipmap maximum par defaut */

/* force certaines valeurs lors d'un appel a AddTexData() */
static int force_pxf = SCE_FALSE, forced_pxf;
static int force_type = SCE_FALSE, forced_type;
static int force_fmt = SCE_FALSE, forced_fmt;


/* nombre de textures utilisees de chaque type */
static int n_textype[4];

static void* SCE_RLoadTextureResource (const char*, int, void*);

/**
 * \internal
 * \brief Initializes the textures manager
 */
int SCE_RTextureInit (void)
{
    size_t i;

    /* recuperation du nombre maximum d'unites de texture */
    SCE_RGetIntegerv (GL_MAX_TEXTURE_UNITS, &max_tex_units);
    /* recuperation de la taille maximale d'une texture */
    SCE_RGetIntegerv (GL_MAX_TEXTURE_SIZE, &max_dimensions);
    SCE_RGetIntegerv (GL_MAX_CUBE_MAP_TEXTURE_SIZE, &max_cube_dimensions);
    SCE_RGetIntegerv (GL_MAX_3D_TEXTURE_SIZE, &max_3d_dimensions);
    /* ...et assignation au mipmap maximum */
    max_mipmap_level = SCE_Math_PowerOfTwo (max_dimensions);

    /* creation du tableau de stockage des textures en cours d'utilisation */
    texused = SCE_malloc (max_tex_units * sizeof *texused);
    if (!texused)
        goto fail;
    for (i = 0; i < max_tex_units; i++)
        texused[i] = NULL;
    resource_type = SCE_Resource_RegisterType (SCE_FALSE,
                                               SCE_RLoadTextureResource, NULL);
    if (resource_type < 0)
        goto fail;

    SCE_btend ();
    return SCE_OK;
fail:
    SCEE_LogSrc ();
    SCEE_LogSrcMsg ("failed to initialize textures manager");
    return SCE_ERROR;
}

/**
 * \internal
 * \brief Quits the textures manager
 */
void SCE_RTextureQuit (void)
{
    SCE_free (texused);
    texused = NULL;
}


int SCE_RGetTextureResourceType (void)
{
    return resource_type;
}


/**
 * \brief Non-reentrant function
 */
static void SCE_RBindTexture (SCE_RTexture *tex)
{
    static SCE_RTexture *bound = NULL;
    if (tex != bound) {
        bound = tex;
        glBindTexture (bound->target, bound->id);
    }
}


/**
 * \brief Initializes a texture data structure
 */
void SCE_RInitTexData (SCE_RTexData *d)
{
    d->img = NULL;
    d->canfree = SCE_FALSE;
    d->user = SCE_TRUE;
    d->target = SCE_TEX_1D;
    d->level = 0;
    d->w = d->h = d->d = 1;
    d->pxf = d->fmt = GL_RGBA;
    d->type = SCE_UNSIGNED_BYTE;
    d->data_size = 0;
    d->data_user = SCE_TRUE;
    d->data = NULL;
    d->comp = SCE_FALSE;
    SCE_List_InitIt (&d->it);
    SCE_List_SetData (&d->it, d);
}

/**
 * \brief Creates a texture data structure
 * \returns the new texture data
 */
SCE_RTexData* SCE_RCreateTexData (void)
{
    SCE_RTexData *d = NULL;
    d = SCE_malloc (sizeof *d);
    if (!d)
        SCEE_LogSrc ();
    else
        SCE_RInitTexData (d);
    return d;
}

/**
 * \brief Creates a texture data from the bound SCE image
 * \returns the new texture data
 * \sa SCE_RImage
 */
SCE_RTexData* SCE_RCreateTexDataFromImage (SCE_RImage *img)
{
    SCE_RTexData *d = NULL;

    d = SCE_RCreateTexData ();
    if (!d) {
        SCEE_LogSrc ();
        return NULL;
    }

    d->data_size = SCE_RGetImageDataSize (img);
    d->data_user = SCE_FALSE;
    d->data = SCE_malloc (d->data_size);
    if (!d->data) {
        SCE_free (d);
        SCEE_LogSrc ();
        return NULL;
    }
    memcpy (d->data, SCE_RGetImageData (img), d->data_size);
    d->img = img;
    d->target = SCE_RGetImageType (img);
    d->level = SCE_RGetImageMipmapLevel (img);
    d->w = SCE_RGetImageWidth (img);
    d->h = SCE_RGetImageHeight (img);
    d->d = SCE_RGetImageDepth (img);
    d->pxf = SCE_RGetImagePixelFormat (img);
    d->fmt = SCE_RGetImageFormat (img);
    d->type = SCE_RGetImageDataType (img);
    d->comp = SCE_RGetImageIsCompressed (img);

    return d;
}

/**
 * \brief Deletes a texture data
 */
void SCE_RDeleteTexData (void *data)
{
    if (data) {
        SCE_RTexData *d = data;
        if (!d->data_user)
            SCE_free (d->data);
        if (d->canfree && SCE_Resource_Free (d->img))
            SCE_RDeleteImage (d->img);
        SCE_free (d);
    }
}
static void SCE_RDeleteTexData_ (void *data)
{
    if (data) {
        SCE_RTexData *d = data;
        if (!d->user) {
            SCE_free (d->data);
            if (d->canfree && SCE_Resource_Free (d->img))
                SCE_RDeleteImage (d->img);
            SCE_free (d);
        }
    }
}

/**
 * \brief Duplicates a texture data
 * \param d the texture data to copy
 * \returns the copy of \p d
 */
SCE_RTexData* SCE_RDupTexData (SCE_RTexData *d)
{
    SCE_RTexData *data = NULL;

    data = SCE_RCreateTexData ();
    if (!data) {
        SCEE_LogSrc ();
        return NULL;
    }

    *data = *d;
    /* NOTE: bouh bouh ugly */
    SCE_List_InitIt (&data->it);
    SCE_List_SetData (&data->it, data);
    /* and what about data->user, data->data_user ? */

    if (data->data_size > 0) {
        data->data = SCE_malloc (data->data_size);
        if (!data->data) {
            SCE_free (data);
            SCEE_LogSrc ();
            return NULL;
        }
        memcpy (data->data, d->data, data->data_size);
    }
    return data;
}



static void SCE_RInitTexture (SCE_RTexture *tex)
{
    unsigned int i;
    tex->id = 0;
    tex->target = 0;
    tex->have_data = SCE_FALSE;
    tex->use_mipmap = tex->hw_mipmap = SCE_FALSE;
    for (i = 0; i < 6; i++) {
        SCE_List_Init (&tex->data[i]);
        SCE_List_SetFreeFunc (&tex->data[i], SCE_RDeleteTexData_);
    }
}

/**
 * \brief Creates a core texture
 * \param target the type of the texture to create (can set SCE_TEX_1D,
 * SCE_TEX_2D, SCE_TEX_3D or SCE_TEX_CUBE)
 * \note If \p target is a non valid target, then... what ?
 * \todo que fait cette fonction quand \p target est non valide ?
 */
SCE_RTexture* SCE_RCreateTexture (SCEenum target)
{
    unsigned int i;
    SCE_RTexture *tex = NULL;

    tex = SCE_malloc (sizeof *tex);
    if (!tex) {
        SCEE_LogSrc ();
        return NULL;
    }
    SCE_RInitTexture (tex);

    tex->target = target;
    /* assignation du type */
    switch (target) {
    case SCE_TEX_1D: tex->type = SCE_TEXTYPE_1D; break;
    case SCE_TEX_2D: tex->type = SCE_TEXTYPE_2D; break;
    case SCE_TEX_3D: tex->type = SCE_TEXTYPE_3D; break;
    case SCE_TEX_CUBE: tex->type = SCE_TEXTYPE_CUBE;
    }

    glGenTextures (1, &tex->id);

    return tex;
}


/**
 * \brief Deletes a core texture
 * \param tex the texture to delete
 */
void SCE_RDeleteTexture (SCE_RTexture *tex)
{
    if (tex) {
        unsigned int i;
        for (i = 0; i < 6; i++)
            SCE_List_Clear (&tex->data[i]);
        glDeleteTextures (1, &tex->id);
        SCE_free (tex);
    }
}


/**
 * \brief Gets the number of texture units available
 * \returns the number of texture units
 */
int SCE_RGetMaxTextureUnits (void)
{
    return max_tex_units;
}
/**
 * \brief Gets the maximum textures size
 */
int SCE_RGetMaxTextureSize (void)
{
    return max_dimensions;
}
/**
 * \brief Gets the maximum textures cubemap size
 */
int SCE_RGetMaxTextureCubeSize (void)
{
    return max_cube_dimensions;
}
/**
 * \brief Gets the maximum 3D textures size
 */
int SCE_RGetMaxTexture3DSize (void)
{
    return max_3d_dimensions;
}


/**
 * \brief Sets the filter of a texture when it goes far away from the view point
 * \param level is the level of filtering, can set SCE_TEX_NEAREST,
 * SCE_TEX_LINEAR, SCE_TEX_BILINEAR, SCE_TEX_TRILINEAR.
 *
 * \sa SCE_RPixelizeTexture()
 */
void SCE_RSetTextureFilter (SCE_RTexture *tex, SCEint filter)
{
    SCE_RSetTextureParam (tex, GL_TEXTURE_MIN_FILTER, filter);
}

/**
 * \brief Defines if a texture is pixelized when is near
 * \param p can be SCE_TRUE or SCE_FALSE
 *
 * \sa SCE_RSetTextureFilter()
 */
void SCE_RPixelizeTexture (SCE_RTexture *tex, int p)
{
    SCE_RSetTextureParam (tex, GL_TEXTURE_MAG_FILTER,
                          (p ? GL_NEAREST : GL_LINEAR));
}

/**
 * \deprecated
 * \brief Sets a parameter to a texture
 * \param tex the texture to assign the parameter
 * \param pname the type of the parameter
 * \param param the value of the parameter
 *
 * This function calls glTexParameteri()
 *
 * \sa SCE_RSetTextureParamf()
 * \todo this function is GL-specific, so remove it.
 */
void SCE_RSetTextureParam (SCE_RTexture *tex, SCEenum pname, SCEint param)
{
    SCE_RBindTexture (tex);
    glTexParameteri (tex->target, pname, param);
}

/**
 * \deprecated
 * \brief Sets a parameter to a texture
 * \param tex the texture to assign the parameter
 * \param pname the type of the parameter
 * \param param the value of the parameter
 *
 * This function calls glTexParameterf()
 *
 * \sa SCE_RSetTextureParam()
 * \todo this function is GL-specific, so remove it.
 */
void SCE_RSetTextureParamf (SCE_RTexture *tex, SCEenum pname, SCEfloat param)
{
    SCE_RBindTexture (tex);
    glTexParameterf (tex->target, pname, param);
}

/**
 * \deprecated
 * \brief Sets the parameters of texture coordinates generation
 * 
 * This function calls glTexGeni(\p a, \p b, \p c). See the documentation of
 * this GL function for more details.
 *
 * \sa SCE_RSetTextureGenf() SCE_RSetTextureGenfv()
 * \todo this function is GL-specific, so remove it.
 */
void SCE_RSetTextureGen (SCE_RTexture *tex, SCEenum a, SCEenum b, int c)
{
    SCE_RBindTexture (tex);
    glTexGeni (a, b, c);
}

/**
 * \deprecated
 * \brief Sets the parameters of texture coordinates generation
 * 
 * This function calls glTexGenf(\p a, \p b, \p c). See the documentation of
 * this GL function for more details.
 *
 * \sa SCE_RSetTextureGen() SCE_RSetTextureGenfv()
 * \todo this function is GL-specific, so remove it.
 */
void SCE_RSetTextureGenf (SCE_RTexture *tex, SCEenum a, SCEenum b, float c)
{
    SCE_RBindTexture (tex);
    glTexGenf (a, b, c);
}

/**
 * \deprecated
 * \brief Sets the parameters of texture coordinates generation
 * 
 * This function calls glTexGenfv(\p a, \p b, \p c). See the documentation of
 * this GL function for more details.
 *
 * \sa SCE_RSetTextureGen() SCE_RSetTextureGenf()
 * \todo this function is GL-specific, so remove it.
 */
void SCE_RSetTextureGenfv (SCE_RTexture *tex, SCEenum a, SCEenum b, float *c)
{
    SCE_RBindTexture (tex);
    glTexGenfv (a, b, c);
}

/**
 * \brief Forces the pixel format when calling SCE_RAddTextureTexData()
 * \param force do we force the pixel format ?
 * \param pxf forced pixel format
 * \warning this function has side-effets; it changes local static variables
 */
void SCE_RForceTexturePixelFormat (int force, int pxf)
{
    force_pxf = force;
    forced_pxf = pxf;
}
/**
 * \brief Forces the type when calling SCE_RAddTextureTexData()
 * \param force do we force the type ?
 * \param type forced type
 * \warning this function has side-effets; it changes local static variables
 */
void SCE_RForceTextureType (int force, int type)
{
    force_type = force;
    forced_type = type;
}
/**
 * \brief Forces the format when calling SCE_RAddTextureTexData()
 * \param force do we force the format ?
 * \param fmt forced format
 * \warning this function has side-effets; it changes local static variables
 */
void SCE_RForceTextureFormat (int force, int fmt)
{
    force_fmt = force;
    forced_fmt = fmt;
}


/**
 * \brief Gets the type of a core texture
 * \returns the type of \p tex
 */
SCEenum SCE_RGetTextureTarget (SCE_RTexture *tex)
{
    return tex->target;
}


/* TODO: wat? edit comment */
/* retourne un identifiant valide pour le tableau des donnees
   et assigne a 'target' la valeur adequat */
static unsigned int SCE_RGetTextureTargetID (SCE_RTexture *tex, int *target)
{
    unsigned int i = 0;
    if (*target >= SCE_TEX_POSX && *target <= SCE_TEX_NEGZ &&
        tex->target == SCE_TEX_CUBE)
        i = *target - SCE_TEX_POSX;
    else
        *target = tex->target;
    return i;
}


/**
 * \brief Gets the texture's data of the specified target and mipmap level
 * \param target target of the texture, used only for cubemaps, determines
 * the cube face, can be 0
 * \param level mipmap level of the asked data
 * \returns the texture's data corresponding to the given parameters
 * \sa SCE_RTexData
 */
SCE_RTexData* SCE_RGetTextureTexData (SCE_RTexture *tex, int target, int level)
{
    unsigned int i;
    SCE_RTexData *data = NULL;

    i = SCE_RGetTextureTargetID (tex, &target);
    if (level > SCE_List_GetSize (&tex->data[i]))
        data = SCE_List_GetData (SCE_List_GetLast (&tex->data[i]));
    else
        data = SCE_List_GetData (SCE_List_GetIterator (&tex->data[i], level));
    return data;
}

/**
 * \brief Indicates if a core texture has any data
 */
int SCE_RHasTextureData (SCE_RTexture *tex)
{
    return tex->have_data;
}

/**
 * \brief Indicates if a texture is using mipmapping
 */
int SCE_RIsTextureUsingMipmaps (SCE_RTexture *tex)
{
    return tex->use_mipmap;
}

/**
 * \brief Gets the number of mipmap levels of \p tex
 * \param target used only for cubemaps, determines the cube face, can be 0
 * \returns the number of mipmap levels of \p tex
 */
int SCE_RGetTextureNumMipmaps (SCE_RTexture *tex, int target)
{
    unsigned int i;
    i = SCE_RGetTextureTargetID (tex, &target);
    return SCE_List_GetSize (&tex->data[i]);
}

/**
 * \brief Gets the width of a texture
 *
 * This function returns SCE_RGetTextureTexData (\p tex, \p target, \p level)->w
 */
int SCE_RGetTextureWidth (SCE_RTexture *tex, int target, int level)
{
    return SCE_RGetTextureTexData (tex, target, level)->w;
}
/**
 * \brief Gets the height of a texture
 *
 * This function returns SCE_RGetTextureTexData (\p tex, \p target, \p level)->h
 */
int SCE_RGetTextureHeight (SCE_RTexture *tex, int target, int level)
{
    return SCE_RGetTextureTexData (tex, target, level)->h;
}


/**
 * \brief Gets hardware compatible size from anything texture's size
 * \param min set as SCE_TRUE, the returned value is lesser than \p s,
 * greather otherwise
 * \param s the initial size
 * \returns hardware compatible size that is the most closer of \p s
 */
int SCE_RGetTextureValidSize (int min, int s)
{
    if (s > max_dimensions)
        return max_dimensions;
    if (!SCE_RHasCap (SCE_TEX_NON_POWER_OF_TWO)) {
        int tmp = 1;
        while (tmp < s)
            tmp *= 2;
        if (min)
            tmp /= 2;
        s = tmp;
    }
    return s;
}


/**
 * \brief Resizes an image with hardware compatibles dimensions
 * \param img the image to resize
 * \param w the new width (0 or lesser keep the current value)
 * \param h the new height (0 or lesser keep the current value)
 * \param d the new depth (0 or lesser keep the current value)
 *
 * Calls SCE_RResizeImage() over \p img after the check of \p w, \p h and \p d
 * with SCE_RGetTextureValidSize().
 */
void SCE_RResizeTextureImage (SCE_RImage *img, int w, int h, int d)
{
    /* on verifie, pour chaque composante, si elle a ete specifiee */
    if (w <= 0)
        w = SCE_RGetImageWidth (img);
    if (h <= 0)
        h = SCE_RGetImageHeight (img);
    if (d <= 0)
        d = SCE_RGetImageDepth (img);
    /* recuperation de tailles valides */
    /* -> ici SCE_TRUE indique qu'on recupere la dimension inferieur */
    w = SCE_RGetTextureValidSize (sce_tex_reduce, w);
    h = SCE_RGetTextureValidSize (sce_tex_reduce, h);
    d = SCE_RGetTextureValidSize (sce_tex_reduce, d);
    /* mise a l'echelle (uniquement si besoin est) */
    if (w != SCE_RGetImageWidth (img) || h != SCE_RGetImageHeight (img) ||
        d != SCE_RGetImageDepth (img))
        SCE_RResizeImage (img, w, h, d);
}


/**
 * \brief Adds an image to a texture
 * \param tex the texture where add the image
 * \param target the target where bind the image
 * \param img the image to add
 * \param canfree can \p tex's deletion deletes \p img ?
 *
 * Add an image to a texture and bind it as \p target, this parameter can be 0
 * then is automatically set, or can be SCE_TEX_nD for n dimension texture or
 * SCE_TEX_POSX + n where n is the cube face of the cubemap (requires that
 * \p tex is a cubemap). \p img is automatically resized to hardware compatibles
 * dimentions.
 */
int SCE_RAddTextureImage (SCE_RTexture *tex, int target,
                          SCE_RImage *img, int canfree)
{
    SCE_RTexData *d = NULL;
    unsigned int i, j, n_mipmaps;
    int old_level;

    /* verification du target */
    i = SCE_RGetTextureTargetID (tex, &target);

    old_level = SCE_RGetImageMipmapLevel (img);

    /* verification des dimensions de l'image */
    SCE_RResizeTextureImage (img, 0, 0, 0);
#if 0
    /* log de l'operation */
    SCEE_SendMsg ("your hardware doesn't support non-power of two "
                  "textures.\n%s of '%s' forced.\nnew dimentions: "
                  "%d*%d\n", sce_img_reduce ? "shrinkage":"extention",
                  fname, w, h);
#endif

    /**
     * \todo fucking hack de merde, le mipmapping DDS chie "un peu"
     *       avec DevIL, le nombre de mipmap semble changer...
     */
    n_mipmaps = /*SCE_RGetImageNumMipmaps (img)*/ 1;
    /* assignation des donnees de l'image */
    for (j = 0; j < n_mipmaps; j++) {
        SCE_RSetImageMipmapLevel (img, j);
        /* creation des donnees a partir du niveau de mipmap j de l'image img */
        d = SCE_RCreateTexDataFromImage (img);
        if (!d) {
            SCEE_LogSrc ();
            return SCE_ERROR;
        }
        d->target = target;  /* assignation du target */
        d->canfree = canfree;
        d->user = SCE_FALSE;

        SCE_List_Appendl (&tex->data[i], &d->it);
        d = NULL;
    }

    SCE_RSetImageMipmapLevel (img, old_level);
    tex->have_data = SCE_TRUE;
    return SCE_OK;
}


/**
 * \brief Adds a new texture data to a texture
 * \param target the target where bind the new data
 * \param d the new data
 * \param canfree defines if \p d can be deleted on \p tex deletion
 *
 * If this function fails, then the forced values will not set.
 * 
 * \sa SCE_RAddTextureTexDataDup() SCE_RTexData
 */
void SCE_RAddTextureTexData (SCE_RTexture *tex, int target,
                             SCE_RTexData *d, int canfree)
{
    unsigned int i;

    i = SCE_RGetTextureTargetID (tex, &target);
    d->target = target;
    d->user = !canfree;
    SCE_List_Appendl (&tex->data[i], &d->it);
    tex->have_data = SCE_TRUE;
    /* on place les valeurs forcees (si elles le sont) */
    if (force_pxf) d->pxf = forced_pxf;
    if (force_type) d->type = forced_type;
    if (force_fmt) d->fmt = forced_fmt;
}

/**
 * \brief Duplicates and adds a new texture data to a texture
 *
 * This function works like SCE_RAddTextureTexData() except that duplicates
 * \p d before adding.
 *
 * \sa SCE_RAddTextureTexData() SCE_RTexData
 */
int SCE_RAddTextureTexDataDup (SCE_RTexture *tex, int target, SCE_RTexData *d)
{
    SCE_RTexData *data = NULL;

    data = SCE_RDupTexData (d);
    if (!data) {
        SCEE_LogSrc ();
        return SCE_ERROR;
    }
    data->user = SCE_FALSE;     /* tak */
    SCE_RAddTextureTexData (tex, target, data, SCE_TRUE);
    return SCE_OK;
}

/**
 * \brief 
 * \param 
 */
SCE_RImage* SCE_RRemoveTextureImage (SCE_RTexture *tex, int target, int level)
{
    SCE_RImage *img = NULL;
    SCE_RTexData *d = NULL;
    SCE_SListIterator *it = NULL;
    unsigned int i;

    i = SCE_RGetTextureTargetID (tex, &target);

    /* si level n'existe pas, on prend le dernier */
    /* >= parce que level commence a 0 */
    if (level >= SCE_List_GetSize (&tex->data[i]))
        it = SCE_List_GetLast (&tex->data[i]);
    else
        it = SCE_List_GetIterator (&tex->data[i], level);

    d = SCE_List_GetData (it);
    img = d->img;
    d->canfree = SCE_FALSE;

    /* what is that for? */
    if (img) {
        SCE_List_ForEach (it, &tex->data[i]) {
            d = SCE_List_GetData (it);
            if (d->img == img)
                d->img = NULL;
        }
    }

    return img;
}

/**
 * \brief 
 * \param 
 */
void SCE_REraseTextureImage (SCE_RTexture *tex, int target, int level)
{
    SCE_RImage *img = NULL;
    img = SCE_RRemoveTextureImage (tex, target, level);
    if (SCE_Resource_Free (img))
        SCE_RDeleteImage (img);
}

/**
 * \brief 
 * \param 
 */
SCE_RTexData* SCE_RRemoveTextureTexData (SCE_RTexture *tex,
                                         int target, int level)
{
    SCE_RTexData *d = NULL, *data = NULL;
    SCE_SListIterator *pro = NULL, *it = NULL;
    unsigned int i;

    i = SCE_RGetTextureTargetID (tex, &target);

    /* si level n'existe pas, on prend le dernier */
    if (level > SCE_List_GetSize (&tex->data[i]))
        d = SCE_List_GetData (SCE_List_GetLast (&tex->data[i]));
    else
        d = SCE_List_GetData (SCE_List_GetIterator (&tex->data[i], level));

    d->user = SCE_TRUE;
    data = d;

    SCE_List_ForEachProtected (pro, it, &tex->data[i]) {
        d = SCE_List_GetData (it);
        if (d == data)
            SCE_List_Removel (it);
    }

    return data;

}

/**
 * \brief 
 * \param 
 */
void SCE_REraseTextureTexData (SCE_RTexture *tex, int target, int level)
{
    SCE_RTexData *d = NULL;
    d = SCE_RRemoveTextureTexData (tex, target, level);
    /* TexData are no resources, so.. */
    SCE_RDeleteTexData (d);
}


typedef struct
{
    int type, w, h, d;
    const char **names;
} SCE_RTexResInfo;

static void* SCE_RLoadTextureResource (const char *name, int force, void *data)
{
    unsigned int i = 0, j;
    SCE_RImage *img = NULL;
    SCE_RTexture *tex = NULL;
    int resize;
    int type, w, h, d;
    SCE_RTexResInfo *rinfo = data;

    (void)name;
    type = rinfo->type;
    w = rinfo->w; h = rinfo->h; d = rinfo->d;
    resize = (w > 0 || h > 0 || d > 0);

    /* si type est egal a SCE_TEX_CUBE, 6 const char* seront recuperes,
     * representant respectivement les 6 faces du cube. si des parametres
     * manquent, seul le premier sera construit lors de la construction.
     * sinon, chaque parametre representera un niveau de mipmap
     */

    if (force > 0)
        force--;
    for (j = 0; rinfo->names[j]; j++) {
        const char *fname = rinfo->names[j];
        if (!(img = SCE_Resource_Load (SCE_RGetImageResourceType (),
                                       fname, force, NULL)))
            goto fail;

        if (resize)
            SCE_RResizeTextureImage (img, w, h, d);

        if (!tex) {
            if (type <= 0)
                type = SCE_RGetImageType (img);
            tex = SCE_RCreateTexture (type);
            if (!tex)
                goto fail;
        }

        if (SCE_RAddTextureImage (tex, (type == SCE_TEX_CUBE ?
                                  SCE_TEX_POSX + i : 0), img, SCE_TRUE) < 0)
            goto fail;

        /* cubemap..? */
        if (type == SCE_TEX_CUBE)
            i++;
        else if (resize) {
            /* mipmapping */
            w = SCE_RGetImageWidth (img) / 2;
            h = SCE_RGetImageHeight (img) / 2;
            d = SCE_RGetImageDepth (img) / 2;
        }

        if (i == 6)             /* cube map completed */
            break;
    }

    return tex;
fail:
    SCE_RDeleteTexture (tex);
    SCEE_LogSrc ();
    return NULL;
}

/**
 * \brief 
 * \param force force a new texture to be loaded
 */
SCE_RTexture* SCE_RLoadTexturev (int type, int w, int h, int d, int force,
                                 const char **names)
{
    unsigned int i;
    char buf[2048] = {0};       /* hahaha */
    SCE_RTexResInfo info;
    SCE_RTexture *tex;

    info.type = type;
    info.w = w; info.h = h; info.d = d;
    info.names = names;
    for (i = 0; names[i]; i++)
        strcat (buf, names[i]);

    if (!(tex = SCE_Resource_Load (resource_type, buf, force, &info)))
        goto fail;

    return tex;
fail:
    SCEE_LogSrc ();
    return NULL;
}
/**
 * \brief 
 * \param 
 */
SCE_RTexture* SCE_RLoadTexture (int type, int w, int h, int d, int force, ...)
{
    va_list args;
    unsigned int i = 0;
    const char *name = NULL;
    const char *names[42];
    SCE_RTexture *tex = NULL;

    va_start (args, force);
    name = va_arg (args, const char*);
    while (name && i < 42 - 1) {
        names[i] = name;
        name = va_arg (args, const char*);
        i++;
    }
    va_end (args);
    names[i] = NULL;
#ifdef SCE_DEBUG
    if (i == 0) {
        SCEE_Log (SCE_INVALID_ARG);
        SCEE_LogMsg ("excpected at least 1 file name, but 0 given");
        return NULL;
    }
#endif
    tex = SCE_RLoadTexturev (type, w, h, d, force, names);
    return tex;
}


typedef void (*SCE_RMakeTextureFunc)(SCE_RTexData*);

static void SCE_RMakeTexture1DComp (SCE_RTexData *d)
{
    glCompressedTexImage1D (d->target, d->level, d->pxf, d->w, 0,
                            d->data_size, d->data);
}
static void SCE_RMakeTexture2DComp (SCE_RTexData *d)
{
    glCompressedTexImage2D (d->target, d->level, d->pxf, d->w, d->h,
                            0, d->data_size, d->data);
}
static void SCE_RMakeTexture3DComp (SCE_RTexData *d)
{
    glCompressedTexImage3D (d->target, d->level, d->pxf, d->w, d->h, d->d,
                            0, d->data_size, d->data);
}
static void SCE_RMakeTexture1D (SCE_RTexData *d)
{
    glTexImage1D (d->target, d->level, d->pxf, d->w, 0, d->fmt,
                  sce_rgltypes[d->type], d->data);
}
static void SCE_RMakeTexture2D (SCE_RTexData *d)
{
    glTexImage2D (d->target, d->level, d->pxf, d->w, d->h,
                  0, d->fmt, sce_rgltypes[d->type], d->data);
}
static void SCE_RMakeTexture3D (SCE_RTexData *d)
{
    glTexImage3D (d->target, d->level, d->pxf, d->w, d->h, d->d,
                  0, d->fmt, sce_rgltypes[d->type], d->data);
}
/* fonctions de mise a jour */
static void SCE_RMakeTexture1DCompUp (SCE_RTexData *d)
{
    glCompressedTexSubImage1D (d->target, d->level, 0, d->w,
                               d->pxf, d->data_size, d->data);
}
static void SCE_RMakeTexture2DCompUp (SCE_RTexData *d)
{
    glCompressedTexSubImage2D (d->target, d->level, 0, 0, d->w, d->h, d->pxf,
                               d->data_size, d->data);
}
static void SCE_RMakeTexture3DCompUp (SCE_RTexData *d)
{
    glCompressedTexSubImage3D (d->target, d->level, 0, 0, 0, d->w, d->h,
                               d->d, d->pxf, d->data_size, d->data);
}
static void SCE_RMakeTexture1DUp (SCE_RTexData *d)
{
    glTexSubImage1D (d->target, d->level, 0, d->w, d->fmt,
                     sce_rgltypes[d->type], d->data);
}
static void SCE_RMakeTexture2DUp (SCE_RTexData *d)
{
    glTexSubImage2D (d->target, d->level, 0, 0, d->w, d->h, d->fmt,
                     sce_rgltypes[d->type], d->data);
}
static void SCE_RMakeTexture3DUp (SCE_RTexData *d)
{
    glTexSubImage3D (d->target, d->level, 0, 0, 0, d->w, d->h, d->d, d->fmt,
                     sce_rgltypes[d->type], d->data);
}
/* determine quelle fonction utiliser pour le stockage des donnees */
static SCE_RMakeTextureFunc SCE_RGetMakeTextureFunc (int type, int comp)
{
    SCE_RMakeTextureFunc make = NULL;
    if (comp) {
        if (type == SCE_TEX_1D)
            make = (texsub ? SCE_RMakeTexture1DCompUp : SCE_RMakeTexture1DComp);
        else if (type == SCE_TEX_2D || type == SCE_TEX_CUBE)
            make = (texsub ? SCE_RMakeTexture2DCompUp : SCE_RMakeTexture2DComp);
        else if (type == SCE_TEX_3D)
            make = (texsub ? SCE_RMakeTexture3DCompUp : SCE_RMakeTexture3DComp);
    } else {
        if (type == SCE_TEX_1D)
            make = (texsub ? SCE_RMakeTexture1DUp : SCE_RMakeTexture1D);
        else if (type == SCE_TEX_2D || type == SCE_TEX_CUBE)
            make = (texsub ? SCE_RMakeTexture2DUp : SCE_RMakeTexture2D);
        else if (type == SCE_TEX_3D)
            make = (texsub ? SCE_RMakeTexture3DUp : SCE_RMakeTexture3D);
    }
    /* NOTE: make may be NULL here */
    return make;
}
/* construit une texture avec les infos minimales */
static void SCE_RMakeTexture (SCEenum textype, SCE_SList *data,
                              SCEenum target, int use_mipmap)
{
    SCE_SListIterator *i = NULL;
    SCE_RTexData *d = NULL;
    SCE_RMakeTextureFunc make = NULL;

    SCE_List_ForEach (i, data) {
        d = SCE_List_GetData (i);
        make = SCE_RGetMakeTextureFunc (textype, d->comp);
        /* si un target specifique a ete specifie */
        if (target != 0)
            d->target = target;
        make (d);
        if (!use_mipmap)
            break;
        /* TODO: penser a generer les niveaux non-existants dans 'data' */
    }
}
int SCE_RBuildTexture (SCE_RTexture *tex, int use_mipmap, int hw_mipmap)
{
    unsigned int i, n = 1;
    SCEenum t;
    void (*make)(SCEenum, SCE_SList*, SCEenum, int) = SCE_RMakeTexture;

    t = tex->target;
    /* TODO: crap behavior */
    if (!tex->have_data) {
        SCEE_Log (SCE_INVALID_OPERATION);
        SCEE_LogMsg ("you must specify data before texture build");
        return SCE_ERROR;
    }

    /* si un parametre est non-specifie, on lui attribue la valeur
       deja presente dans la texture, sinon c'est l'inverse */
    if (use_mipmap < 0)
        use_mipmap = tex->use_mipmap;
    else
        tex->use_mipmap = use_mipmap;
    if (hw_mipmap < 0)
        hw_mipmap = tex->hw_mipmap;
    else
        tex->hw_mipmap = hw_mipmap;

    if (tex->target == SCE_TEX_CUBE)
        n = 6;

    if (use_mipmap) {
        if (hw_mipmap && SCE_RHasCap (SCE_TEX_HW_GEN_MIPMAP)) {
            SCE_RSetTextureParam (tex, GL_GENERATE_MIPMAP_SGIS, SCE_TRUE);
            for (i = 0; i < n; i++)
                make (t, &tex->data[i], ((n > 1) ? SCE_TEX_POSX+i:0), SCE_FALSE);
        } else {
            if (hw_mipmap)
                SCEE_SendMsg ("hardware mipmap generation isn't supported");
            for (i = 0; i < n; i++)
                make (t, &tex->data[i], (n > 1 ? SCE_TEX_POSX+i : 0), SCE_TRUE);
        }
        SCE_RSetTextureParam (tex, GL_TEXTURE_MAX_LEVEL, max_mipmap_level);
        SCE_RSetTextureFilter (tex, SCE_TEX_TRILINEAR);
    } else {
        for (i = 0; i < n; i++)
            make (t, &tex->data[i], (n > 1 ? SCE_TEX_POSX + i : 0), SCE_FALSE);
        SCE_RSetTextureParam (tex, GL_TEXTURE_MAX_LEVEL, 0);
    }

    SCE_RPixelizeTexture (tex, SCE_FALSE);
    texsub = SCE_FALSE;

    return SCE_OK;
}


/**
 * \brief 
 * \param 
 */
int SCE_RUpdateTexture (SCE_RTexture *tex, int use_mipmap, int hw_mipmap)
{
    texsub = SCE_TRUE;
    return SCE_RBuildTexture (tex, use_mipmap, hw_mipmap);
}

/**
 * \deprecated
 * \brief lol
 * \todo GL specific function, remove it
 */
void SCE_RSetActiveTextureUnit (unsigned int unit)
{
    glActiveTexture (SCE_TEX0 + unit);
}

/**
 * \todo l'organisation des deux prochaines fonctions pue peut-etre :
 *       je suppose qu'il y a un BindTexture a faire pour chaque unite
 *       de texturage, or c'est peut-etre faux... a voir.
 *       et y'a-t-il un glDisable/Enable a faire pour chaque unite ?
 */
static void SCE_RSetTextureUsed (SCE_RTexture *tex, int unit)
{
    if (tex) {
        glActiveTexture (SCE_TEX0 + unit);
        glEnable (tex->target);
        glBindTexture (tex->target, tex->id);
        n_textype[tex->type]++;
        texused[unit] = tex;
#if 0
        nbatchs++;
#endif
    } else if (texused[unit]) {
        glActiveTexture (SCE_TEX0 + unit);
        glBindTexture (texused[unit]->target, 0);
        n_textype[texused[unit]->type]--;
        if (n_textype[texused[unit]->type] <= 0) {
            n_textype[texused[unit]->type] = 0;
            glDisable (texused[unit]->target);
        }
        texused[unit] = NULL;
    }
}
/**
 * \brief 
 * \param 
 * \todo reviser
 */
void SCE_RUseTexture (SCE_RTexture *tex, int unit)
{
    /* invalid texture unit */
    if (unit >= max_tex_units)
        return;
    else if (unit >= 0)
        SCE_RSetTextureUsed (tex, unit);
    else {
        glDisable (SCE_TEX_1D);
        glDisable (SCE_TEX_2D);
        glDisable (SCE_TEX_3D);
        glDisable (SCE_TEX_CUBE);

        for (unit = 0; unit < max_tex_units; unit++) {
            texused[unit] = NULL;
            glActiveTexture (SCE_TEX0 + unit);
            glBindTexture (SCE_TEX_1D, 0);
            glBindTexture (SCE_TEX_2D, 0);
            glBindTexture (SCE_TEX_3D, 0);
            glBindTexture (SCE_TEX_CUBE, 0);
            /* don't do any glDisable here? */
        }
    }
}

/** @} */
