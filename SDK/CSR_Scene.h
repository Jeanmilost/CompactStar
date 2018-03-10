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

#ifndef CSR_SceneH
#define CSR_SceneH

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Model.h"
#include "CSR_Shader.h"
#include "CSR_Renderer.h"

//---------------------------------------------------------------------------
// Enumerators
//---------------------------------------------------------------------------

/**
* Scene item type
*/
typedef enum
{
    CSR_IT_Mesh,
    CSR_IT_Model,
    CSR_IT_MDL
} CSR_ESceneItemType;

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

/**
* Scene item
*/
typedef struct
{
    void*              m_pItem;
    CSR_ESceneItemType m_Type;
    CSR_Matrix4        m_Matrix;
    CSR_Shader*        m_pShader;
    size_t             m_TextureIndex;
    size_t             m_ModelIndex;
    size_t             m_MeshIndex;
    int                m_Visible;
} CSR_SceneItem;

/**
* Scene
*/
typedef struct
{
    CSR_Color      m_Color;
    CSR_Matrix4    m_Matrix;
    CSR_SceneItem* m_pItem;
    size_t         m_ItemCount;
    CSR_SceneItem* m_pTransparentItem;
    size_t         m_TransparentItemCount;

    #ifndef CSR_OPENGL_2_ONLY
        CSR_MSAA*  m_pMSAA;
    #endif
} CSR_Scene;

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Scene item functions
        //-------------------------------------------------------------------

        /**
        * Creates a scene item
        *@return newly created scene item, 0 on error
        *@note The scene item must be released when no longer used, see csrSceneItemRelease()
        */
        CSR_SceneItem* csrSceneItemCreate(void);

        /**
        * Releases a scene item
        *@param[in, out] pSceneItem - scene item to release
        */
        void csrSceneItemRelease(CSR_SceneItem* pSceneItem);

        /**
        * Initializes a scene item structure
        *@param[in, out] pSceneItem - scene item to initialize
        */
        void csrSceneItemInit(CSR_SceneItem* pSceneItem);

        /**
        * Draws a scene item
        *@param pScene - scene at which the item belongs
        *@param pSceneItem - scene item to draw
        */
        void csrSceneItemDraw(const CSR_Scene* pScene, const CSR_SceneItem* pSceneItem);

        //-------------------------------------------------------------------
        // Scene functions
        //-------------------------------------------------------------------

        /**
        * Creates a scene
        *@return newly created scene, 0 on error
        *@note The scene must be released when no longer used, see csrSceneRelease()
        */
        CSR_Scene* csrSceneCreate(void);

        /**
        * Releases a scene
        *@param[in, out] pScene - scene to release
        */
        void csrSceneRelease(CSR_Scene* pScene);

        /**
        * Initializes a scene structure
        *@param[in, out] pScene - scene to initialize
        */
        void csrSceneInit(CSR_Scene* pScene);

        /**
        * Adds a mesh to a scene
        *@param pScene - scene in which the mesh will be added
        *@param pMesh - mesh to add
        *@param pShader - shader to use to draw the mesh
        *@param transparent - if 1, the mesh is transparent, if 0 the mesh is opaque
        *@return 1 on success, otherwise 0
        *@note Because the scene will keep a reference on the added mesh and shader, these objects
        *      should be released only after the scene is released
        */
        int csrSceneAddMesh(CSR_Scene* pScene, CSR_Mesh* pMesh, CSR_Shader* pShader, int transparent);

        /**
        * Adds a model to a scene
        *@param pScene - scene in which the model will be added
        *@param pModel- model to add
        *@param pShader - shader to use to draw the model
        *@param transparent - if 1, the model is transparent, if 0 the model is opaque
        *@return 1 on success, otherwise 0
        *@note Because the scene will keep a reference on the added mesh and shader, these objects
        *      should be released only after the scene is released
        */
        int csrSceneAddModel(CSR_Scene* pScene, CSR_Model* pModel, CSR_Shader* pShader, int transparent);

        /**
        * Adds a MDL model to a scene
        *@param pScene - scene in which the model will be added
        *@param pMDL - model to add
        *@param pShader - shader to use to draw the model
        *@param transparent - if 1, the model is transparent, if 0 the model is opaque
        *@return 1 on success, otherwise 0
        *@note Because the scene will keep a reference on the added mesh and shader, these objects
        *      should be released only after the scene is released
        */
        int csrSceneAddMDL(CSR_Scene* pScene, CSR_MDL* pMDL, CSR_Shader* pShader, int transparent);

        /**
        * Draws a scene
        *@param pScene - scene to draw
        */
        void csrSceneDraw(const CSR_Scene* pScene);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "CSR_Scene.c"
#endif

#endif