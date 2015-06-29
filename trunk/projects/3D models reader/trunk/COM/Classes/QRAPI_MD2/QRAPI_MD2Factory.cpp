/******************************************************************************
 * ==> QRAPI_MD2Factory ------------------------------------------------------*
 ******************************************************************************
 * Description : MD2 model COM object factory                                 *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#include "QRAPI_MD2Factory.h"

// interface
#include "QRAPI_VertexBufferFactory.h"

// libraries
#include <gl\gl.h>

//---------------------------------------------------------------------------
// QRAPI_MD2Factory - c++ COM API
//---------------------------------------------------------------------------
__fastcall QRAPI_MD2Factory::QRAPI_MD2Factory() :
    TInterfacedObject(),
    QRAPI_IMD2()
{
    ::InterlockedIncrement(&g_nComObjsInUse);
}
//---------------------------------------------------------------------------
__fastcall QRAPI_MD2Factory::~QRAPI_MD2Factory()
{
    ::InterlockedDecrement(&g_nComObjsInUse);
}
//---------------------------------------------------------------------------
QRAPI_EResult __stdcall QRAPI_MD2Factory::LoadMesh(const QRAPI_Char*  pFileName,
                                                   const QRAPI_Float* pVersion)
{
    try
    {
        // no file name?
        if (!pFileName)
            return QRAPI_R_Param_Not_Initialized;

        // load mesh
        if (!m_MD2.LoadMesh(std::string(pFileName),
                            pVersion ? *pVersion : M_MD2_Mesh_File_Version))
            return QRAPI_R_Internal_Call_Failed;

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_EResult __stdcall QRAPI_MD2Factory::LoadNormalsTable(const QRAPI_Char*  pFileName,
                                                           const QRAPI_Float* pVersion)
{
    try
    {
        // no file name?
        if (!pFileName)
            return QRAPI_R_Param_Not_Initialized;

        // load mesh normals table
        if (!m_MD2.LoadNormalsTable(std::string(pFileName),
                                    pVersion ? *pVersion : M_MD2_Normals_Table_File_Version))
            return QRAPI_R_Internal_Call_Failed;

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_EResult __stdcall QRAPI_MD2Factory::SetAnimation(QRAPI_SizeT       start,
                                                       QRAPI_SizeT       end,
                                                       const QRAPI_Char* pName)
{
    try
    {
        // populate animation info
        QR_MD2::IAnimationInfo animationInfo;
        animationInfo.m_Start = start;
        animationInfo.m_End   = end;

        // no name?
        if (pName)
            animationInfo.m_Name = std::string(pName);

        // set model animation
        if (!m_MD2.SetAnimation(animationInfo))
            return QRAPI_R_Internal_Call_Failed;

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_EResult __stdcall QRAPI_MD2Factory::Animate(const QRAPI_Double* pElapsedTime, QRAPI_SizeT fps)
{
    try
    {
        // no elapsed time?
        if (!pElapsedTime)
            return QRAPI_R_Param_Not_Initialized;

        // animate model
        if (!m_MD2.Animate(*pElapsedTime, fps))
            return QRAPI_R_Internal_Call_Failed;

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_EResult __stdcall QRAPI_MD2Factory::GetVertexBuffer(_di_QRAPI_IVertexBuffers pVBs,
                                                          QRAPI_EVertexFormat      vertexFormat,
                                                          bool                     convertRHLH) const
{
    try
    {
        // no vertex buffer container to write to?
        if (!pVBs)
            return QRAPI_R_Param_Not_Initialized;

        QR_MD2::ILight            light;
        QR_MD2::IVertexBufferList vbs;

        // get md2 vertex buffers
        if (!m_MD2.GetVertexBuffer(vbs,
                                   QR_MD2::IE_L_Native,
                                   light,
                                   (QR_Vertex::IEFormat)vertexFormat,
                                   convertRHLH))
        {
            // clear vertex buffer list
            for (QRAPI_SizeT i = 0; i < vbs.size(); ++i)
                delete vbs[i];

            return QRAPI_R_Get_Vertex_Buffer;
        }

        // get vertex buffers count
        const QRAPI_SizeT count = vbs.size();

        // iterate through vertex buffers to copy
        for (QRAPI_SizeT i = 0; i < count; ++i)
        {
            // copy buffer content
            std::auto_ptr<QRAPI_IVertexBuffer> pVB(QRAPI_CreateVertexBuffer());
            pVB->SetType((QRAPI_EVertexType)vbs[i]->m_Type);

            // get vertex buffer size
            const QRAPI_SizeT vertexSize = vbs[i]->m_Buffer.size();

            // copy buffer data
            pVB->SetData(&vbs[i]->m_Buffer[0], vertexSize);

            // delete useless buffer
            delete vbs[i];

            // add newly copied buffer
            pVBs->Add((_di_QRAPI_IVertexBuffer)pVB.release());
        }

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_EResult __stdcall QRAPI_MD2Factory::Draw_To_OpenGL_1(QRAPI_EVertexFormat vertexFormat,
                                                           bool                convertRHLH) const
{
    try
    {
        // calculate vertex size
        QRAPI_SizeT vertexSize = 3;

        // do include normals?
        if (vertexFormat & QRAPI_VF_Normals)
            vertexSize += 3;

        // do include texture coordinates?
        if (vertexFormat & QRAPI_VF_TexCoords)
            vertexSize += 2;

        // do include vertex color?
        if (vertexFormat & QRAPI_VF_Colors)
            vertexSize += 4;

        QR_MD2::ILight            light;
        QR_MD2::IVertexBufferList vbs;

        // get md2 vertex buffers
        if (!m_MD2.GetVertexBuffer(vbs,
                                   QR_MD2::IE_L_Native,
                                   light,
                                   (QR_Vertex::IEFormat)vertexFormat,
                                   convertRHLH))
        {
            // clear vertex buffer list
            for (QRAPI_SizeT i = 0; i < vbs.size(); ++i)
                delete vbs[i];

            return QRAPI_R_Get_Vertex_Buffer;
        }

        // get vertex buffer count
        const QRAPI_SizeT vbCount = vbs.size();

        // iterate through vertex buffers to draw
        for (QRAPI_SizeT i = 0; i < vbCount; ++i)
        {
            // get vertex buffer
            QR_MD2::IVertexBuffer* pVB = vbs[i];

            // found it?
            if (!pVB)
                continue;

            // bind vertex array
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3,
                            GL_FLOAT,
                            vertexSize * sizeof(QRAPI_VertexBufferDataType),
                            &pVB->m_Buffer[0]);

            std::size_t offset = 3;

            // bind normals array
            if (vertexFormat & QRAPI_VF_Normals)
            {
                glEnableClientState(GL_NORMAL_ARRAY);
                glNormalPointer(GL_FLOAT,
                                vertexSize * sizeof(QRAPI_VertexBufferDataType),
                                &pVB->m_Buffer[offset]);

                offset += 3;
            }

            // bind texture coordinates array
            if (vertexFormat & QRAPI_VF_TexCoords)
            {
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexCoordPointer(2,
                                  GL_FLOAT,
                                  vertexSize * sizeof(QRAPI_VertexBufferDataType),
                                  &pVB->m_Buffer[offset]);

                offset += 2;
            }

            // bind colors array
            if (vertexFormat & QRAPI_VF_Colors)
            {
                glEnableClientState(GL_COLOR_ARRAY);
                glColorPointer(4,
                               GL_FLOAT,
                               vertexSize * sizeof(QRAPI_VertexBufferDataType),
                               &pVB->m_Buffer[offset]);
            }

            // draw MD2 mesh
            switch (pVB->m_Type)
            {
                case QRAPI_VT_TriangleStrip:
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, pVB->m_Buffer.size() / vertexSize);
                    break;

                case QRAPI_VT_TriangleFan:
                    glDrawArrays(GL_TRIANGLE_FAN, 0, pVB->m_Buffer.size() / vertexSize);
                    break;
            }

            // unbind vertex array
            glDisableClientState(GL_VERTEX_ARRAY);

            // unbind normals array
            if (vertexFormat & QRAPI_VF_Normals)
                glDisableClientState(GL_NORMAL_ARRAY);

            // unbind texture coordinates array
            if (vertexFormat & QRAPI_VF_TexCoords)
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);

            // unbind colors array
            if (vertexFormat & QRAPI_VF_Colors)
                glDisableClientState(GL_COLOR_ARRAY);

            glFlush();

            // now vertex buffer is useless, delete it
            delete pVB;
        }
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
HRESULT __stdcall QRAPI_MD2Factory::QueryInterface(const GUID& IID, void **Obj)
{
    return GetInterface(IID, Obj) ? S_OK : E_NOINTERFACE;
}
//---------------------------------------------------------------------------
ULONG __stdcall QRAPI_MD2Factory::AddRef()
{
    return TInterfacedObject::_AddRef();
}
//---------------------------------------------------------------------------
ULONG __stdcall QRAPI_MD2Factory::Release()
{
    return TInterfacedObject::_Release();
}
//---------------------------------------------------------------------------

