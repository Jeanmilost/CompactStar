/****************************************************************************
 * ==> CSR_Scene -----------------------------------------------------------*
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

#include "CSR_Scene.h"

// std
#include <stdlib.h>

//---------------------------------------------------------------------------
// Scene context functions
//---------------------------------------------------------------------------
void csrSceneContextInit(CSR_SceneContext* pContext)
{
    // no context to initialize?
    if (!pContext)
        return;

    // initialize the context
    pContext->m_Handle             = 0;
    pContext->m_fOnSceneBegin      = 0;
    pContext->m_fOnSceneEnd        = 0;
    pContext->m_fOnGetShader       = 0;
    pContext->m_fOnGetModelIndex   = 0;
    pContext->m_fOnGetMDLIndex     = 0;
}
//---------------------------------------------------------------------------
// Scene item private functions
//---------------------------------------------------------------------------
CSR_SceneItem* csrSceneItemDeleteModelFrom(CSR_SceneItem* pItem, size_t index, size_t count)
{
    CSR_SceneItem* pNewItem;

    // no item list to delete from?
    if (!pItem)
        return 0;

    // was the last model in the scene?
    if (count == 1)
        // don't create a new scene item list
        return 0;

    // create a scene item list one item smaller than the existing one
    pNewItem = (CSR_SceneItem*)csrMemoryAlloc(0, sizeof(CSR_SceneItem), count - 1);

    // succeeded?
    if (!pNewItem)
        return 0;

    // copy all the remaining items in the new list
    if (!index)
        memcpy(pNewItem, pItem + 1, (count - 1) * sizeof(CSR_SceneItem));
    else
    if (index == (count - 1))
        memcpy(pNewItem, pItem, (count - 1) * sizeof(CSR_SceneItem));
    else
    {
        memcpy(pNewItem,         pItem,             sizeof(CSR_SceneItem) *          index);
        memcpy(pNewItem + index, pItem + index + 1, sizeof(CSR_SceneItem) * (count - index - 1));
    }

    // release the item content
    csrSceneItemContentRelease(&pItem[index]);

    return pNewItem;
}
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
void csrSceneItemContentRelease(CSR_SceneItem* pSceneItem)
{
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
    {
        size_t i;

        // release all the tree content
        for (i = 0; i < pSceneItem->m_AABBTreeCount; ++i)
        {
            // get the AABB tree root node
            CSR_AABBNode* pNode = &pSceneItem->m_pAABBTree[i];

            // release all children on left side
            if (pNode->m_pLeft)
                csrAABBTreeNodeRelease(pNode->m_pLeft);

            // release all children on right side
            if (pNode->m_pRight)
                csrAABBTreeNodeRelease(pNode->m_pRight);

            // delete node content
            csrAABBTreeNodeContentRelease(pNode);
        }

        // free the tree container
        free(pSceneItem->m_pAABBTree);
    }

    // release the matrix array
    csrArrayRelease(pSceneItem->m_pMatrixArray);

    // NOTE don't release the shader, as it's just linked with the item, not owned
}
//---------------------------------------------------------------------------
void csrSceneItemInit(CSR_SceneItem* pSceneItem)
{
    // no scene item to initialize?
    if (!pSceneItem)
        return;

    pSceneItem->m_pModel        = 0;
    pSceneItem->m_Type          = CSR_MT_Model;
    pSceneItem->m_pMatrixArray  = 0;
    pSceneItem->m_pAABBTree     = 0;
    pSceneItem->m_AABBTreeCount = 0;
}
//---------------------------------------------------------------------------
void csrSceneItemDraw(const CSR_Scene*        pScene,
                      const CSR_SceneContext* pContext,
                      const CSR_SceneItem*    pItem)
{
    GLint       slot;
    CSR_Shader* pShader;

    // validate the inputs
    if (!pScene || !pContext || !pItem)
        return;

    pShader = 0;

    // get the shader to use with the model
    if (pContext->m_fOnGetShader)
        pShader = pContext->m_fOnGetShader(pItem->m_pModel, pItem->m_Type);

    // found one?
    if (!pShader)
        return;

    // enable the item shader
    csrShaderEnable(pShader);

    // get the view matrix slot from shader
    slot = glGetUniformLocation(pShader->m_ProgramID, "csr_uView");

    // connect view matrix to shader
    if (slot >= 0)
        glUniformMatrix4fv(slot, 1, 0, &pScene->m_Matrix.m_Table[0][0]);

    // draw the model
    switch (pItem->m_Type)
    {
        case CSR_MT_Mesh:
            // draw the mesh
            csrDrawMesh((const CSR_Mesh*)pItem->m_pModel,
                                         pShader,
                                         pItem->m_pMatrixArray);

            break;

        case CSR_MT_Model:
        {
            size_t index = 0;

            // notify the caller that the model is about to be drawn
            if (pContext->m_fOnGetModelIndex)
                pContext->m_fOnGetModelIndex((const CSR_Model*)pItem->m_pModel, &index);

            // draw the model
            csrDrawModel((const CSR_Model*)pItem->m_pModel,
                                           index,
                                           pShader,
                                           pItem->m_pMatrixArray);

            break;
        }

        case CSR_MT_MDL:
        {
            size_t textureIndex = 0;
            size_t modelIndex   = 0;
            size_t meshIndex    = 0;

            // notify the caller that the MDL model is about to be drawn
            if (pContext->m_fOnGetMDLIndex)
                pContext->m_fOnGetMDLIndex((const CSR_MDL*)pItem->m_pModel,
                                                          &textureIndex,
                                                          &modelIndex,
                                                          &meshIndex);

            // draw the MDL model
            csrDrawMDL((const CSR_MDL*)pItem->m_pModel,
                                       pShader,
                                       pItem->m_pMatrixArray,
                                       textureIndex,
                                       modelIndex,
                                       meshIndex);

            break;
        }
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
            csrSceneItemContentRelease(&pScene->m_pItem[i]);

        // free the scene items
        free(pScene->m_pItem);
    }

    // do free the transparent items content?
    if (pScene->m_pTransparentItem)
    {
        // iterate through each scene transparent item to release, and release each of them
        for (i = 0; i < pScene->m_TransparentItemCount; ++i)
            csrSceneItemContentRelease(&pScene->m_pTransparentItem[i]);

        // free the scene transparent items
        free(pScene->m_pTransparentItem);
    }

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
}
//---------------------------------------------------------------------------
int csrSceneAddMesh(CSR_Scene* pScene, CSR_Mesh* pMesh, int transparent, int aabb)
{
    CSR_SceneItem* pItem;
    int            index;

    // validate the inputs
    if (!pScene || !pMesh)
        return 0;

    // is mesh already added in the scene?
    if (csrSceneGetItem(pScene, pMesh))
        return 1;

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
    pItem[index].m_pModel = pMesh;
    pItem[index].m_Type   = CSR_MT_Mesh;

    // generate the aligned-axis bounding box tree for this mesh
    if (aabb)
    {
        pItem[index].m_AABBTreeCount = 1;
        pItem[index].m_pAABBTree     = csrAABBTreeFromMesh(pMesh);

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
int csrSceneAddModel(CSR_Scene* pScene, CSR_Model* pModel, int transparent, int aabb)
{
    CSR_SceneItem* pItem;
    int            index;

    // validate the inputs
    if (!pScene || !pModel)
        return 0;

    // is model already added in the scene?
    if (csrSceneGetItem(pScene, pModel))
        return 1;

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
    pItem[index].m_pModel = pModel;
    pItem[index].m_Type   = CSR_MT_Model;

    // generate the aligned-axis bounding box tree for this model
    if (aabb)
    {
        size_t i;

        // reserve memory for all the AABB trees to create
        pItem[index].m_AABBTreeCount = pModel->m_MeshCount;
        pItem[index].m_pAABBTree     = (CSR_AABBNode*)csrMemoryAlloc(0,
                                                                     sizeof(CSR_AABBNode),
                                                                     pItem[index].m_AABBTreeCount);

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
            csrAABBTreeNodeRelease(pAABBTree);
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
int csrSceneAddMDL(CSR_Scene* pScene, CSR_MDL* pMDL, int transparent, int aabb)
{
    CSR_SceneItem* pItem;
    int            index;

    // validate the inputs
    if (!pScene || !pMDL)
        return 0;

    // is model already added in the scene?
    if (csrSceneGetItem(pScene, pMDL))
        return 1;

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
    pItem[index].m_pModel = pMDL;
    pItem[index].m_Type   = CSR_MT_MDL;

    // generate the aligned-axis bounding box tree for this model
    if (aabb)
    {
        size_t i;
        size_t j;

        // reserve memory for all the AABB trees to create
        pItem[index].m_AABBTreeCount = pMDL->m_ModelCount * pMDL->m_pModel->m_MeshCount;
        pItem[index].m_pAABBTree     = (CSR_AABBNode*)csrMemoryAlloc(0,
                                                                     sizeof(CSR_AABBNode),
                                                                     pItem[index].m_AABBTreeCount);

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
                csrAABBTreeNodeRelease(pAABBTree);
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
int csrSceneAddModelMatrix(CSR_Scene* pScene, const void* pModel, CSR_Matrix4* pMatrix)
{
    size_t         i;
    CSR_SceneItem* pSceneItem;

    // validate inputs
    if (!pScene || !pModel || !pMatrix)
        return 0;

    // get the scene item matching with the model for which the matrix should be added
    pSceneItem = csrSceneGetItem(pScene, pModel);

    // found it?
    if (!pSceneItem)
        return 0;

    // do create a matrix array for the item?
    if (!pSceneItem->m_pMatrixArray)
    {
        // create a new matrix array
        pSceneItem->m_pMatrixArray = (CSR_Array*)csrMemoryAlloc(0, sizeof(CSR_Array), 1);

        // succeeded?
        if (!pSceneItem->m_pMatrixArray)
            return 0;
    }

    // add the matrix to the array
    csrArrayAddUnique(pMatrix, pSceneItem->m_pMatrixArray, 0);

    return 1;
}
//---------------------------------------------------------------------------
CSR_SceneItem* csrSceneGetItem(const CSR_Scene* pScene, const void* pKey)
{
    size_t i;
    size_t j;

    // validate inputs
    if (!pScene || !pKey)
        return 0;

    // first search in the standard models
    for (i = 0; i < pScene->m_ItemCount; ++i)
    {
        // found a matching model?
        if (pScene->m_pItem[i].m_pModel == pKey)
            return &pScene->m_pItem[i];

        // check also if the key is a known matrix
        if (pScene->m_pItem[i].m_pMatrixArray)
            for (j = 0; j < pScene->m_pItem[i].m_pMatrixArray->m_Count; ++j)
                if (&pScene->m_pItem[i].m_pMatrixArray->m_pItem[j].m_pData == pKey)
                    return &pScene->m_pItem[i];
    }

    // then search in the transparent models
    for (i = 0; i < pScene->m_TransparentItemCount; ++i)
    {
        // found a matching model?
        if (pScene->m_pTransparentItem[i].m_pModel == pKey)
            return &pScene->m_pTransparentItem[i];

        // check also if the key is a known matrix
        if (pScene->m_pTransparentItem[i].m_pMatrixArray)
            for (j = 0; j < pScene->m_pTransparentItem[i].m_pMatrixArray->m_Count; ++j)
                if (&pScene->m_pTransparentItem[i].m_pMatrixArray->m_pItem[j].m_pData == pKey)
                    return &pScene->m_pTransparentItem[i];
    }

    // not found
    return 0;
}
//---------------------------------------------------------------------------
void csrSceneDeleteFrom(CSR_Scene* pScene, const void* pKey)
{
    size_t          i;
    size_t          j;
    CSR_SceneItem*  pSceneItem;

    // validate inputs
    if (!pScene || !pKey)
        return;

    // first search in the standard models
    for (i = 0; i < pScene->m_ItemCount; ++i)
    {
        // found a matching model?
        if (pScene->m_pItem[i].m_pModel == pKey)
        {
            // delete the item from the list
            pSceneItem = csrSceneItemDeleteModelFrom(pScene->m_pItem, i, pScene->m_ItemCount);

            // update the scene content
            free(pScene->m_pItem);
            pScene->m_pItem = pSceneItem;
            --pScene->m_ItemCount;

            return;
        }

        // check also if the key is a known matrix
        if (pScene->m_pItem[i].m_pMatrixArray)
            for (j = 0; j < pScene->m_pItem[i].m_pMatrixArray->m_Count; ++j)
                if (&pScene->m_pItem[i].m_pMatrixArray->m_pItem[j].m_pData == pKey)
                {
                    // delete the matrix
                    csrArrayDeleteAt(j, pScene->m_pItem[i].m_pMatrixArray);
                    return;
                }
    }

    // then search in the transparent models
    for (i = 0; i < pScene->m_TransparentItemCount; ++i)
    {
        // found a matching model?
        if (pScene->m_pTransparentItem[i].m_pModel == pKey)
        {
            // delete the item from the list
            pSceneItem = csrSceneItemDeleteModelFrom(pScene->m_pTransparentItem,
                                                     i,
                                                     pScene->m_TransparentItemCount);

            // update the scene content
            free(pScene->m_pTransparentItem);
            pScene->m_pTransparentItem = pSceneItem;
            --pScene->m_TransparentItemCount;

            return;
        }

        // check also if the key is a known matrix
        if (pScene->m_pTransparentItem[i].m_pMatrixArray)
            for (j = 0; j < pScene->m_pTransparentItem[i].m_pMatrixArray->m_Count; ++j)
                if (&pScene->m_pTransparentItem[i].m_pMatrixArray->m_pItem[j].m_pData == pKey)
                {
                    // delete the matrix
                    csrArrayDeleteAt(j, pScene->m_pItem[i].m_pMatrixArray);
                    return;
                }
    }
}
//---------------------------------------------------------------------------
void csrSceneDraw(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    size_t i;
    GLint  slot;

    // no scene to draw?
    if (!pScene)
        return;

    // no scene context?
    if (!pContext)
        return;

    // begin the scene drawing
    if (pContext->m_fOnSceneBegin)
        pContext->m_fOnSceneBegin(pScene, pContext);
    else
        csrDrawBegin(&pScene->m_Color);

    // first draw the standard models
    for (i = 0; i < pScene->m_ItemCount; ++i)
        csrSceneItemDraw(pScene,
                         pContext,
                        &pScene->m_pItem[i]);

    // then draw the transparent models
    for (i = 0; i < pScene->m_TransparentItemCount; ++i)
        csrSceneItemDraw(pScene,
                         pContext,
                        &pScene->m_pTransparentItem[i]);

    // end the scene drawing
    if (pContext->m_fOnSceneEnd)
        pContext->m_fOnSceneEnd(pScene, pContext);
    else
        csrDrawEnd();
}
//---------------------------------------------------------------------------
void csrSceneDetectCollision(const CSR_Scene*          pScene,
                             const CSR_Ray3*           pRay,
                                   CSR_SceneCollision* pR)
{
    size_t      i;
    size_t      j;
    size_t      k;
    float       determinant;
    CSR_Vector3 rayPos;
    CSR_Vector3 rayDir;
    CSR_Vector3 rayDirN;
    CSR_Matrix4 invertModel;
    CSR_Ray3    rayModel;

    // validate the inputs
    if (!pScene || !pRay || !pR)
        return;

    // initialize the result
    pR->m_Collision        = 0;
    pR->m_SlidingPlane.m_A = 0;
    pR->m_SlidingPlane.m_B = 0;
    pR->m_SlidingPlane.m_C = 0;
    pR->m_SlidingPlane.m_D = 0;

    // iterate through the scene items
    for (i = 0; i < pScene->m_ItemCount; ++i)
    {
        // do detect collision on this model?
        if (!pScene->m_pItem[i].m_pMatrixArray || !pScene->m_pItem[i].m_AABBTreeCount)
            continue;

        // iterate through each model position
        for (j = 0; j < pScene->m_pItem[i].m_pMatrixArray->m_Count; ++j)
        {
            // transform the ray to match with the model position
            csrMat4Inverse((CSR_Matrix4*)pScene->m_pItem[i].m_pMatrixArray[j].m_pItem->m_pData,
                           &invertModel,
                           &determinant);
            csrMat4ApplyToVector(&invertModel, &pRay->m_Pos, &rayPos);
            csrMat4ApplyToNormal(&invertModel, &pRay->m_Dir, &rayDir);
            csrVec3Normalize(&rayDir, &rayDirN);

            // get the transformed ray
            csrRay3FromPointDir(&rayPos, &rayDirN, &rayModel);

            // iterate through AABB trees to check
            for (k = 0; k < pScene->m_pItem[i].m_AABBTreeCount; ++k)
                // check for collision
                pR->m_Collision |= csrAABBTreeResolve(&rayModel,
                                                      &pScene->m_pItem[i].m_pAABBTree[k],
                                                       1, // thus the polygon buffer will not be reseted
                                                      &pR->m_Polygons);
        }
    }
}
//---------------------------------------------------------------------------
void csrSceneTouchPosToViewportPos(const CSR_Vector2* pTouchPos,
                                   const CSR_Rect*    pTouchRect,
                                   const CSR_Rect*    pViewportRect,
                                         CSR_Vector3* pViewportPos)
{
    float touchWidth;
    float touchHeight;
    float viewportWidth;
    float viewportHeight;

    // validate the inputs
    if (!pTouchPos || !pTouchRect || !pViewportRect || !pViewportPos)
        return;

    // calculate the touch area width and height
    touchWidth  = pTouchRect->m_Max.m_X - pTouchRect->m_Min.m_X;
    touchHeight = pTouchRect->m_Max.m_Y - pTouchRect->m_Min.m_Y;

    // invalid touch width or height?
    if (!touchWidth || !touchHeight)
    {
        pViewportPos->m_X = 0.0f;
        pViewportPos->m_Y = 0.0f;
        pViewportPos->m_Z = 0.0f;
        return;
    }

    viewportWidth  = pViewportRect->m_Max.m_X - pViewportRect->m_Min.m_X;
    viewportHeight = pViewportRect->m_Min.m_Y - pViewportRect->m_Max.m_Y;

    // convert touch position to viewport position
    pViewportPos->m_X = pViewportRect->m_Min.m_X + ((pTouchPos->m_X * viewportWidth)  / touchWidth);
    pViewportPos->m_Y = pViewportRect->m_Min.m_Y - ((pTouchPos->m_Y * viewportHeight) / touchHeight);
    pViewportPos->m_Z = 0.0f;
}
//---------------------------------------------------------------------------
void csrSceneGetTouchRay(const CSR_Vector2* pTouchPos,
                         const CSR_Rect*    pTouchRect,
                         const CSR_Matrix4* pProjectionMatrix,
                         const CSR_Matrix4* pViewMatrix,
                               CSR_Ray3*    pTouchRay)
{
    CSR_Rect viewportRect;

    // validate the inputs
    if (!pTouchPos || !pTouchRect || !pProjectionMatrix || !pViewMatrix || !pTouchRay)
        return;

    // get the viewport rectangle
    viewportRect.m_Min.m_X = -1.0f;
    viewportRect.m_Min.m_Y =  1.0f;
    viewportRect.m_Max.m_X =  1.0f;
    viewportRect.m_Max.m_Y = -1.0f;

    // get the ray in the touch coordinate system
    csrSceneTouchPosToViewportPos(pTouchPos, pTouchRect, &viewportRect, &pTouchRay->m_Pos);
    pTouchRay->m_Dir.m_X =  pTouchRay->m_Pos.m_X;
    pTouchRay->m_Dir.m_Y =  pTouchRay->m_Pos.m_Y;
    pTouchRay->m_Dir.m_Z = -1.0f;

    // put the ray in the viewport coordinates
    csrMat4Unproject(pProjectionMatrix, pViewMatrix, pTouchRay);
}
//---------------------------------------------------------------------------
