/******************************************************************************
 * ==> QRAPI_VertexBufferFactory ---------------------------------------------*
 ******************************************************************************
 * Description : Vertex buffers COM object factory                            *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#include "QRAPI_VertexBufferFactory.h"

//---------------------------------------------------------------------------
// QRAPI_VertexBufferFactory - c++ COM API
//---------------------------------------------------------------------------
__fastcall QRAPI_VertexBufferFactory::QRAPI_VertexBufferFactory() :
    TInterfacedObject(),
    QRAPI_IVertexBuffer()
{
    ::InterlockedIncrement(&g_nComObjsInUse);
}
//---------------------------------------------------------------------------
__fastcall QRAPI_VertexBufferFactory::~QRAPI_VertexBufferFactory()
{
    ::InterlockedDecrement(&g_nComObjsInUse);
}
//---------------------------------------------------------------------------
QRAPI_EVertexType __stdcall QRAPI_VertexBufferFactory::GetType() const
{
    return m_Type;
}
//---------------------------------------------------------------------------
void __stdcall QRAPI_VertexBufferFactory::SetType(QRAPI_EVertexType type)
{
    m_Type = type;
}
//---------------------------------------------------------------------------
QRAPI_BufferSizeType __stdcall QRAPI_VertexBufferFactory::GetSize() const
{
    return m_Data.size();
}
//---------------------------------------------------------------------------
QRAPI_EResult __stdcall QRAPI_VertexBufferFactory::GetData(void* pData) const
{
    try
    {
        // no output data?
        if (!pData)
            return QRAPI_R_Param_Not_Initialized;

        // copy data
        std::memcpy(pData, &m_Data[0], sizeof(M_QRAPI_Precision) * m_Data.size());

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_EResult __stdcall QRAPI_VertexBufferFactory::SetData(void* pData, QRAPI_SizeT size)
{
    try
    {
        // nothing to copy?
        if (!size)
            return QRAPI_R_Success;

        // no source data?
        if (!pData)
            return QRAPI_R_Param_Not_Initialized;

        // copy data
        m_Data.resize(size);
        std::memcpy(&m_Data[0], pData, sizeof(QRAPI_VertexBufferDataType) * size);

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
HRESULT __stdcall QRAPI_VertexBufferFactory::QueryInterface(const GUID& IID, void **Obj)
{
    return GetInterface(IID, Obj) ? S_OK : E_NOINTERFACE;
}
//---------------------------------------------------------------------------
ULONG __stdcall QRAPI_VertexBufferFactory::AddRef()
{
    return TInterfacedObject::_AddRef();
}
//---------------------------------------------------------------------------
ULONG __stdcall QRAPI_VertexBufferFactory::Release()
{
    return TInterfacedObject::_Release();
}
//---------------------------------------------------------------------------
// QRAPI_VertexBuffersFactory - c++ COM API
//---------------------------------------------------------------------------
__fastcall QRAPI_VertexBuffersFactory::QRAPI_VertexBuffersFactory() :
    TInterfacedObject(),
    QRAPI_IVertexBuffers()
{
    ::InterlockedIncrement(&g_nComObjsInUse);
}
//---------------------------------------------------------------------------
__fastcall QRAPI_VertexBuffersFactory::~QRAPI_VertexBuffersFactory()
{
    try
    {
        // get vertex buffer count
        const QRAPI_SizeT count = m_Buffers.size();

        // delete all added vertex buffers
        for (QRAPI_SizeT i = 0; i < count; ++i)
            try
            {
                delete m_Buffers[i];
            }
            catch (...)
            {}
    }
    catch (...)
    {}

    ::InterlockedDecrement(&g_nComObjsInUse);
}
//---------------------------------------------------------------------------
QRAPI_SizeT __stdcall QRAPI_VertexBuffersFactory::GetCount() const
{
    return m_Buffers.size();
}
//---------------------------------------------------------------------------
_di_QRAPI_IVertexBuffer __stdcall QRAPI_VertexBuffersFactory::Get(QRAPI_SizeT index) const
{
    try
    {
        // is index out of bounds?
        if (index >= m_Buffers.size())
            return NULL;

        return m_Buffers[index];
    }
    catch (...)
    {}

    return NULL;
}
//---------------------------------------------------------------------------
QRAPI_EResult __stdcall QRAPI_VertexBuffersFactory::Add(_di_QRAPI_IVertexBuffer pVB)
{
    try
    {
        // no source data?
        if (!pVB)
            return QRAPI_R_Param_Not_Initialized;

        m_Buffers.push_back(pVB);
        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
HRESULT __stdcall QRAPI_VertexBuffersFactory::QueryInterface(const GUID& IID, void **Obj)
{
    return GetInterface(IID, Obj) ? S_OK : E_NOINTERFACE;
}
//---------------------------------------------------------------------------
ULONG __stdcall QRAPI_VertexBuffersFactory::AddRef()
{
    return TInterfacedObject::_AddRef();
}
//---------------------------------------------------------------------------
ULONG __stdcall QRAPI_VertexBuffersFactory::Release()
{
    return TInterfacedObject::_Release();
}
//---------------------------------------------------------------------------

