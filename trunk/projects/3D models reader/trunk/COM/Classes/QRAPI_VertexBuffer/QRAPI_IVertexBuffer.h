/******************************************************************************
 * ==> QRAPI_IVertexBuffer ---------------------------------------------------*
 ******************************************************************************
 * Description : Vertex buffer COM interface                                  *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QRAPI_IVertexBufferH
#define QRAPI_IVertexBufferH

// embarcadero
#include <System.hpp>

// interface
#include "QRAPI.h"
#include "QRAPI_Common.h"

/**
* Vertex buffer types
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
#ifdef __cplusplus
    }
#endif

// {3830A923-3EDB-4C1C-A58C-ABFDE1F8A802}
static const IID IID_QRAPI_IVertexBuffer =
        {0x3830a923, 0x3edb, 0x4c1c, {0xa5, 0x8c, 0xab, 0xfd, 0xe1, 0xf8, 0xa8, 0x02}};

/**
* Vertex buffer interface
*@author Jean-Milost Reymond
*/
struct __declspec(uuid("{3830A923-3EDB-4C1C-A58C-ABFDE1F8A802}")) QRAPI_IVertexBuffer : public System::IInterface
{
    /**
    * Gets vertex buffer type
    *@return vertex buffer type
    */
    virtual QRAPI_EVertexType __stdcall GetType() const = 0;

    /**
    * Sets vertex buffer type
    *@param type - vertex buffer type
    */
    virtual void __stdcall SetType(QRAPI_EVertexType type) = 0;

    /**
    * Gets vertex buffer size
    *@return vertex buffer size
    */
    virtual QRAPI_BufferSizeType __stdcall GetSize() const = 0;

    /**
    * Gets vertex buffer data
    *@param[out] pData - vertex buffer data
    *@return success or error message
    *@note pData should be initialized first, and should contain as much space as the value returned
    *      by Get_VertexBuffer_Size()
    */
    virtual QRAPI_EResult __stdcall GetData(void* pData) const = 0;

    /**
    * Sets vertex buffer data
    *@param pData - source data to copy from
    *@param size - size to copy
    *@return success or error message
    */
    virtual QRAPI_EResult __stdcall SetData(void* pData, QRAPI_SizeT size) = 0;
};

/**
* Vertex buffer Delphi interface
*/
typedef System::DelphiInterface<QRAPI_IVertexBuffer> _di_QRAPI_IVertexBuffer;

/**
* Creates vertex buffer instance
*@return vertex buffer instance
*/
EXTERN_C _di_QRAPI_IVertexBuffer QR_ENGINE_API QRAPI_CreateVertexBuffer();

// {2FCBBE4E-3C84-4F5C-87B9-C95952A0BF28}
static const IID IID_QRAPI_IVertexBuffers =
        {0x2fcbbe4e, 0x3c84, 0x4f5c, {0x87, 0xb9, 0xc9, 0x59, 0x52, 0xa0, 0xbf, 0x28}};

/**
* Vertex buffer list interface
*@author Jean-Milost Reymond
*/
struct __declspec(uuid("{2FCBBE4E-3C84-4F5C-87B9-C95952A0BF28}")) QRAPI_IVertexBuffers : public System::IInterface
{
    /**
    * Gets vertex buffer count
    *@return vertex buffer count
    */
    virtual QRAPI_SizeT __stdcall GetCount() const = 0;

    /**
    * Gets vertex buffer
    *@return vertex buffer, NULL if not found or on error
    */
    virtual _di_QRAPI_IVertexBuffer __stdcall Get(QRAPI_SizeT index) const = 0;

    /**
    * Adds vertex buffer
    *@param pVB - vertex buffer to add
    *@return success or error message
    */
    virtual QRAPI_EResult __stdcall Add(_di_QRAPI_IVertexBuffer pVB) = 0;
};

/**
* Vertex buffer list Delphi interface
*/
typedef System::DelphiInterface<QRAPI_IVertexBuffers> _di_QRAPI_IVertexBuffers;

/**
* Creates vertex buffer list instance
*@return vertex buffer list instance
*/
EXTERN_C _di_QRAPI_IVertexBuffers QR_ENGINE_API QRAPI_CreateVertexBuffers();
#endif // QRAPI_IVertexBufferH
