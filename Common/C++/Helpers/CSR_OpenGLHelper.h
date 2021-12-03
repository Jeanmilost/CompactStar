/****************************************************************************
 * ==> CSR_OpenGLHelper ----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides an helper class for OpenGL            *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2022, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

#ifndef CSR_OpenGLHelperH
#define CSR_OpenGLHelperH

// std
#include <map>

// compactStar engine
#include "CSR_Geometry.h"
#include "CSR_Collision.h"
#include "CSR_Renderer_OpenGL.h"

/**
* Helper class for OpenGL
*@author Jean-Milost Reymond
*/
class CSR_OpenGLHelper
{
    public:
        typedef std::map<void*, CSR_OpenGLID*> IResources;

        CSR_OpenGLHelper();
        virtual ~CSR_OpenGLHelper();

        /**
        * Enables OpenGL
        *@param hWnd - Windows handle
        *@param hDC - device context
        *@param hRC - OpenGL rendering context
        */
        static void EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC);

        /**
        * Disables OpenGL
        *@param hWnd - Windows handle
        *@param hDC - device context
        *@param hRC - OpenGL rendering context
        */
        static void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);

        /**
        * Creates the viewport
        *@param w - viewport width
        *@param h - viewport height
        *@param zNear - near clipping plane value
        *@param zFar - far clipping plane value
        *@param pShader - shader linked with the viewport
        *@param[out] matrix - the projection matrix used for the viewport
        */
        static void CreateViewport(float             w,
                                   float             h,
                                   float             zNear,
                                   float             zFar,
                             const CSR_OpenGLShader* pShader,
                                   CSR_Matrix4&      matrix);

        /**
        * Gets a model matrix which make a model to fit the viewport (as closest as possible)
        *@param pBox - model bounding box, if 0 an identity matrix will be returned
        *@param fov - field of view used in the viewport, in radians
        *@param rotated - if true, the model is rotated by default of 90° on the x axis (this is
        *                 generally the case for the MDL models)
        *@return the model matrix
        *@note Using this function, the model will be centered in the scene, with a normalized size
        *      and a rotation of 45° on the y axis
        */
        static CSR_Matrix4 FitModelInView(const CSR_Box* pBox, float fov, bool rotated);

        /**
        * Builds a matrix from the translation, rotation and scaling values
        *@param pTranslation - translation
        *@param pRotation - rotation
        *@param pScaling - scale factor
        *@param[in, out] pMatrix - matrix to populate with the values, populated matrix on function ends
        */
        static void BuildMatrix(const CSR_Vector3* pTranslation,
                                const CSR_Vector3* pRotation,
                                const CSR_Vector3* pScaling,
                                      CSR_Matrix4* pMatrix);

        /**
        * Adds a GPU texture to the resources
        *@param pKey - texture key to add
        *@param id - texture identifier on the GPU
        *@param resources - resources in which the texture will be added
        */
        static void AddTexture(const void* pKey, GLuint id, IResources& resources);

        /**
        * Deletes a texture from the GPU side
        *@param pKey - texture key to delete
        *@param resources - OpenGL resources owning the texture to delete
        *@note The texture will only be deleted on the GPU side, the pTexture object itself will not
        *      be modified
        */
        static void DeleteTexture(const void* pKey, IResources& resources);

        /**
        * Gets the identifier matching with a texture
        *@param pKey - texture key to get
        *@param resources - OpenGL resources owning the texture to get
        *@return texture identifier, NULL if not found or on error
        */
        static void* GetTextureID(const void* pKey, IResources& resources);

        /**
        * Clears all the OpenGL resources
        *@param resources - OpenGL resources to clear
        */
        static void ClearResources(IResources& resources);
};

#endif
