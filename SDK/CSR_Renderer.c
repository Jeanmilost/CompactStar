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

// std
#include <stdlib.h>

//---------------------------------------------------------------------------
// Multisample antialiasing shader
//---------------------------------------------------------------------------
#ifndef CSR_OPENGL_2_ONLY
    const char g_MSAA_VertexProgram[] =
        "attribute vec2 csr_vVertex;"
        "attribute vec2 csr_vTexCoord;"
        "varying   vec2 csr_fTexCoord;"
        "void main()"
        "{"
        "    gl_Position   = vec4(csr_vVertex.x, csr_vVertex.y, 0.0f, 1.0f);"
        "    csr_fTexCoord = csr_vTexCoord;"
        "}";
#endif
//---------------------------------------------------------------------------
#ifndef CSR_OPENGL_2_ONLY
    const char g_MSAA_FragmentProgram[] =
        "uniform sampler2D csr_sTexture;"
        "varying vec2      csr_fTexCoord;"
        "void main()"
        "{"
        "    gl_FragColor = texture(csr_sTexture, csr_fTexCoord);"
        "}";
#endif
//---------------------------------------------------------------------------
// Multisample antialiasing constants
//---------------------------------------------------------------------------
#ifndef CSR_OPENGL_2_ONLY
    const float g_MSAA_VB[] =
    {
        // x,   y,    tu,   tv
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f
    };
#endif
//---------------------------------------------------------------------------
// Multisample antialiasing private functions
//---------------------------------------------------------------------------
#ifndef CSR_OPENGL_2_ONLY
    void csrMSAALinkStaticVB(const CSR_Shader* pShader, const void* pCustomData)
    {
        CSR_Buffer buffer;

        // get the multisampling antialiasing sent in custom data
        CSR_MSAA* pMSAA = (CSR_MSAA*)pCustomData;

        // found it?
        if (!pMSAA)
            return;

        // configure the vertex buffer
        buffer.m_pData  = g_MSAA_VB;
        buffer.m_Length = sizeof(g_MSAA_VB);

        // create the shape on which the final texture will be drawn
        pMSAA->m_pStaticBuffer           = csrStaticBufferCreate(pShader, &buffer);
        pMSAA->m_pStaticBuffer->m_Stride = 4;
    }
#endif
//---------------------------------------------------------------------------
#ifndef CSR_OPENGL_2_ONLY
    int csrMSAAConfigure(size_t width, size_t height, size_t factor, CSR_MSAA* pMSAA)
    {
        GLuint msTexture;
        GLuint texture;

        // enable multisampling
        glEnable(GL_MULTISAMPLE);

        // create and bind the frame buffer
        glGenFramebuffers(1, &pMSAA->m_FrameBufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, pMSAA->m_FrameBufferID);

        // create a new texture
        glGenTextures(1, &msTexture);

        // bind texture to sampler
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msTexture);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, factor, GL_RGB, width, height, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

        // add texture to the frame buffer
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D_MULTISAMPLE,
                               msTexture,
                               0);

        // create and bind the render buffer for depth and stencil attachments
        glGenRenderbuffers(1, &pMSAA->m_RenderBufferID);
        glBindRenderbuffer(GL_RENDERBUFFER, pMSAA->m_RenderBufferID);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER,
                                         factor,
                                         GL_DEPTH24_STENCIL8,
                                         width,
                                         height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER,
                                  pMSAA->m_RenderBufferID);

        // build the render buffer
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            return 0;

        // unbind the frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // create multisampled output texture
        glGenTextures(1, &pMSAA->m_TextureID);
        glBindTexture(GL_TEXTURE_2D, pMSAA->m_TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

        // configure texture filters to use and bind texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        // create and bind a multisampled texture buffer
        glGenFramebuffers(1, &pMSAA->m_TextureBufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, pMSAA->m_TextureBufferID);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D,
                               pMSAA->m_TextureID,
                               0);

        // build the multisampled texture buffer
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            return 0;

        // unbind the texture buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // set the scene size
        pMSAA->m_Width  = width;
        pMSAA->m_Height = height;
        pMSAA->m_Factor = factor;

        return 1;
    }
#endif
//---------------------------------------------------------------------------
// Multisample antialiasing functions
//---------------------------------------------------------------------------
#ifndef CSR_OPENGL_2_ONLY
    CSR_MSAA* csrMSAACreate(size_t width, size_t height, size_t factor)
    {
        // create a new multisample antialiasing
        CSR_MSAA* pMSAA = (CSR_MSAA*)malloc(sizeof(CSR_MSAA));

        // succeeded?
        if (!pMSAA)
            return 0;

        // initialize the multisample antialiasing content
        if (!csrMSAAInit(width, height, factor, pMSAA))
        {
            csrMSAARelease(pMSAA);
            return 0;
        }

        return pMSAA;
    }
#endif
//---------------------------------------------------------------------------
#ifndef CSR_OPENGL_2_ONLY
    void csrMSAARelease(CSR_MSAA* pMSAA)
    {
        // no multisampling antialiasing to release?
        if (!pMSAA)
            return;

        // enable the MSAA shader
        if (pMSAA->m_pShader)
            csrShaderEnable(pMSAA->m_pShader);

        // delete the multisampled texture
        if (pMSAA->m_TextureID)
            glDeleteTextures(1, &pMSAA->m_TextureID);

        // delete the multisampled texture buffer
        if (pMSAA->m_TextureBufferID)
            glDeleteFramebuffers(1, &pMSAA->m_TextureBufferID);

        // delete the render buffer
        if (pMSAA->m_RenderBufferID)
            glDeleteRenderbuffers(1, &pMSAA->m_RenderBufferID);

        // delete the frame buffer
        if (pMSAA->m_FrameBufferID)
            glDeleteFramebuffers(1, &pMSAA->m_FrameBufferID);

        // delete the shader
        if (pMSAA->m_pShader)
            csrShaderRelease(pMSAA->m_pShader);

        // delete the static buffer
        if (pMSAA->m_pStaticBuffer)
            csrStaticBufferRelease(pMSAA->m_pStaticBuffer);

        // delete the multisampling antialiasing
        free(pMSAA);
    }
#endif
//---------------------------------------------------------------------------
#ifndef CSR_OPENGL_2_ONLY
    int csrMSAAInit(size_t width, size_t height, size_t factor, CSR_MSAA* pMSAA)
    {
        // no multisample antialiasing to initialize?
        if (!pMSAA)
            return 0;

        // validate input
        if (factor != 2 && factor != 4 && factor != 8)
            return 0;

        // load the shader to use for MSAA
        pMSAA->m_pShader = csrShaderLoadFromStr(g_MSAA_VertexProgram,
                                                sizeof(g_MSAA_VertexProgram),
                                                g_MSAA_FragmentProgram,
                                                sizeof(g_MSAA_FragmentProgram),
                                                csrMSAALinkStaticVB,
                                                pMSAA);

        // succeeded?
        if (!pMSAA->m_pShader)
            return 0;

        // the static buffer containing the final surface to show was generated successfully?
        if (!pMSAA->m_pStaticBuffer)
            return 0;

        // enable the MSAA shader
        csrShaderEnable(pMSAA->m_pShader);

        // get the vertex slot
        pMSAA->m_pShader->m_VertexSlot =
                glGetAttribLocation(pMSAA->m_pShader->m_ProgramID, "csr_vVertex");

        // found it?
        if (pMSAA->m_pShader->m_VertexSlot == -1)
            return 0;

        // get the texture coordinates slot
        pMSAA->m_pShader->m_TexCoordSlot =
                glGetAttribLocation(pMSAA->m_pShader->m_ProgramID, "csr_vTexCoord");

        // found it?
        if (pMSAA->m_pShader->m_TexCoordSlot == -1)
            return 0;

        // get the texture sampler slot
        pMSAA->m_pShader->m_TextureSlot =
                glGetUniformLocation(pMSAA->m_pShader->m_ProgramID, "csr_sTexture");

        // found it?
        if (pMSAA->m_pShader->m_TextureSlot == -1)
            return 0;

        // configure the multisample antialiasing
        return csrMSAAConfigure(width, height, factor, pMSAA);
    }
#endif
//---------------------------------------------------------------------------
#ifndef CSR_OPENGL_2_ONLY
    int csrMSAAChangeSize(size_t width, size_t height, CSR_MSAA* pMSAA)
    {
        // validate the input
        if (!pMSAA)
            return 0;

        // delete the multisampled texture
        if (pMSAA->m_TextureID)
        {
            glDeleteTextures(1, &pMSAA->m_TextureID);
            pMSAA->m_TextureID = M_CSR_Error_Code;
        }

        // delete the multisampled texture buffer
        if (pMSAA->m_TextureBufferID)
        {
            glDeleteFramebuffers(1, &pMSAA->m_TextureBufferID);
            pMSAA->m_TextureBufferID = M_CSR_Error_Code;
        }

        // delete the render buffer
        if (pMSAA->m_RenderBufferID)
        {
            glDeleteRenderbuffers(1, &pMSAA->m_RenderBufferID);
            pMSAA->m_RenderBufferID = M_CSR_Error_Code;
        }

        // delete the frame buffer
        if (pMSAA->m_FrameBufferID)
        {
            glDeleteFramebuffers(1, &pMSAA->m_FrameBufferID);
            pMSAA->m_FrameBufferID = M_CSR_Error_Code;
        }

        // recreate the multisample antialiasing
        return csrMSAAConfigure(width, height, pMSAA->m_Factor, pMSAA);
    }
#endif
//---------------------------------------------------------------------------
#ifndef CSR_OPENGL_2_ONLY
    void csrMSAASceneBegin(const CSR_Color* pColor, const CSR_MSAA* pMSAA)
    {
        // do apply a multisample antialiasing on the scene
        if (pMSAA && pMSAA->m_pShader)
        {
            // enable the MSAA shader
            csrShaderEnable(pMSAA->m_pShader);

            // enable multisampling
            glEnable(GL_MULTISAMPLE);

            // bind the frame buffer on which the scene should be drawn
            glBindFramebuffer(GL_FRAMEBUFFER, pMSAA->m_FrameBufferID);
        }

        // begin the scene
        csrSceneBegin(pColor);
    }
#endif
//---------------------------------------------------------------------------
#ifndef CSR_OPENGL_2_ONLY
    void csrMSAASceneEnd(const CSR_MSAA* pMSAA)
    {
        // end the scene
        csrSceneEnd();

        // do finalize the multisampling antialiasing effect?
        if (pMSAA && pMSAA->m_pShader && pMSAA->m_pStaticBuffer)
        {
            // configure the culling
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            glFrontFace(GL_CW);

            // disable the alpha blending
            glDisable(GL_BLEND);

            // set polygon mode to fill
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            // enable the MSAA shader
            csrShaderEnable(pMSAA->m_pShader);

            // blit the multisampled buffer containing the drawn scene to the output texture buffer
            glBindFramebuffer(GL_READ_FRAMEBUFFER, pMSAA->m_FrameBufferID);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pMSAA->m_TextureBufferID);
            glBlitFramebuffer(0,
                              0,
                              pMSAA->m_Width,
                              pMSAA->m_Height,
                              0,
                              0,
                              pMSAA->m_Width,
                              pMSAA->m_Height,
                              GL_COLOR_BUFFER_BIT,
                              GL_NEAREST);

            // unbind the frame buffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // configure the depth testing
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);

            // select the texture sampler to use (GL_TEXTURE0 for normal textures)
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(pMSAA->m_pShader->m_TextureSlot, GL_TEXTURE0);

            // bind the texure to use
            glBindTexture(GL_TEXTURE_2D, pMSAA->m_TextureID);

            // bind the VBO containing the shape to draw
            glBindBuffer(GL_ARRAY_BUFFER, pMSAA->m_pStaticBuffer->m_BufferID);

            // enable the vertices
            glEnableVertexAttribArray(pMSAA->m_pShader->m_VertexSlot);
            glVertexAttribPointer(pMSAA->m_pShader->m_VertexSlot,
                                  2,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  pMSAA->m_pStaticBuffer->m_Stride * sizeof(float),
                                  0);

            // enable the texture coordinates
            glEnableVertexAttribArray(pMSAA->m_pShader->m_TexCoordSlot);
            glVertexAttribPointer(pMSAA->m_pShader->m_TexCoordSlot,
                                  2,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  pMSAA->m_pStaticBuffer->m_Stride * sizeof(float),
                                  (void*)(2 * sizeof(float)));

            // draw the surface
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            // disable the vertex attribute arrays
            glDisableVertexAttribArray(pMSAA->m_pShader->m_TexCoordSlot);
            glDisableVertexAttribArray(pMSAA->m_pShader->m_VertexSlot);

            // unbind the VBO
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
#endif
//---------------------------------------------------------------------------
// Scene item functions
//---------------------------------------------------------------------------
CSR_SceneItem* csrSceneItemCreate(void)
{
    // create a new scene item
    CSR_SceneItem* pSceneItem = (CSR_SceneItem*)malloc(sizeof(CSR_SceneItem));

    // succeeded?
    if (!pSceneItem)
        return 0;

    // initialize the scene item content
    csrSceneItemInit(pSceneItem);

    return pSceneItem;
}
//---------------------------------------------------------------------------
void csrSceneItemRelease(CSR_SceneItem* pSceneItem)
{
    size_t i;

    // no scene item to release?
    if (!pSceneItem)
        return;

    // release the model
    if (pSceneItem->m_pModel)
        switch (pSceneItem->m_Type)
        {
            case CSR_MT_Mesh:  csrMeshRelease(pSceneItem->m_pModel);  break;
            case CSR_MT_Model: csrModelRelease(pSceneItem->m_pModel); break;
            case CSR_MT_MDL:   csrMDLRelease(pSceneItem->m_pModel);   break;
        }

    // release the aligned-axis bounding box tree
    if (pSceneItem->m_pAABBTree)
        csrAABBTreeRelease(pSceneItem->m_pAABBTree);

    // release the matrix list
    if (pSceneItem->m_pMatrixList)
    {
        // iterate through each matrix and release each of them
        for (i = 0; i < pSceneItem->m_pMatrixList->m_Count; ++i)
            free(pSceneItem->m_pMatrixList->m_pMatrix);

        free(pSceneItem->m_pMatrixList);
    }

    // NOTE don't release the shader, as it's just linked with the item, not owned
}
//---------------------------------------------------------------------------
void csrSceneItemInit(CSR_SceneItem* pSceneItem)
{
    // no scene item to initialize?
    if (!pSceneItem)
        return;

    pSceneItem->m_pModel       = 0;
    pSceneItem->m_Type         = CSR_MT_Model;
    pSceneItem->m_pMatrixList  = 0;
    pSceneItem->m_pShader      = 0;
    pSceneItem->m_pAABBTree    = 0;
    pSceneItem->m_TextureIndex = 0;
    pSceneItem->m_ModelIndex   = 0;
    pSceneItem->m_MeshIndex    = 0;
    pSceneItem->m_Visible      = 1;
}
//---------------------------------------------------------------------------
void csrSceneItemDraw(const CSR_Scene* pScene, const CSR_SceneItem* pSceneItem)
{
    GLint slot;

    // validate the input
    if (!pScene || !pSceneItem)
        return;

    // is scene item visible?
    if (!pSceneItem->m_Visible)
        return;

    // enable the item shader
    csrShaderEnable(pSceneItem->m_pShader);

    // get the view matrix slot from shader
    slot = glGetUniformLocation(pSceneItem->m_pShader->m_ProgramID, "csr_uView");

    // connect view matrix to shader
    if (slot >= 0)
        glUniformMatrix4fv(slot, 1, 0, &pScene->m_Matrix.m_Table[0][0]);

    // draw the model
    switch (pSceneItem->m_Type)
    {
        case CSR_MT_Mesh:
            csrSceneDrawMesh((const CSR_Mesh*)pSceneItem->m_pModel,
                                              pSceneItem->m_pShader,
                                              pSceneItem->m_pMatrixList);
            break;

        case CSR_MT_Model:
            csrSceneDrawModel((const CSR_Model*)pSceneItem->m_pModel,
                                                pSceneItem->m_ModelIndex,
                                                pSceneItem->m_pShader,
                                                pSceneItem->m_pMatrixList);
            break;

        case CSR_MT_MDL:
            csrSceneDrawMDL((const CSR_MDL*)pSceneItem->m_pModel,
                                            pSceneItem->m_pShader,
                                            pSceneItem->m_pMatrixList,
                                            pSceneItem->m_TextureIndex,
                                            pSceneItem->m_ModelIndex,
                                            pSceneItem->m_MeshIndex);
            break;
    }

    // disable the item shader
    csrShaderEnable(0);
}
//---------------------------------------------------------------------------
// Scene functions
//---------------------------------------------------------------------------
CSR_Scene* csrSceneCreate(void)
{
    // create a new scene
    CSR_Scene* pScene = (CSR_Scene*)malloc(sizeof(CSR_Scene));

    // succeeded?
    if (!pScene)
        return 0;

    // initialize the scene content
    csrSceneInit(pScene);

    return pScene;
}
//---------------------------------------------------------------------------
void csrSceneRelease(CSR_Scene* pScene)
{
    size_t i;

    // no scene to release?
    if (!pScene)
        return;

    // do free the normal items content?
    if (pScene->m_pItem)
    {
        // iterate through each scene item to release, and release each of them
        for (i = 0; i < pScene->m_ItemCount; ++i)
            csrSceneItemRelease(&pScene->m_pItem[i]);

        // free the scene items
        free(pScene->m_pItem);
    }

    // do free the transparent items content?
    if (pScene->m_pTransparentItem)
    {
        // iterate through each scene transparent item to release, and release each of them
        for (i = 0; i < pScene->m_TransparentItemCount; ++i)
            csrSceneItemRelease(&pScene->m_pTransparentItem[i]);

        // free the scene transparent items
        free(pScene->m_pTransparentItem);
    }

    #ifndef CSR_OPENGL_2_ONLY
        // do free the antialiasing structure?
        if (pScene->m_pMSAA)
            // free the antialiasing structure
            csrMSAARelease(pScene->m_pMSAA);
    #endif

    // free the scene
    free(pScene);
}
//---------------------------------------------------------------------------
void csrSceneInit(CSR_Scene* pScene)
{
    // no scene to initialize?
    if (!pScene)
        return;

    // initialize the scene
    pScene->m_Color.m_R            = 0.0f;
    pScene->m_Color.m_G            = 0.0f;
    pScene->m_Color.m_B            = 0.0f;
    pScene->m_Color.m_A            = 1.0f;
    pScene->m_pItem                = 0;
    pScene->m_ItemCount            = 0;
    pScene->m_pTransparentItem     = 0;
    pScene->m_TransparentItemCount = 0;

    // set the default item matrix to identity
    csrMat4Identity(&pScene->m_Matrix);

    #ifndef CSR_OPENGL_2_ONLY
        pScene->m_pMSAA = 0;
    #endif
}
//---------------------------------------------------------------------------
int csrSceneAddMesh(CSR_Scene*  pScene,
                    CSR_Mesh*   pMesh,
                    CSR_Shader* pShader,
                    int         transparent,
                    int         aabb)
{
    CSR_SceneItem* pItem;
    int            index;

    // validate the input data
    if (!pScene || !pMesh)
        return 0;

    // do add a transparent item?
    if (transparent)
    {
        // add a new item to the transparent items
        pItem = (CSR_SceneItem*)csrMemoryAlloc(pScene->m_pTransparentItem,
                                               sizeof(CSR_SceneItem),
                                               pScene->m_TransparentItemCount + 1);

        // succeeded?
        if (!pItem)
            return 0;

        // get the item index to update
        index = pScene->m_TransparentItemCount;
    }
    else
    {
        // add a new item to the scene items
        pItem = (CSR_SceneItem*)csrMemoryAlloc(pScene->m_pItem,
                                               sizeof(CSR_SceneItem),
                                               pScene->m_ItemCount + 1);

        // succeeded?
        if (!pItem)
            return 0;

        // get the scene item index to update
        index = pScene->m_ItemCount;
    }

    // initialize the newly created item with the default values
    csrSceneItemInit(&pItem[index]);

    // configure the item
    pItem[index].m_pModel  = pMesh;
    pItem[index].m_Type    = CSR_MT_Mesh;
    pItem[index].m_pShader = pShader;

    // generate the aligned-axis bounding box tree for this mesh
    if (aabb)
    {
        pItem[index].m_pAABBTree = csrAABBTreeFromMesh(pMesh);

        // succeeded?
        if (!pItem[index].m_pAABBTree)
        {
            // realloc to the previous size, thus the latest added item will be freed
            if (transparent)
                pScene->m_pTransparentItem = (CSR_SceneItem*)csrMemoryAlloc(pItem,
                                                                            sizeof(CSR_SceneItem),
                                                                            pScene->m_TransparentItemCount);
            else
                pScene->m_pItem = (CSR_SceneItem*)csrMemoryAlloc(pItem,
                                                                 sizeof(CSR_SceneItem),
                                                                 pScene->m_ItemCount);

            return 0;
        }
    }

    // do add a transparent item?
    if (transparent)
    {
        // add item to the transparent item list
        pScene->m_pTransparentItem = pItem;
        ++pScene->m_TransparentItemCount;
    }
    else
    {
        // add item to the normal item list
        pScene->m_pItem = pItem;
        ++pScene->m_ItemCount;
    }

    return 1;
}
//---------------------------------------------------------------------------
int csrSceneAddModel(CSR_Scene*  pScene,
                     CSR_Model*  pModel,
                     CSR_Shader* pShader,
                     int         transparent,
                     int         aabb)
{
    CSR_SceneItem* pItem;
    int            index;

    // validate the input data
    if (!pScene || !pModel)
        return 0;

    // do add a transparent item?
    if (transparent)
    {
        // add a new item to the transparent items
        pItem = (CSR_SceneItem*)csrMemoryAlloc(pScene->m_pTransparentItem,
                                               sizeof(CSR_SceneItem),
                                               pScene->m_TransparentItemCount + 1);

        // succeeded?
        if (!pItem)
            return 0;

        // get the item index to update
        index = pScene->m_TransparentItemCount;
    }
    else
    {
        // add a new item to the scene items
        pItem = (CSR_SceneItem*)csrMemoryAlloc(pScene->m_pItem,
                                               sizeof(CSR_SceneItem),
                                               pScene->m_ItemCount + 1);

        // succeeded?
        if (!pItem)
            return 0;

        // get the scene item index to update
        index = pScene->m_ItemCount;
    }

    // initialize the newly created item with the default values
    csrSceneItemInit(&pItem[index]);

    // configure the item
    pItem[index].m_pModel  = pModel;
    pItem[index].m_Type    = CSR_MT_Model;
    pItem[index].m_pShader = pShader;

    // generate the aligned-axis bounding box tree for this model
    if (aabb)
    {
        size_t i;

        // reserve memory for all the AABB trees to create
        pItem[index].m_pAABBTree = (CSR_AABBNode*)csrMemoryAlloc(0,
                                                                 sizeof(CSR_AABBNode),
                                                                 pModel->m_MeshCount);

        // succeeded?
        if (!pItem[index].m_pAABBTree)
        {
            // realloc to the previous size, thus the latest added item will be freed
            if (transparent)
                pScene->m_pTransparentItem =
                        (CSR_SceneItem*)csrMemoryAlloc(pItem,
                                                       sizeof(CSR_SceneItem),
                                                       pScene->m_TransparentItemCount);
            else
                pScene->m_pItem = (CSR_SceneItem*)csrMemoryAlloc(pItem,
                                                                 sizeof(CSR_SceneItem),
                                                                 pScene->m_ItemCount);

            return 0;
        }

        // iterate through the model meshes
        for (i = 0; i < pModel->m_MeshCount; ++i)
        {
            // create a new tree for the mesh
            CSR_AABBNode* pAABBTree = csrAABBTreeFromMesh(&pModel->m_pMesh[i]);

            // succeeded?
            if (!pAABBTree)
            {
                // realloc to the previous size, thus the latest added item will be freed
                if (transparent)
                    pScene->m_pTransparentItem =
                            (CSR_SceneItem*)csrMemoryAlloc(pItem,
                                                           sizeof(CSR_SceneItem),
                                                           pScene->m_TransparentItemCount);
                else
                    pScene->m_pItem = (CSR_SceneItem*)csrMemoryAlloc(pItem,
                                                                     sizeof(CSR_SceneItem),
                                                                     pScene->m_ItemCount);

                return 0;
            }

            // copy the tree content
            memcpy(&pItem[index].m_pAABBTree[i], pAABBTree, sizeof(CSR_AABBNode));

            // release the source tree
            csrAABBTreeRelease(pAABBTree);
        }
    }

    // do add a transparent item?
    if (transparent)
    {
        // add item to the transparent item list
        pScene->m_pTransparentItem = pItem;
        ++pScene->m_TransparentItemCount;
    }
    else
    {
        // add item to the normal item list
        pScene->m_pItem = pItem;
        ++pScene->m_ItemCount;
    }

    return 1;
}
//---------------------------------------------------------------------------
int csrSceneAddMDL(CSR_Scene*  pScene,
                   CSR_MDL*    pMDL,
                   CSR_Shader* pShader,
                   int         transparent,
                   int         aabb)
{
    CSR_SceneItem* pItem;
    int            index;

    // validate the input data
    if (!pScene || !pMDL)
        return 0;

    // do add a transparent item?
    if (transparent)
    {
        // add a new item to the transparent items
        pItem = (CSR_SceneItem*)csrMemoryAlloc(pScene->m_pTransparentItem,
                                               sizeof(CSR_SceneItem),
                                               pScene->m_TransparentItemCount + 1);

        // succeeded?
        if (!pItem)
            return 0;

        // get the item index to update
        index = pScene->m_TransparentItemCount;
    }
    else
    {
        // add a new item to the scene items
        pItem = (CSR_SceneItem*)csrMemoryAlloc(pScene->m_pItem,
                                               sizeof(CSR_SceneItem),
                                               pScene->m_ItemCount + 1);

        // succeeded?
        if (!pItem)
            return 0;

        // get the scene item index to update
        index = pScene->m_ItemCount;
    }

    // initialize the newly created item with the default values
    csrSceneItemInit(&pItem[index]);

    // configure the item
    pItem[index].m_pModel  = pMDL;
    pItem[index].m_Type    = CSR_MT_MDL;
    pItem[index].m_pShader = pShader;

    // generate the aligned-axis bounding box tree for this model
    if (aabb)
    {
        size_t i;
        size_t j;

        // reserve memory for all the AABB trees to create
        pItem[index].m_pAABBTree =
                (CSR_AABBNode*)csrMemoryAlloc(0,
                                              sizeof(CSR_AABBNode),
                                              pMDL->m_ModelCount * pMDL->m_pModel->m_MeshCount);

        // succeeded?
        if (!pItem[index].m_pAABBTree)
        {
            // realloc to the previous size, thus the latest added item will be freed
            if (transparent)
                pScene->m_pTransparentItem = (CSR_SceneItem*)csrMemoryAlloc(pItem,
                                                                            sizeof(CSR_SceneItem),
                                                                            pScene->m_TransparentItemCount);
            else
                pScene->m_pItem = (CSR_SceneItem*)csrMemoryAlloc(pItem,
                                                                 sizeof(CSR_SceneItem),
                                                                 pScene->m_ItemCount);

            return 0;
        }

        // iterate through the model meshes
        for (i = 0; i < pMDL->m_ModelCount; ++i)
            for (j = 0; j < pMDL->m_pModel->m_MeshCount; ++j)
            {
                // create a new tree for the mesh
                CSR_AABBNode* pAABBTree = csrAABBTreeFromMesh(&pMDL->m_pModel[i].m_pMesh[j]);

                // succeeded?
                if (!pAABBTree)
                {
                    // realloc to the previous size, thus the latest added item will be freed
                    if (transparent)
                        pScene->m_pTransparentItem =
                                (CSR_SceneItem*)csrMemoryAlloc(pItem,
                                                               sizeof(CSR_SceneItem),
                                                               pScene->m_TransparentItemCount);
                    else
                        pScene->m_pItem = (CSR_SceneItem*)csrMemoryAlloc(pItem,
                                                                         sizeof(CSR_SceneItem),
                                                                         pScene->m_ItemCount);

                    return 0;
                }

                // copy the tree content
                memcpy(&pItem[index].m_pAABBTree[i], pAABBTree, sizeof(CSR_AABBNode));

                // release the source tree
                csrAABBTreeRelease(pAABBTree);
            }
    }

    // do add a transparent item?
    if (transparent)
    {
        // add item to the transparent item list
        pScene->m_pTransparentItem = pItem;
        ++pScene->m_TransparentItemCount;
    }
    else
    {
        // add item to the normal item list
        pScene->m_pItem = pItem;
        ++pScene->m_ItemCount;
    }

    return 1;
}
//---------------------------------------------------------------------------
/* FIXME implement delete functions based on the following example
int* remove_element(int* array, int sizeOfArray, int indexToRemove)
{
    int* temp = malloc((sizeOfArray - 1) * sizeof(int)); // allocate an array with a size 1 less than the current one

    if (indexToRemove != 0)
        memcpy(temp, array, (indexToRemove - 1) * sizeof(int)); // copy everything BEFORE the index

    if (indexToRemove != (sizeOfArray - 1))
        memcpy(temp+indexToRemove, array+indexToRemove+1, (sizeOfArray - indexToRemove - 1) * sizeof(int)); // copy everything AFTER the index

    free (array);
    return temp;
}
*/
//---------------------------------------------------------------------------
void csrSceneBegin(const CSR_Color* pColor)
{
    // no scene background color?
    if (!pColor)
        return;

    // clear scene background and depth buffer
    glClearColor(pColor->m_R, pColor->m_G, pColor->m_B, pColor->m_A);
    glClearDepthf(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // configure the OpenGL depth testing for the scene
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);
}
//---------------------------------------------------------------------------
void csrSceneEnd(void)
{}
//---------------------------------------------------------------------------
void csrSceneDrawVertexBuffer(const CSR_VertexBuffer* pVB,
                              const CSR_Shader*       pShader,
                              const CSR_MatrixList*   pMatrixList)
{
    GLvoid* pCoords;
    GLvoid* pNormals;
    GLvoid* pTexCoords;
    GLvoid* pColors;
    size_t  i;
    size_t  offset;
    size_t  vertexCount;

    // no vertex buffer to draw?
    if (!pVB)
        return;

    // no shader?
    if (!pShader)
        return;

    // check if vertex buffer is empty, skip to next if yes
    if (!pVB->m_Count || !pVB->m_Format.m_Stride)
        return;

    // configure the culling
    switch (pVB->m_Culling.m_Type)
    {
        case CSR_CT_None:  glDisable(GL_CULL_FACE); glCullFace(GL_NONE);           break;
        case CSR_CT_Front: glEnable(GL_CULL_FACE);  glCullFace(GL_FRONT);          break;
        case CSR_CT_Back:  glEnable(GL_CULL_FACE);  glCullFace(GL_BACK);           break;
        case CSR_CT_Both:  glEnable(GL_CULL_FACE);  glCullFace(GL_FRONT_AND_BACK); break;
        default:           glDisable(GL_CULL_FACE); glCullFace(GL_NONE);           break;
    }

    // configure the culling face
    switch (pVB->m_Culling.m_Face)
    {
        case CSR_CF_CW:  glFrontFace(GL_CW);  break;
        case CSR_CF_CCW: glFrontFace(GL_CCW); break;
    }

    // configure the alpha blending
    if (pVB->m_Material.m_Transparent)
    {
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
        glDisable(GL_BLEND);

    // configure the wireframe mode
    #ifndef CSR_OPENGL_2_ONLY
        if (pVB->m_Material.m_Wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    #endif

    // enable vertex slot
    glEnableVertexAttribArray(pShader->m_VertexSlot);

    // enable normal slot
    if (pVB->m_Format.m_HasNormal)
        glEnableVertexAttribArray(pShader->m_NormalSlot);

    // enable texture slot
    if (pVB->m_Format.m_HasTexCoords)
        glEnableVertexAttribArray(pShader->m_TexCoordSlot);

    // enable color slot
    if (pVB->m_Format.m_HasPerVertexColor)
        glEnableVertexAttribArray(pShader->m_ColorSlot);

    offset = 0;

    // send vertices to shader
    pCoords = &pVB->m_pData[offset];
    glVertexAttribPointer(pShader->m_VertexSlot,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          pVB->m_Format.m_Stride * sizeof(float),
                          pCoords);

    offset += 3;

    // vertices have normals?
    if (pVB->m_Format.m_HasNormal)
    {
        // send normals to shader
        pNormals = &pVB->m_pData[offset];
        glVertexAttribPointer(pShader->m_TexCoordSlot,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              pVB->m_Format.m_Stride * sizeof(float),
                              pNormals);

        offset += 3;
    }

    // vertices have UV texture coordinates?
    if (pVB->m_Format.m_HasTexCoords)
    {
        // send textures to shader
        pTexCoords = &pVB->m_pData[offset];
        glVertexAttribPointer(pShader->m_TexCoordSlot,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              pVB->m_Format.m_Stride * sizeof(float),
                              pTexCoords);

        offset += 2;
    }

    // vertices have per-vertex color?
    if (pVB->m_Format.m_HasPerVertexColor)
    {
        // send colors to shader
        pColors = &pVB->m_pData[offset];
        glVertexAttribPointer(pShader->m_ColorSlot,
                              4,
                              GL_FLOAT,
                              GL_FALSE,
                              pVB->m_Format.m_Stride * sizeof(float),
                              pColors);
    }
    else
    if (pShader->m_ColorSlot != -1)
    {
        // get the color component values
        const float r = (float)((pVB->m_Material.m_Color >> 24) & 0xFF) / 255.0f;
        const float g = (float)((pVB->m_Material.m_Color >> 16) & 0xFF) / 255.0f;
        const float b = (float)((pVB->m_Material.m_Color >> 8)  & 0xFF) / 255.0f;
        const float a = (float) (pVB->m_Material.m_Color        & 0xFF) / 255.0f;

        // connect the vertex color to the shader
        glVertexAttrib4f(pShader->m_ColorSlot, r, g, b, a);
    }

    // calculate the vertex count
    vertexCount = pVB->m_Count / pVB->m_Format.m_Stride;

    // do draw the vertex buffer several times?
    if (pMatrixList && pMatrixList->m_Count)
        // yes, iterate through each matrix to use to draw the vertex buffer
        for (i = 0; i < pMatrixList->m_Count; ++i)
        {
            // get the model matrix slot from shader
            const GLint slot = glGetUniformLocation(pShader->m_ProgramID, "csr_uModel");

            // found it?
            if (slot < 0)
                continue;

            // connect the model matrix to the shader
            glUniformMatrix4fv(slot, 1, 0, &pMatrixList->m_pMatrix[i].m_Table[0][0]);

            // draw the next buffer
            switch (pVB->m_Format.m_Type)
            {
                case CSR_VT_Triangles:     glDrawArrays(GL_TRIANGLES,      0, vertexCount); break;
                case CSR_VT_TriangleStrip: glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount); break;
                case CSR_VT_TriangleFan:   glDrawArrays(GL_TRIANGLE_FAN,   0, vertexCount); break;
            }
        }
    else
        // no, draw the buffer
        switch (pVB->m_Format.m_Type)
        {
            case CSR_VT_Triangles:     glDrawArrays(GL_TRIANGLES,      0, vertexCount); break;
            case CSR_VT_TriangleStrip: glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount); break;
            case CSR_VT_TriangleFan:   glDrawArrays(GL_TRIANGLE_FAN,   0, vertexCount); break;
        }

    // disable vertices slots from shader
    glDisableVertexAttribArray(pShader->m_VertexSlot);

    // disable normal slot
    if (pVB->m_Format.m_HasNormal)
        glDisableVertexAttribArray(pShader->m_NormalSlot);

    // disable texture slot
    if (pVB->m_Format.m_HasTexCoords)
        glDisableVertexAttribArray(pShader->m_TexCoordSlot);

    // disable color slot
    if (pVB->m_Format.m_HasPerVertexColor)
        glDisableVertexAttribArray(pShader->m_ColorSlot);
}
//---------------------------------------------------------------------------
void csrSceneDrawMesh(const CSR_Mesh*       pMesh,
                      const CSR_Shader*     pShader,
                      const CSR_MatrixList* pMatrixList)
{
    size_t i;

    // no mesh to draw?
    if (!pMesh)
        return;

    // no shader?
    if (!pShader)
        return;

    // enable the shader to use for drawing
    csrShaderEnable(pShader);

    // iterate through the vertex buffers composing the mesh to draw
    for (i = 0; i < pMesh->m_Count; ++i)
    {
        // vertices have UV texture coordinates?
        if (pMesh->m_pVB[i].m_Format.m_HasTexCoords)
        {
            // a texture is defined for this mesh?
            if (pMesh->m_Shader.m_TextureID != M_CSR_Error_Code)
            {
                // select the texture sampler to use (GL_TEXTURE0 for normal textures)
                glActiveTexture(GL_TEXTURE0);
                glUniform1i(pShader->m_TextureSlot, GL_TEXTURE0);

                // bind the texure to use
                glBindTexture(GL_TEXTURE_2D, pMesh->m_Shader.m_TextureID);
            }

            // a bump map is defined for this mesh?
            if (pMesh->m_Shader.m_BumpMapID != M_CSR_Error_Code)
            {
                // select the texture sampler to use (GL_TEXTURE1 for bump map textures)
                glActiveTexture(GL_TEXTURE1);
                glUniform1i(pShader->m_BumpMapSlot, GL_TEXTURE1);

                // bind the texure to use
                glBindTexture(GL_TEXTURE_2D, pMesh->m_Shader.m_BumpMapID);
            }
        }

        // draw the next mesh vertex buffer
        csrSceneDrawVertexBuffer(&pMesh->m_pVB[i], pShader, pMatrixList);
    }
}
//---------------------------------------------------------------------------
void csrSceneDrawModel(const CSR_Model*      pModel,
                             size_t          index,
                       const CSR_Shader*     pShader,
                       const CSR_MatrixList* pMatrixList)
{
    // no model to draw?
    if (!pModel)
        return;

    // draw the model mesh
    csrSceneDrawMesh(&pModel->m_pMesh[index % pModel->m_MeshCount], pShader, pMatrixList);
}
//---------------------------------------------------------------------------
void csrSceneDrawMDL(const CSR_MDL*        pMDL,
                     const CSR_Shader*     pShader,
                     const CSR_MatrixList* pMatrixList,
                           size_t          textureIndex,
                           size_t          modelIndex,
                           size_t          meshIndex)
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
            pMDL->m_pTexture[textureIndex].m_TextureID != M_CSR_Error_Code)
        {
            // select the texture sampler to use (GL_TEXTURE0 for normal textures)
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(pShader->m_TextureSlot, GL_TEXTURE0);

            // bind the texure to use
            glBindTexture(GL_TEXTURE_2D, pMDL->m_pTexture[textureIndex].m_TextureID);
        }

    // draw the model mesh
    csrSceneDrawMesh(pMesh, pShader, pMatrixList);
}
//---------------------------------------------------------------------------
void csrSceneDraw(const CSR_Scene* pScene)
{
    size_t i;
    GLint  slot;

    // no scene to draw?
    if (!pScene)
        return;

    // begin the scene drawing
    #ifndef CSR_OPENGL_2_ONLY
        if (pScene->m_pMSAA)
            csrMSAASceneBegin(&pScene->m_Color, pScene->m_pMSAA);
        else
    #endif
            csrSceneBegin(&pScene->m_Color);

    // todo -cImprovement -oJean: the models are reloaded on the GPU every time they will be drawn.
    //                            This is acceptable as long as the scenes are small, but this is a
    //                            huge time and resource wasting when the scene is large. A better
    //                            solution would be to group each similar models, load the mesh once
    //                            on the GPU, then only change the model matrix to draw them

    // first draw the standard models
    for (i = 0; i < pScene->m_ItemCount; ++i)
        csrSceneItemDraw(pScene, &pScene->m_pItem[i]);

    // then draw the transparent models
    for (i = 0; i < pScene->m_TransparentItemCount; ++i)
        csrSceneItemDraw(pScene, &pScene->m_pTransparentItem[i]);

    // end the scene drawing
    #ifndef CSR_OPENGL_2_ONLY
        if (pScene->m_pMSAA)
            csrMSAASceneEnd(pScene->m_pMSAA);
        else
    #endif
            csrSceneEnd();
}
//---------------------------------------------------------------------------
