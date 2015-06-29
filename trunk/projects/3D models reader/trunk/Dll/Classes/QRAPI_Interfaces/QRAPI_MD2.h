/******************************************************************************
 * ==> QRAPI_MD2 -------------------------------------------------------------*
 ******************************************************************************
 * Description : DLL interface for MD2 model                                  *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QRAPI_MD2H
#define QRAPI_MD2H

// qr api
#include "QRAPI_Common.h"
#include "QRAPI_VertexBuffer.h"

/**
* DLL interface for MD2 models
*@author Jean-Milost Reymond
*/
#ifdef __cplusplus
    extern "C"
    {
#endif
        // class prototype
        class QR_MD2;

        // MD2 model handler
        typedef QR_MD2* QRAPI_MD2_Handle;

        /**
        * Creates MD2 model instance
        *@return MD2 model handle, NULL on error
        */
        QRAPI_MD2_Handle QR_ENGINE_API Create_MD2();

        /**
        * Deletes MD2 model instance
        *@param handle - MD2 model handle to delete
        *@return success or error message
        */
        QRAPI_EResult QR_ENGINE_API Delete_MD2(QRAPI_MD2_Handle handle);

        /**
        * Loads MD2 mesh from file
        *@param handle - MD2 model handle
        *@param pFileName - file name
        *@param pVersion - file version, can be NULL (default version will be used in this case)
        *@return success or error message        */        QRAPI_EResult QR_ENGINE_API LoadMesh_MD2(QRAPI_MD2_Handle   handle,
                                                 const QRAPI_Char*  pFileName,
                                                 const QRAPI_Float* pVersion = NULL);

        /**
        * Loads normals table from file
        *@param handle - MD2 model handle
        *@param pFileName - file name
        *@param pVersion - file version
        *@return success or error message        */        QRAPI_EResult QR_ENGINE_API LoadNormalsTable_MD2(QRAPI_MD2_Handle   handle,
                                                         const QRAPI_Char*  pFileName,
                                                         const QRAPI_Float* pVersion = NULL);

        /**
        * Set animation
        *@param handle - MD2 model handle
        *@param start - animation start frame index
        *@param end - animation end frame index
        *@param pName - animation name
        *@return success or error message
        */
        QRAPI_EResult QR_ENGINE_API SetAnimation_MD2(QRAPI_MD2_Handle  handle,
                                                     QRAPI_SizeT       start,
                                                     QRAPI_SizeT       end,
                                                     const QRAPI_Char* pName);

        /**
        * Animate mesh
        *@param handle - MD2 model handle
        *@param pElapsedTime - elapsed time since last frame was drawn
        *@param fps - number of frames per seconds
        *@return success or error message
        */
        QRAPI_EResult QR_ENGINE_API Animate_MD2(QRAPI_MD2_Handle    handle,
                                                const QRAPI_Double* pElapsedTime,
                                                QRAPI_SizeT         fps);

        /**
        * Gets vertex buffer
        *@param handle - MD2 model handle
        *@param hVBs - vertex buffer container handle to populate
        *@param vertexFormat - vertex format (i.e what data the vertex contains)
        *@param convertRHLH - if true, left hand system will be converted to right hand or vice-versa
        *@return success or error message
        *@note vertex buffer content is organized as follow:
        *      [1]x [2]y [3]z [4]nx [5]ny [6]nz [7]tu [8]tv [9]r [10]g [11]b [12]a
        *      where:
        *      x/y/z    - vertex coordinates
        *      nx/ny/nz - vertex normal (if includeNormal is activated)
        *      tu/tv    - vertex texture coordinates(if includeTexture is activated)
        *      r/g/b/a  - vertex color(if includeColor is activated)
        *@note Vertex buffers created internally in container must be deleted when useless. WARNING,
        *      the container can contain data to delete even if function failed
        */
        QRAPI_EResult QR_ENGINE_API GetVertexBuffer_MD2(QRAPI_MD2_Handle                   handle,
                                                        QRAPI_VertexBufferContainer_Handle hVBs,
                                                        QRAPI_EVertexFormat                vertexFormat,
                                                        bool                               convertRHLH);

        /**
        * Draws MD2 model using OpenGL (in direct mode)
        *@param handle - MD2 model handle
        *@param vertexFormat - vertex format (i.e what data the vertex contains)
        *@param convertRHLH - if true, left hand system will be converted to right hand or vice-versa
        *@return success or error message
        */
        QRAPI_EResult QR_ENGINE_API Draw_MD2_To_OpenGL_1(QRAPI_MD2_Handle    handle,
                                                         QRAPI_EVertexFormat vertexFormat,
                                                         bool                convertRHLH);
#ifdef __cplusplus
    }
#endif

#endif
