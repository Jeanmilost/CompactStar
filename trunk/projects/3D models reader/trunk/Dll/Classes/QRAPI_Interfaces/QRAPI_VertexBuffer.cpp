/******************************************************************************
 * ==> QRAPI_VertexBuffer ----------------------------------------------------*
 ******************************************************************************
 * Description : QR_Engine API vertex buffer functions                        *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#include "QRAPI_VertexBuffer.h"

//---------------------------------------------------------------------------
QRAPI_VertexBufferContainer_Handle QR_ENGINE_API Create_VertexBufferContainer()
{
    try
    {
        // create new vertex buffer container instance and return handle
        return new QRAPI_VertexBufferContainer();
    }
    catch (...)
    {}

    return NULL;
}
//---------------------------------------------------------------------------
QRAPI_EResult QR_ENGINE_API Delete_VertexBufferContainer(QRAPI_VertexBufferContainer_Handle handle)
{
    try
    {
        // nothing to delete?
        if (!handle)
            return QRAPI_R_Success;

        // delete vertex buffer container instance
        delete handle;

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_EResult QR_ENGINE_API Get_VertexBuffer_Count(QRAPI_VertexBufferContainer_Handle handle,
                                                   QRAPI_SizeT*                       pResult)
{
    try
    {
        // no handle?
        if (!handle)
            return QRAPI_R_Param_Not_Initialized;

        // no output?
        if (!pResult)
            return QRAPI_R_Param_Not_Initialized;

        // get vertex buffer count
        *pResult = handle->size();

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_VertexBuffer_Handle QR_ENGINE_API Get_VertexBuffer(QRAPI_VertexBufferContainer_Handle handle,
                                                         QRAPI_SizeT                        index)
{
    try
    {
        // no handle?
        if (!handle)
            return NULL;

        // is index out of bounds?
        if (index >= handle->size())
            return NULL;

        // get vertex buffer instance
        return (*handle)[index];
    }
    catch (...)
    {}

    return NULL;
}
//---------------------------------------------------------------------------
QRAPI_EResult QR_ENGINE_API Delete_VertexBuffer(QRAPI_VertexBuffer_Handle handle)
{
    try
    {
        // nothing to delete?
        if (!handle)
            return QRAPI_R_Success;

        // delete vertex buffer instance
        delete handle;

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_EResult QR_ENGINE_API Get_VertexBuffer_Type(QRAPI_VertexBuffer_Handle handle,
                                                  QRAPI_EVertexType*        pResult)
{
    try
    {
        // no handle?
        if (!handle)
            return QRAPI_R_Param_Not_Initialized;

        // no result value?
        if (!pResult)
            return QRAPI_R_Param_Not_Initialized;

        // get vertex buffer type
        *pResult = handle->m_Type;

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_EResult QR_ENGINE_API Get_VertexBuffer_Size(QRAPI_VertexBuffer_Handle handle,
                                                  QRAPI_SizeT*              pResult)
{
    try
    {
        // no handle?
        if (!handle)
            return QRAPI_R_Param_Not_Initialized;

        // no result value?
        if (!pResult)
            return QRAPI_R_Param_Not_Initialized;

        // get vertex buffer size
        *pResult = handle->m_Data.size();

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_EResult QR_ENGINE_API Get_VertexBuffer_Data(QRAPI_VertexBuffer_Handle handle,
                                                  void*                     pData)
{
    try
    {
        // no handle?
        if (!handle)
            return QRAPI_R_Param_Not_Initialized;

        // no output data?
        if (!pData)
            return QRAPI_R_Param_Not_Initialized;

        // copy data
        std::memcpy(pData, &handle->m_Data[0], sizeof(M_QRAPI_Precision) * handle->m_Data.size());

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------

