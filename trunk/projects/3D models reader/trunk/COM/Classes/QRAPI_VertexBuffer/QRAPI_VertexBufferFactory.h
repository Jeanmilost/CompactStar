/******************************************************************************
 * ==> QRAPI_VertexBufferFactory ---------------------------------------------*
 ******************************************************************************
 * Description : Vertex buffers COM object factory                            *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QRAPI_VertexBufferFactoryH
#define QRAPI_VertexBufferFactoryH

// std
#include <vector>

// interface
#include "QRAPI_IVertexBuffer.h"

// {1C88B031-A543-4611-BAAC-EEA676C51735}
static const CLSID CLSID_QRAPI_VertexBufferFactory =
        {0x1c88b031, 0xa543, 0x4611, {0xba, 0xac, 0xee, 0xa6, 0x76, 0xc5, 0x17, 0x35}};

/**
* Vertex buffer COM object factory
*@author Jean-Milost Reymond
*/
class QRAPI_VertexBufferFactory : public TInterfacedObject, QRAPI_IVertexBuffer
{
    public:
        __fastcall QRAPI_VertexBufferFactory();
        virtual __fastcall ~QRAPI_VertexBufferFactory();

        /**
        * Gets vertex buffer type
        *@return vertex buffer type
        */
        virtual QRAPI_EVertexType __stdcall GetType() const;

        /**
        * Sets vertex buffer type
        *@param type - vertex buffer type
        */
        virtual void __stdcall SetType(QRAPI_EVertexType type);

        /**
        * Gets vertex buffer size
        *@return vertex buffer size
        */
        virtual QRAPI_BufferSizeType __stdcall GetSize() const;

        /**
        * Gets vertex buffer data
        *@param[out] pData - vertex buffer data
        *@return success or error message
        *@note pData should be initialized first, and should contain as much space as the value
        *      returned by GetSize()
        */
        virtual QRAPI_EResult __stdcall GetData(void* pData) const;

        /**
        * Sets vertex buffer data
        *@param pData - source data to copy from
        *@param size - size to copy
        *@return success or error message
        */
        virtual QRAPI_EResult __stdcall SetData(void* pData, QRAPI_SizeT size);

        /**
        * Query interface
        *@param IID - interface identifier
        *@param[out] pObj - object matching with interface
        *@return error or success message
        */
        virtual HRESULT __stdcall QueryInterface(const GUID& IID, void** pObj);

        /**
        * Add interface reference
        *@return new reference count
        */
        virtual ULONG __stdcall AddRef();

        /**
        * Release interface
        *@return new reference count
        */
        virtual ULONG __stdcall Release();

    private:
        typedef std::vector<QRAPI_VertexBufferDataType> QRAPI_VertexData;

        QRAPI_VertexData  m_Data;
        QRAPI_EVertexType m_Type;
};

// {35DFA66E-351D-4742-9853-D2345D4A0152}
static const CLSID CLSID_QRAPI_VertexBuffersFactory =
        {0x35dfa66e, 0x351d, 0x4742, {0x98, 0x53, 0xd2, 0x34, 0x5d, 0x4a, 0x01, 0x52}};

/**
* Vertex buffer list COM object factory
*@author Jean-Milost Reymond
*/
class QRAPI_VertexBuffersFactory : public TInterfacedObject, QRAPI_IVertexBuffers
{
    public:
        __fastcall QRAPI_VertexBuffersFactory();
        virtual __fastcall ~QRAPI_VertexBuffersFactory();

        /**
        * Gets vertex buffer count
        *@return vertex buffer count
        */
        virtual QRAPI_SizeT __stdcall GetCount() const;

        /**
        * Gets vertex buffer
        *@return vertex buffer, NULL if not found or on error
        */
        virtual _di_QRAPI_IVertexBuffer __stdcall Get(QRAPI_SizeT index) const;

        /**
        * Adds vertex buffer
        *@param pVB - vertex buffer to add
        *@return success or error message
        *@note Added vertex buffer will be deleted internally, do not delete it externally
        */
        virtual QRAPI_EResult __stdcall Add(_di_QRAPI_IVertexBuffer pVB);

        /**
        * Query interface
        *@param IID - interface identifier
        *@param[out] pObj - object matching with interface
        *@return error or success message
        */
        virtual HRESULT __stdcall QueryInterface(const GUID& IID, void** pObj);

        /**
        * Add interface reference
        *@return new reference count
        */
        virtual ULONG __stdcall AddRef();

        /**
        * Release interface
        *@return new reference count
        */
        virtual ULONG __stdcall Release();

    private:
        typedef std::vector<_di_QRAPI_IVertexBuffer> QRAPI_VertexBuffers;

        QRAPI_VertexBuffers m_Buffers;
};
#endif
