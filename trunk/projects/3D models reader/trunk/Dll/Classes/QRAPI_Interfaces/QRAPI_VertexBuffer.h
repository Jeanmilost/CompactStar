/******************************************************************************
 * ==> QRAPI_VertexBuffer ----------------------------------------------------*
 ******************************************************************************
 * Description : QR_Engine API vertex buffer functions                        *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QRAPI_VertexBufferH
#define QRAPI_VertexBufferH

// std
#include <vector>

// qr api
#include "QRAPI.h"
#include "QRAPI_Common.h"

/**
* Vertex buffer functions
*@author Jean-Milost Reymond
*/
#ifdef __cplusplus
    extern "C"
    {
#endif
        /**
        * Vertex format enumeration
        *@note Flags can be combined
        */
        enum QRAPI_EVertexFormat
        {
            QRAPI_VF_None      = 0x00,
            QRAPI_VF_Normals   = 0x01,
            QRAPI_VF_TexCoords = 0x02,
            QRAPI_VF_Colors    = 0x04,
        };

        /**
        * Vertex buffer type enumeration
        */
        enum QRAPI_EVertexType
        {
            QRAPI_VT_Unknown = 0,
            QRAPI_VT_Triangles,
            QRAPI_VT_TriangleStrip,
            QRAPI_VT_TriangleFan,
            QRAPI_VT_Quads,
            QRAPI_VT_QuadStrip,
        };

        typedef std::vector<QRAPI_VertexDataType> QRAPI_VertexData;

        /**
        * Vertex buffer for the DLL import/export
        *@author Jean-Milost Reymond
        */
        struct QRAPI_VertexBuffer
        {
            QRAPI_VertexData  m_Data;
            QRAPI_EVertexType m_Type;
        };

        typedef QRAPI_VertexBuffer*              QRAPI_VertexBuffer_Handle;
        typedef std::vector<QRAPI_VertexBuffer*> QRAPI_VertexBufferContainer;

        /**
        * Vertex buffer container handle, that can contain list of vertex buffers
        *@note When accessing an STL object created in one DLL or EXE through a pointer or reference in
        *      a different DLL or EXE, you may experience an access violation or other serious program
        *      errors including the appearance of data corruption or data loss. Most classes in the
        *      Standard C++ Libraries use static data members directly or indirectly. Since these classes
        *      are generated through template instantiation, each executable image (usually with DLL or
        *      EXE file name extensions) will contain its own copy of the static data member for a given
        *      class. When a method of the class that requires the static data member is executed, it
        *      uses the static data member in the executable image in which the method code resides.
        *      Since the static data members in the executable images are not in sync, this action could
        *      result in an access violation or data may appear to be lost or corrupted.
        */
        typedef QRAPI_VertexBufferContainer* QRAPI_VertexBufferContainer_Handle;

        /**
        * Creates vertex buffer container instance
        *@returns vertex buffer container handle, NULL on error
        */
        QRAPI_VertexBufferContainer_Handle QR_ENGINE_API Create_VertexBufferContainer();

        /**
        * Deletes vertex buffer container instance
        *@param handle - vertex buffer container handle to delete
        *@return success or error message
        */
        QRAPI_EResult QR_ENGINE_API Delete_VertexBufferContainer(QRAPI_VertexBufferContainer_Handle handle);

        /**
        * Gets vertex buffer count
        *@param handle - vertex buffer container handle to count
        *@param pResult - resulting value
        *@return success or error message
        */
        QRAPI_EResult QR_ENGINE_API Get_VertexBuffer_Count(QRAPI_VertexBufferContainer_Handle handle,
                                                           QRAPI_SizeT*                       pResult);

        /**
        * Gets vertex buffer
        *@param handle - vertex buffer container handle to extract from
        *@param index - vertex buffer index
        *@return vertex buffer, NULL on error or if not found
        */
        QRAPI_VertexBuffer_Handle QR_ENGINE_API Get_VertexBuffer(QRAPI_VertexBufferContainer_Handle handle,
                                                                 QRAPI_SizeT                        index);

        /**
        * Deletes vertex buffer
        *@param handle - vertex buffer container handle to extract from
        *@return success or error message
        */
        QRAPI_EResult QR_ENGINE_API Delete_VertexBuffer(QRAPI_VertexBuffer_Handle handle);

        /**
        * Gets vertex buffer type
        *@param handle - vertex buffer handle to get from
        *@param pResult - resulting value
        *@return success or error message
        */
        QRAPI_EResult QR_ENGINE_API Get_VertexBuffer_Type(QRAPI_VertexBuffer_Handle handle,
                                                          QRAPI_EVertexType*        pResult);

        /**
        * Gets vertex buffer size
        *@param handle - vertex buffer handle to get from
        *@param pResult - resulting value
        *@return success or error message
        */
        QRAPI_EResult QR_ENGINE_API Get_VertexBuffer_Size(QRAPI_VertexBuffer_Handle handle,
                                                          QRAPI_SizeT*              pResult);

        /**
        * Gets vertex buffer data
        *@param handle - vertex buffer handle to get from
        *@param pData - vertex buffer data
        *@return success or error message
        *@note pData should be initialized first, and should contain as much space as the value returned
        *      by Get_VertexBuffer_Size()
        */
        QRAPI_EResult QR_ENGINE_API Get_VertexBuffer_Data(QRAPI_VertexBuffer_Handle handle,
                                                          void*                     pData);
#ifdef __cplusplus
    }
#endif

#endif
