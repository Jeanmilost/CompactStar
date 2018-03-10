/****************************************************************************
 * ==> CSR_Scene -----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a scene in which the rendering will   *
 *               be performed                                               *
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

#include "CSR_Scene.h"

// std
#include <stdlib.h>

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
    // no scene item to release?
    if (!pSceneItem)
        return;

    // free the scene item
    free(pSceneItem);
}
//---------------------------------------------------------------------------
void csrSceneItemInit(CSR_SceneItem* pSceneItem)
{
    // no scene item to initialize?
    if (!pSceneItem)
        return;

    pSceneItem->m_pItem        = 0;
    pSceneItem->m_Type         = CSR_IT_Model;
    pSceneItem->m_pShader      = 0;
    pSceneItem->m_TextureIndex = 0;
    pSceneItem->m_ModelIndex   = 0;
    pSceneItem->m_MeshIndex    = 0;
    pSceneItem->m_Visible      = 1;

    // set the default item matrix to identity
    csrMat4Identity(&pSceneItem->m_Matrix);
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

    // get the model matrix slot from shader
    slot = glGetUniformLocation(pSceneItem->m_pShader->m_ProgramID, "csr_uModel");

    // connect model matrix to shader
    if (slot >= 0)
        glUniformMatrix4fv(slot, 1, 0, &pSceneItem->m_Matrix.m_Table[0][0]);

    // draw the model
    switch (pSceneItem->m_Type)
    {
        case CSR_IT_Mesh:
            csrSceneDrawMesh((const CSR_Mesh*)pSceneItem->m_pItem,
                                              pSceneItem->m_pShader);
            break;

        case CSR_IT_Model:
            csrSceneDrawModel((const CSR_Model*)pSceneItem->m_pItem,
                                                pSceneItem->m_ModelIndex,
                                                pSceneItem->m_pShader);
            break;

        case CSR_IT_MDL:
            csrSceneDrawMDL((const CSR_MDL*)pSceneItem->m_pItem,
                                            pSceneItem->m_pShader,
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
        // free the scene items
        free(pScene->m_pItem);

    // do free the transparent items content?
    if (pScene->m_pTransparentItem)
        // free the scene transparent items
        free(pScene->m_pItem);

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
int csrSceneAddMesh(CSR_Scene* pScene, CSR_Mesh* pMesh, CSR_Shader* pShader, int transparent)
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

        // update the transparent item data
        pScene->m_pTransparentItem = pItem;
        ++pScene->m_TransparentItemCount;
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

        // update the scene item data
        pScene->m_pItem = pItem;
        ++pScene->m_ItemCount;
    }

    // initialize the newly created item with the default values
    csrSceneItemInit(&pItem[index]);

    // configure the item
    pItem[index].m_pItem   = pMesh;
    pItem[index].m_Type    = CSR_IT_Mesh;
    pItem[index].m_pShader = pShader;

    return 1;
}
//---------------------------------------------------------------------------
int csrSceneAddModel(CSR_Scene* pScene, CSR_Model* pModel, CSR_Shader* pShader, int transparent)
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
                                               pScene->m_TransparentItemCount);

        // succeeded?
        if (!pItem)
            return 0;

        // get the item index to update
        index = pScene->m_TransparentItemCount;

        // update the transparent item data
        pScene->m_pTransparentItem = pItem;
        ++pScene->m_TransparentItemCount;
    }
    else
    {
        // add a new item to the scene items
        pItem = (CSR_SceneItem*)csrMemoryAlloc(pScene->m_pItem,
                                               sizeof(CSR_SceneItem),
                                               pScene->m_ItemCount);

        // succeeded?
        if (!pItem)
            return 0;

        // get the scene item index to update
        index = pScene->m_ItemCount;

        // update the scene item data
        pScene->m_pItem = pItem;
        ++pScene->m_ItemCount;
    }

    // initialize the newly created item with the default values
    csrSceneItemInit(&pItem[index]);

    // configure the item
    pItem[index].m_pItem   = pModel;
    pItem[index].m_Type    = CSR_IT_Model;
    pItem[index].m_pShader = pShader;

    return 1;
}
//---------------------------------------------------------------------------
int csrSceneAddMDL(CSR_Scene* pScene, CSR_MDL* pMDL, CSR_Shader* pShader, int transparent)
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
                                               pScene->m_TransparentItemCount);

        // succeeded?
        if (!pItem)
            return 0;

        // get the item index to update
        index = pScene->m_TransparentItemCount;

        // update the transparent item data
        pScene->m_pTransparentItem = pItem;
        ++pScene->m_TransparentItemCount;
    }
    else
    {
        // add a new item to the scene items
        pItem = (CSR_SceneItem*)csrMemoryAlloc(pScene->m_pItem,
                                               sizeof(CSR_SceneItem),
                                               pScene->m_ItemCount);

        // succeeded?
        if (!pItem)
            return 0;

        // get the scene item index to update
        index = pScene->m_ItemCount;

        // update the scene item data
        pScene->m_pItem = pItem;
        ++pScene->m_ItemCount;
    }

    // initialize the newly created item with the default values
    csrSceneItemInit(&pItem[index]);

    // configure the item
    pItem[index].m_pItem   = pMDL;
    pItem[index].m_Type    = CSR_IT_MDL;
    pItem[index].m_pShader = pShader;

    return 1;
}
//---------------------------------------------------------------------------
void csrSceneDraw(const CSR_Scene* pScene)
{
    size_t i;

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
