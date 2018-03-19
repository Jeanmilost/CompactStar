/****************************************************************************
 * ==> CSR_Serializer ------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the functions required for the        *
 *               serialization of models and scenes                         *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2018, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

#include "CSR_Serializer.h"

//---------------------------------------------------------------------------
// Global defines
//---------------------------------------------------------------------------
#define M_CSR_Scene_File_Header_Version 1
//---------------------------------------------------------------------------
// Write functions
//---------------------------------------------------------------------------
int csrSerializerWriteVB(const CSR_VertexBuffer* pVB, CSR_Buffer* pBuffer)
{
    size_t              i;
    unsigned            length;
    CSR_SceneFileHeader header;

    // validate the input
    if (!pVB || !pBuffer)
        return 0;

    // populate the common header data
    header.m_Length  = sizeof(CSR_SceneFileHeader);
    header.m_Version = M_CSR_Scene_File_Header_Version;

    // populate the vertex format header to write
    header.m_HeaderType = CSR_HT_VertexFormat;
    header.m_DataCount  = 1;
    header.m_DataLength = sizeof(CSR_VertexFormat);

    // write the vertex format header
    if (!csrBufferWrite(pBuffer, &header, header.m_Length, 1))
        return 0;

    // write the vertex format content
    if (!csrBufferWrite(pBuffer, &pVB->m_Format, header.m_DataLength, header.m_DataCount))
        return 0;

    // populate the vertex culling header to write
    header.m_HeaderType = CSR_HT_VertexCulling;
    header.m_DataCount  = 1;
    header.m_DataLength = sizeof(CSR_VertexCulling);

    // write the vertex format header
    if (!csrBufferWrite(pBuffer, &header, header.m_Length, 1))
        return 0;

    // write the vertex culling content
    if (!csrBufferWrite(pBuffer, &pVB->m_Culling, header.m_DataLength, header.m_DataCount))
        return 0;

    // populate the vertex material header to write
    header.m_HeaderType = CSR_HT_VertexMaterial;
    header.m_DataCount  = 1;
    header.m_DataLength = sizeof(CSR_Material);

    // write the vertex format header
    if (!csrBufferWrite(pBuffer, &header, header.m_Length, 1))
        return 0;

    // write the vertex material content
    if (!csrBufferWrite(pBuffer, &pVB->m_Material, header.m_DataLength, header.m_DataCount))
        return 0;

    // write the animation time
    if (!csrBufferWrite(pBuffer, &pVB->m_Time, sizeof(double), 1))
        return 0;

    // get the vertex buffer length
    length = (unsigned)pVB->m_Count * sizeof(float);

    // write the vertex data size
    if (!csrBufferWrite(pBuffer, &length, sizeof(unsigned), 1))
        return 0;

    // write the vertex data content
    if (!csrBufferWrite(pBuffer, pVB->m_pData, sizeof(float), pVB->m_Count))
        return 0;

    return 1;
}
//---------------------------------------------------------------------------
