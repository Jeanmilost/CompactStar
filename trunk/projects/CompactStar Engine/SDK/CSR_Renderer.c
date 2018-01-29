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
        // configure the culling
        switch (pMesh->m_pVB[i].m_Culling.m_Type)
        {
            case CSR_CT_None:  glDisable(GL_CULL_FACE); glCullFace(GL_NONE);           break;
            case CSR_CT_Front: glEnable(GL_CULL_FACE);  glCullFace(GL_FRONT);          break;
            case CSR_CT_Back:  glEnable(GL_CULL_FACE);  glCullFace(GL_BACK);           break;
            case CSR_CT_Both:  glEnable(GL_CULL_FACE);  glCullFace(GL_FRONT_AND_BACK); break;
            default:           glDisable(GL_CULL_FACE); glCullFace(GL_NONE);           break;
        }

        // configure the culling face
        switch (pMesh->m_pVB[i].m_Culling.m_Face)
        {
            case CSR_CF_CW:  glFrontFace(GL_CW);  break;
            case CSR_CF_CCW: glFrontFace(GL_CCW); break;
        }

        // vertices have UV texture coordinates?
        if (pMesh->m_pVB[i].m_Format.m_HasTexCoords)
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
        if (pMesh->m_pVB[i].m_Format.m_HasNormal)
            glEnableVertexAttribArray(pShader->m_NormalSlot);

        // enable texture slot
        if (pMesh->m_pVB[i].m_Format.m_HasTexCoords)
            glEnableVertexAttribArray(pShader->m_TexCoordSlot);

        // enable color slot
        if (pMesh->m_pVB[i].m_Format.m_HasPerVertexColor)
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

        // vertices have normals?
        if (pMesh->m_pVB[i].m_Format.m_HasNormal)
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

        // vertices have UV texture coordinates?
        if (pMesh->m_pVB[i].m_Format.m_HasTexCoords)
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

        // vertices have color?
        if (pMesh->m_pVB[i].m_Format.m_HasPerVertexColor)
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
        if (pMesh->m_pVB[i].m_Format.m_HasNormal)
            glDisableVertexAttribArray(pShader->m_NormalSlot);

        // disable texture slot
        if (pMesh->m_pVB[i].m_Format.m_HasTexCoords)
            glDisableVertexAttribArray(pShader->m_TexCoordSlot);

        // disable color slot
        if (pMesh->m_pVB[i].m_Format.m_HasPerVertexColor)
            glDisableVertexAttribArray(pShader->m_ColorSlot);
    }
}
//---------------------------------------------------------------------------
void csrSceneDrawMDL(const CSR_MDL*    pMDL,
                           CSR_Shader* pShader,
                           size_t      textureIndex,
                           size_t      modelIndex,
                           size_t      meshIndex)
{
    // get the current model mesh to draw
    const CSR_Mesh* pMesh = csrMDLGetMesh(pMDL, modelIndex, meshIndex);

    // found it?
    if (!pMesh)
        return;

    // normally each mesh should contain only one vertex buffer
    if (pMesh->m_Count != 1)
        // unsupported if not (because cannot know which texture should be binded. If a such model
        // exists, a custom version of this function should also be written for it)
        return;

    // can use texture?
    if (pMesh->m_pVB->m_Format.m_HasTexCoords && textureIndex < pMDL->m_TextureCount &&
            pMDL->m_pTexture[textureIndex].m_TextureID != GL_INVALID_VALUE)
        {
            // select the texture sampler to use (GL_TEXTURE0 for normal textures)
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(pShader->m_TextureSlot, GL_TEXTURE0);

            // bind the texure to use
            glBindTexture(GL_TEXTURE_2D, pMDL->m_pTexture[textureIndex].m_TextureID);
        }

    // draw the model mesh
    csrSceneDrawMesh(pMesh, pShader);
}
//---------------------------------------------------------------------------
