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

    pSceneItem->m_pModel  = 0;
    pSceneItem->m_Type    = CSR_IT_Model;
    pSceneItem->m_pShader = 0;
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

    pScene->m_pItem                = 0;
    pScene->m_ItemCount            = 0;
    pScene->m_pTransparentItem     = 0;
    pScene->m_TransparentItemCount = 0;

    #ifndef CSR_OPENGL_2_ONLY
        pScene->m_pMSAA = 0;
    #endif
}
//---------------------------------------------------------------------------
