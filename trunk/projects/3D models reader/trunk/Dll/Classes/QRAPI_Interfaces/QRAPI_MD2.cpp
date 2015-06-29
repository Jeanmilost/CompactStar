/******************************************************************************
 * ==> QRAPI_MD2 -------------------------------------------------------------*
 ******************************************************************************
 * Description : DLL interface for MD2 model                                  *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#include "QRAPI_MD2.h"

// qr engine
#include "QR_MD2.h"

// libraries
#include <Windows.h>
#include <gl\gl.h>
#include <gl\glu.h>

//---------------------------------------------------------------------------
// QRAPI_MD2 interface functions
//---------------------------------------------------------------------------
QRAPI_MD2_Handle QR_ENGINE_API Create_MD2()
{
    try
    {
        // create new MD2 model instance and return handle
        return new QR_MD2();
    }
    catch (...)
    {}

    return NULL;
}
//---------------------------------------------------------------------------
QRAPI_EResult QR_ENGINE_API Delete_MD2(QRAPI_MD2_Handle handle)
{
    try
    {
        // nothing to delete?
        if (!handle)
            return QRAPI_R_Success;

        // delete MD2 model instance
        delete handle;

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_EResult QR_ENGINE_API LoadMesh_MD2(QRAPI_MD2_Handle   handle,
                                         const QRAPI_Char*  pFileName,
                                         const QRAPI_Float* pVersion)
{
    try
    {
        // no handle?
        if (!handle)
            return QRAPI_R_Param_Not_Initialized;

        // no file name?
        if (!pFileName)
            return QRAPI_R_Param_Not_Initialized;

        // load mesh
        if (!handle->LoadMesh(std::string(pFileName),
                             pVersion ? *pVersion : M_MD2_Mesh_File_Version))
            return QRAPI_R_Internal_Call_Failed;

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_EResult QR_ENGINE_API LoadNormalsTable_MD2(QRAPI_MD2_Handle   handle,
                                                 const QRAPI_Char*  pFileName,
                                                 const QRAPI_Float* pVersion)
{
    try
    {
        // no handle?
        if (!handle)
            return QRAPI_R_Param_Not_Initialized;

        // no file name?
        if (!pFileName)
            return QRAPI_R_Param_Not_Initialized;

        // load mesh normals table
        if (!handle->LoadNormalsTable(std::string(pFileName),
                                        pVersion ? *pVersion : M_MD2_Normals_Table_File_Version))
            return QRAPI_R_Internal_Call_Failed;

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_EResult QR_ENGINE_API SetAnimation_MD2(QRAPI_MD2_Handle  handle,
                                             QRAPI_SizeT       start,
                                             QRAPI_SizeT       end,
                                             const QRAPI_Char* pName)
{
    try
    {
        // no handle?
        if (!handle)
            return QRAPI_R_Param_Not_Initialized;

        // populate animation info
        QR_MD2::IAnimationInfo animationInfo;
        animationInfo.m_Start = start;
        animationInfo.m_End   = end;

        // no name?
        if (pName)
            animationInfo.m_Name = std::string(pName);

        // set model animation
        if (!handle->SetAnimation(animationInfo))
            return QRAPI_R_Internal_Call_Failed;

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_EResult QR_ENGINE_API Animate_MD2(QRAPI_MD2_Handle    handle,
                                        const QRAPI_Double* pElapsedTime,
                                        QRAPI_SizeT         fps)
{
    try
    {
        // no handle?
        if (!handle)
            return QRAPI_R_Param_Not_Initialized;

        // no elapsed time?
        if (!pElapsedTime)
            return QRAPI_R_Param_Not_Initialized;

        // animate model
        if (!handle->Animate(*pElapsedTime, fps))
            return QRAPI_R_Internal_Call_Failed;

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_EResult QR_ENGINE_API GetVertexBuffer_MD2(QRAPI_MD2_Handle                   handle,
                                                QRAPI_VertexBufferContainer_Handle hVBs,
                                                QRAPI_EVertexFormat                vertexFormat,
                                                bool                               convertRHLH)
{
    try
    {
        // no handle?
        if (!handle)
            return QRAPI_R_Param_Not_Initialized;

        // no vertex buffer container to write to?
        if (!hVBs)
            return QRAPI_R_Param_Not_Initialized;

        QR_MD2::ILight            light;
        QR_MD2::IVertexBufferList vbs;

        // get md2 vertex buffers
        if (!handle->GetVertexBuffer(vbs,
                                     QR_MD2::IE_L_Native,
                                     light,
                                     (QR_Vertex::IEFormat)vertexFormat,
                                     convertRHLH))
        {
            // clear vertex buffer list
            for (unsigned i = 0; i < vbs.size(); ++i)
                delete vbs[i];

            return QRAPI_R_Get_Vertex_Buffer;
        }

        // get vertex buffers count
        const unsigned count = vbs.size();

        // iterate through vertex buffers to copy
        for (unsigned i = 0; i < count; ++i)
        {
            // copy buffer content
            std::auto_ptr<QRAPI_VertexBuffer> pVB(new QRAPI_VertexBuffer());
            pVB->m_Type = (QRAPI_EVertexType)vbs[i]->m_Type;

            // get vertex buffer size
            const unsigned vertexSize = vbs[i]->m_Buffer.size();

            // copy buffer data
            pVB->m_Data.resize(vertexSize);
            std::memcpy(&pVB->m_Data[0], &vbs[i]->m_Buffer[0], sizeof(M_QRAPI_Precision) * vertexSize);

            // delete useless buffer
            delete vbs[i];

            // add newly copied buffer
            hVBs->push_back(pVB.release());
        }

        return QRAPI_R_Success;
    }
    catch (...)
    {}

    return QRAPI_R_Internal_Exception;
}
//---------------------------------------------------------------------------
QRAPI_EResult QR_ENGINE_API Draw_MD2_To_OpenGL_1(QRAPI_MD2_Handle    handle,
                                                 QRAPI_EVertexFormat vertexFormat,
                                                 bool                convertRHLH)
{
    try
    {
        // no handle?
        if (!handle)
            return QRAPI_R_Param_Not_Initialized;

        // calculate vertex size
        std::size_t vertexSize = 3;

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
        if (!handle->GetVertexBuffer(vbs,
                                     QR_MD2::IE_L_Native,
                                     light,
                                     (QR_Vertex::IEFormat)vertexFormat,
                                     convertRHLH))
        {
            // clear vertex buffer list
            for (unsigned i = 0; i < vbs.size(); ++i)
                delete vbs[i];

            return QRAPI_R_Get_Vertex_Buffer;
        }

        // get vertex buffer count
        const std::size_t vbCount = vbs.size();

        // iterate through vertex buffers to draw
        for (unsigned i = 0; i < vbCount; ++i)
        {
            // get vertex buffer
            QR_MD2::IVertexBuffer* pVB = vbs[i];

            // found it?
            if (!pVB)
                continue;

            // bind vertex array
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, vertexSize * sizeof(M_QRAPI_Precision), &pVB->m_Buffer[0]);

            std::size_t offset = 3;

            // bind normals array
            if (vertexFormat & QRAPI_VF_Normals)
            {
                glEnableClientState(GL_NORMAL_ARRAY);
                glNormalPointer(GL_FLOAT,
                                vertexSize * sizeof(M_QRAPI_Precision),
                                &pVB->m_Buffer[offset]);

                offset += 3;
            }

            // bind texture coordinates array
            if (vertexFormat & QRAPI_VF_TexCoords)
            {
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexCoordPointer(2,
                                  GL_FLOAT,
                                  vertexSize * sizeof(M_QRAPI_Precision),
                                  &pVB->m_Buffer[offset]);

                offset += 2;
            }

            // bind colors array
            if (vertexFormat & QRAPI_VF_Colors)
            {
                glEnableClientState(GL_COLOR_ARRAY);
                glColorPointer(4,
                               GL_FLOAT,
                               vertexSize * sizeof(M_QRAPI_Precision),
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

