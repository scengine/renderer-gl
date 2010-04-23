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
 
/* created: 31/01/2006
   updated: 01/08/2009 */

#ifndef SCERSUPPORT_H
#define SCERSUPPORT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup extenstionsupport
 * @{
 */

/**
 * \brief Capabilities constants
 * 
 * Constants about OpenGL's implementation capabilities
 */
enum sce_rcapability {
    SCE_TEX_MULTI,              /**< multitexturing support */
    SCE_TEX_NON_POWER_OF_TWO,   /**< Non-power-of-two texture support */
    SCE_TEX_HW_GEN_MIPMAP,      /**< Hardware mipmapping generation support */
    SCE_TEX_DXT1,               /**< DXT1 texture compression format support */
    SCE_TEX_DXT3,               /**< DXT3 texture compression format support */
    SCE_TEX_DXT5,               /**< DXT5 texture compression format support */
    SCE_TEX_S3TC,               /**< S3TC texture compression format support */
    SCE_TEX_3DC,                /**< 3DC texture compression format support */
    SCE_VBO,                    /**< Vertex buffer objects (VBO) support */
    SCE_VAO,                    /**< Vertex array objects (VAO) support */
    SCE_FBO,                    /**< Frame buffer objects (FB0) support */
    SCE_PBO,                    /**< Pixel buffer objects (PBO) support */
    SCE_VERTEX_SHADER_GLSL,     /**< GLSL vertex shader support */
    SCE_FRAGMENT_SHADER_GLSL,   /**< GLSL fragment shader support */
    SCE_RG_SHADERS,             /**< Cg shaders support */
    SCE_OCCLUSION_QUERY,        /**< Occlusion queries support */
    SCE_MRT,                    /**< Multiple render targets (MRT) support */
    SCE_HW_INSTANCING,          /**< Hardware instancing support */
    SCE_NUM_CAPS
};
/**
 * \copydoc sce_rcapability
 * \see sce_rcapability
 */
typedef enum sce_rcapability SCE_RCap;

/** @} */

int SCE_RSupportInit (void);
void SCE_RSupportQuit (void);

int SCE_RFindExtension (const char*);
int SCE_RIsSupported (const char*);

int SCE_RHasCap (SCE_RCap);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
