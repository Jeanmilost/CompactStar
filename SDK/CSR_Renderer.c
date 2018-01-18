/****************************************************************************
 * ==> CSR_Renderer --------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the functions to draw a scene         *
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

#include "CSR_Renderer.h"

//---------------------------------------------------------------------------
void csrSceneBegin(float r, float g, float b, float a)
{
    // clear scene background and depth buffer
    glClearColor(r, g, b, a);
    glClearDepthf(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
//---------------------------------------------------------------------------
void csrSceneEnd()
{}
//---------------------------------------------------------------------------
void csrSceneDrawMesh(const CSR_Mesh* pMesh, CSR_Shader* pShader)
{
    GLvoid* pCoords;
    GLvoid* pNormals;
    GLvoid* pTexCoords;
    GLvoid* pColors;
    size_t  i;
    size_t  offset;
    size_t  vertexCount;

    // iterate through the vertex buffers composing the mesh to draw
    for (i = 0; i < pMesh->m_Count; ++i)
    {
        // do use texture?
        if (pMesh->m_pVB[i].m_Format.m_UseTextures)
        {
            // a texture is defined for this mesh?
            if (pMesh->m_Shader.m_TextureID != GL_INVALID_VALUE)
            {
                // select the texture sampler to use (GL_TEXTURE0 for normal textures)
                glActiveTexture(GL_TEXTURE0);
                glUniform1i(pShader->m_TextureSlot, GL_TEXTURE0);

                // bind the texure to use
                glBindTexture(GL_TEXTURE_2D, pMesh->m_Shader.m_TextureID);
            }

            // a bump map is defined for this mesh?
            if (pMesh->m_Shader.m_BumpMapID != GL_INVALID_VALUE)
            {
                // select the texture sampler to use (GL_TEXTURE1 for bump map textures)
                glActiveTexture(GL_TEXTURE1);
                glUniform1i(pShader->m_BumpMapSlot, GL_TEXTURE1);

                // bind the texure to use
                glBindTexture(GL_TEXTURE_2D, pMesh->m_Shader.m_BumpMapID);
            }
        }

        // check if vertex buffer is empty, skip to next if yes
        if (!pMesh->m_pVB[i].m_Count || !pMesh->m_pVB[i].m_Format.m_Stride)
            continue;

        // enable position slot
        glEnableVertexAttribArray(pShader->m_VertexSlot);

        // enable normal slot
        if (pMesh->m_pVB[i].m_Format.m_UseNormals)
            glEnableVertexAttribArray(pShader->m_NormalSlot);

        // enable texture slot
        if (pMesh->m_pVB[i].m_Format.m_UseTextures)
            glEnableVertexAttribArray(pShader->m_TexCoordSlot);

        // enable color slot
        if (pMesh->m_pVB[i].m_Format.m_UseColors)
            glEnableVertexAttribArray(pShader->m_ColorSlot);

        offset = 0;

        // send vertices to shader
        pCoords = &pMesh->m_pVB[i].m_pData[offset];
        glVertexAttribPointer(pShader->m_VertexSlot,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              pMesh->m_pVB[i].m_Format.m_Stride * sizeof(float),
                              pCoords);

        offset += 3;

        // do use normals?
        if (pMesh->m_pVB[i].m_Format.m_UseNormals)
        {
            // send normals to shader
            pNormals = &pMesh->m_pVB[i].m_pData[offset];
            glVertexAttribPointer(pShader->m_TexCoordSlot,
                                  3,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  pMesh->m_pVB[i].m_Format.m_Stride * sizeof(float),
                                  pNormals);

            offset += 3;
        }

        // do use textures?
        if (pMesh->m_pVB[i].m_Format.m_UseTextures)
        {
            // send textures to shader
            pTexCoords = &pMesh->m_pVB[i].m_pData[offset];
            glVertexAttribPointer(pShader->m_TexCoordSlot,
                                  2,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  pMesh->m_pVB[i].m_Format.m_Stride * sizeof(float),
                                  pTexCoords);

            offset += 2;
        }

        // do use colors?
        if (pMesh->m_pVB[i].m_Format.m_UseColors)
        {
            // send colors to shader
            pColors = &pMesh->m_pVB[i].m_pData[offset];
            glVertexAttribPointer(pShader->m_ColorSlot,
                                  4,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  pMesh->m_pVB[i].m_Format.m_Stride * sizeof(float),
                                  pColors);
        }

        // calculate the vertex count
        vertexCount = pMesh->m_pVB[i].m_Count / pMesh->m_pVB[i].m_Format.m_Stride;

        // draw the buffer
        switch (pMesh->m_pVB[i].m_Format.m_Type)
        {
            case CSR_VT_Triangles:     glDrawArrays(GL_TRIANGLES,      0, vertexCount); break;
            case CSR_VT_TriangleStrip: glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount); break;
            case CSR_VT_TriangleFan:   glDrawArrays(GL_TRIANGLE_FAN,   0, vertexCount); break;
        }

        // disable vertices slots from shader
        glDisableVertexAttribArray(pShader->m_VertexSlot);

        // disable normal slot
        if (pMesh->m_pVB[i].m_Format.m_UseNormals)
            glDisableVertexAttribArray(pShader->m_NormalSlot);

        // disable texture slot
        if (pMesh->m_pVB[i].m_Format.m_UseTextures)
            glDisableVertexAttribArray(pShader->m_TexCoordSlot);

        // disable color slot
        if (pMesh->m_pVB[i].m_Format.m_UseColors)
            glDisableVertexAttribArray(pShader->m_ColorSlot);
    }
}
//---------------------------------------------------------------------------
void csrSceneDrawMDL(const CSR_MDL*    pMDL,
                           CSR_Shader* pShader,
                           size_t      textureIndex,
                           size_t      modelIndex,
                           size_t      meshindex)
{
    // is model animation index valid?
    if (modelIndex >= pMDL->m_ModelCount)
        return;

    // model contains only one mesh? (i.e. the model isn't animated)
    if (pMDL->m_pModel[modelIndex].m_MeshCount == 1)
    {
        // do use texture?
        if (pMDL->m_pModel[modelIndex].m_pMesh->m_pVB->m_Format.m_UseTextures)
            // is texture index valid and texture is defined?
            if (textureIndex < pMDL->m_TextureCount &&
                pMDL->m_pTexture[textureIndex].m_TextureID != GL_INVALID_VALUE)
            {
                // select the texture sampler to use (GL_TEXTURE0 for normal textures)
                glActiveTexture(GL_TEXTURE0);
                glUniform1i(pShader->m_TextureSlot, GL_TEXTURE0);

                // bind the texure to use
                glBindTexture(GL_TEXTURE_2D, pMDL->m_pTexture[textureIndex].m_TextureID);
            }

        // draw the model mesh
        csrSceneDrawMesh(pMDL->m_pModel[modelIndex].m_pMesh, pShader);
    }
    else
    if (pMDL->m_pModel[modelIndex].m_MeshCount > 1)
    {
        // is mesh index out of bounds?
        if (meshindex >= pMDL->m_pModel[modelIndex].m_MeshCount)
            return;

        // do use texture?
        if (pMDL->m_pModel[modelIndex].m_pMesh[meshindex].m_pVB->m_Format.m_UseTextures)
            // is texture index valid and texture is defined?
            if (textureIndex < pMDL->m_TextureCount &&
                pMDL->m_pTexture[textureIndex].m_TextureID != GL_INVALID_VALUE)
            {
                // select the texture sampler to use (GL_TEXTURE0 for normal textures)
                glActiveTexture(GL_TEXTURE0);
                glUniform1i(pShader->m_TextureSlot, GL_TEXTURE0);

                // bind the texure to use
                glBindTexture(GL_TEXTURE_2D, pMDL->m_pTexture[textureIndex].m_TextureID);
            }

        // draw the model mesh
        csrSceneDrawMesh(&pMDL->m_pModel[modelIndex].m_pMesh[meshindex], pShader);
    }
}
//---------------------------------------------------------------------------
