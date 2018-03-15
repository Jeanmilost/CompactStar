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

#ifndef CSR_RendererH
#define CSR_RendererH

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Collision.h"
#include "CSR_Model.h"
#include "CSR_Shader.h"

//---------------------------------------------------------------------------
// Enumerators
//---------------------------------------------------------------------------

/**
* Model type
*/
typedef enum
{
    CSR_MT_Mesh,
    CSR_MT_Model,
    CSR_MT_MDL
} CSR_EModelType;

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

/**
* Multisampling antialiasing
*/
#ifndef CSR_OPENGL_2_ONLY
    typedef struct
    {
        CSR_Shader*       m_pShader;
        CSR_StaticBuffer* m_pStaticBuffer;
        GLuint            m_FrameBufferID;
        GLuint            m_RenderBufferID;
        GLuint            m_TextureBufferID;
        GLuint            m_TextureID;
        size_t            m_Width;
        size_t            m_Height;
        size_t            m_Factor;
    } CSR_MSAA;
#endif

/**
* Matrix item
*/
typedef struct
{
    CSR_Matrix4* m_pMatrix;
} CSR_MatrixItem;

/**
* Matrix list
*/
typedef struct
{
    CSR_MatrixItem* m_pItem;
    size_t          m_Count;
} CSR_MatrixList;

/**
* Scene context prototype
*/
typedef struct CSR_SceneContext CSR_SceneContext;

/**
* Scene item
*/
typedef struct
{
    void*           m_pModel;        // the model to draw
    CSR_EModelType  m_Type;          // model type (a simple mesh, a model or a complex MDL model)
    CSR_MatrixList* m_pMatrixList;   // items matrices sharing the same model, e.g. all the walls of a room
    CSR_AABBNode*   m_pAABBTree;     // aligned-axis bounding box trees owned by the model
    size_t          m_AABBTreeCount; // aligned-axis bounding box tree count
} CSR_SceneItem;

/**
* Scene
*/
typedef struct
{
    CSR_Color        m_Color;
    CSR_Matrix4      m_Matrix;
    CSR_SceneItem*   m_pItem;
    size_t           m_ItemCount;
    CSR_SceneItem*   m_pTransparentItem;
    size_t           m_TransparentItemCount;
} CSR_Scene;

//---------------------------------------------------------------------------
// Callbacks
//---------------------------------------------------------------------------

/**
* Called when scene begins
*@param pScene - scene to begin
*@param pContext - scene context
*/
typedef void (*CSR_fOnSceneBegin)(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

/**
* Called when scene ends
*@param pScene - scene to end
*@param pContext - scene context
*/
typedef void (*CSR_fOnSceneEnd)(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

/**
* Called when a shader should be get for a model
*@param pModel - model for which the shader shoudl be get
*@param type - model type
*@return shader to use to draw the model, 0 if no shader
*@note The model will not be drawn if no shader is returned
*/
typedef CSR_Shader* (*CSR_fOnGetShader)(const void* pModel, CSR_EModelType type);

/**
* Called when a model index should be get
*@param pModel - model for which the index should be get
*@param[in, out] pIndex - model index
*/
typedef void (*CSR_fOnGetModelIndex)(const CSR_Model* pModel, size_t* pIndex);

/**
* Called when the MDL model indexes should be get
*@param pMDL - MDL model for which the indexes should be get
*@param[in, out] textureIndex - texture index
*@param[in, out] modelIndex - model index
*@param[in, out] meshIndex - mesh index
*/
typedef void (*CSR_fOnGetMDLIndex)(const CSR_MDL* pMDL,
                                         size_t*  pTextureIndex,
                                         size_t*  pModelIndex,
                                         size_t*  pMeshIndex);

/**
* Called when a collision should be detected on a mesh
*@param pMesh - mesh on which the collision should be detected
*@param pAABBTree - aligned-axis bounding box tree matching with the mesh
*@param[in, out] pMatrix - model matrix used to draw the mesh, 0 is matrix is processed externally
*/
typedef void (*CSR_fOnDetectCollision)(const CSR_Mesh*     pMesh,
                                       const CSR_AABBNode* pAABBTree,
                                             CSR_Matrix4*  pMatrix);

//---------------------------------------------------------------------------
// Structures implementations
//---------------------------------------------------------------------------

/**
* Scene context
*/
struct CSR_SceneContext
{
    size_t                 m_Handle;
    CSR_fOnSceneBegin      m_fOnSceneBegin;
    CSR_fOnSceneEnd        m_fOnSceneEnd;
    CSR_fOnGetShader       m_fOnGetShader;
    CSR_fOnGetModelIndex   m_fOnGetModelIndex;
    CSR_fOnGetMDLIndex     m_fOnGetMDLIndex;
    CSR_fOnDetectCollision m_fOnDetectCollision;
};

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Multisampling antialiasing functions
        //-------------------------------------------------------------------

        /**
        * Creates a multisample antialiasing
        *@param width - viewport width on which the antialiasing will be applied
        *@param height - viewport height on which the antialiasing will be applied
        *@param factor - antialiasing factor to apply, may be 2x, 4x or 8x
        *@return newly created multisample antialiasing, 0 on error
        *@note The multisample antialiasing must be released when no longer used, see csrMSAARelease()
        */
        #ifndef CSR_OPENGL_2_ONLY
            CSR_MSAA* csrMSAACreate(size_t width, size_t height, size_t factor);
        #endif

        /**
        * Releases a multisample antialiasing
        *@param[in, out] pMSAA - multisample antialiasing to release
        */
        #ifndef CSR_OPENGL_2_ONLY
            void csrMSAARelease(CSR_MSAA* pMSAA);
        #endif

        /**
        * Initializes a multisample antialiasing structure
        *@param width - viewport width on which the antialiasing will be applied
        *@param height - viewport height on which the antialiasing will be applied
        *@param factor - antialiasing factor to apply, may be 2x, 4x or 8x
        *@param[in, out] pMSAA - multisample antialiasing to initialize
        *@return 1 on success, otherwise 0
        */
        #ifndef CSR_OPENGL_2_ONLY
            int csrMSAAInit(size_t width, size_t height, size_t factor, CSR_MSAA* pMSAA);
        #endif

        /**
        * Changes the size of a multisample antialiasing
        *@param width - new width to apply
        *@param height - new height to apply
        *@param[in, out] pMSAA - multisample antialiasing for which the size should change
        *@return 1 on success, otherwise 0
        */
        #ifndef CSR_OPENGL_2_ONLY
            int csrMSAAChangeSize(size_t width, size_t height, CSR_MSAA* pMSAA);
        #endif

        /**
        * Begins to draw an antialiased scene
        *@param pColor - scene background color
        *@param pMSAA - multisample antialiasing to apply
        */
        #ifndef CSR_OPENGL_2_ONLY
            void csrMSAASceneBegin(const CSR_Color* pColor, const CSR_MSAA* pMSAA);
        #endif

        /**
        * Ends to draw an antialiased scene
        *@param pMSAA - applied multisample antialiasing
        */
        #ifndef CSR_OPENGL_2_ONLY
            void csrMSAASceneEnd(const CSR_MSAA* pMSAA);
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
        *@note Only the item content is released, the item itself is not released
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
        *@param pContext - scene context
        *@param pItem - scene item to draw
        */
        void csrSceneItemDraw(const CSR_Scene*        pScene,
                              const CSR_SceneContext* pContext,
                              const CSR_SceneItem*    pItem);

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
        * Begins to draw a scene
        *@param pColor - scene background color
        */
        void csrSceneBegin(const CSR_Color* pColor);

        /**
        * Ends to draw a scene
        */
        void csrSceneEnd(void);

        /**
        * Adds a mesh to a scene
        *@param pScene - scene in which the mesh will be added
        *@param pMesh - mesh to add
        *@param transparent - if 1, the mesh is transparent, if 0 the mesh is opaque
        *@param aabb - if 1, the AABB tree will be generated for the mesh
        *@return 1 on success, otherwise 0
        *@note Once successfully added, the mesh will be owned by the scene and should no longer be
        *      released from outside
        */
        int csrSceneAddMesh(CSR_Scene* pScene, CSR_Mesh* pMesh, int transparent, int aabb);

        /**
        * Adds a model to a scene
        *@param pScene - scene in which the model will be added
        *@param pModel- model to add
        *@param transparent - if 1, the model is transparent, if 0 the model is opaque
        *@param aabb - if 1, the AABB tree will be generated for the mesh
        *@return 1 on success, otherwise 0
        *@note Once successfully added, the model will be owned by the scene and should no longer be
        *      released from outside
        */
        int csrSceneAddModel(CSR_Scene* pScene, CSR_Model* pModel, int transparent, int aabb);

        /**
        * Adds a MDL model to a scene
        *@param pScene - scene in which the model will be added
        *@param pMDL - model to add
        *@param transparent - if 1, the model is transparent, if 0 the model is opaque
        *@param aabb - if 1, the AABB tree will be generated for the mesh
        *@return 1 on success, otherwise 0
        *@note Once successfully added, the MDL model will be owned by the scene and should no
        *      longer be released from outside
        */
        int csrSceneAddMDL(CSR_Scene* pScene, CSR_MDL* pMDL, int transparent, int aabb);

        /**
        * Adds a model matrix to a scene item. Doing that the same model may be drawn several time
        * at different locations
        *@param pScene - scene in which the model will be added
        *@param pModel - model for which the matrix should be added
        *@param pMatrix - matrix to add
        *@return 1 on success, otherwise 0
        *@note The added matrix is not owned by the scene. For that reason it cannot be deleted as
        *      long as the scene uses it. The caller is responsible to delete the matrix if required
        */
        int csrSceneAddModelMatrix(CSR_Scene* pScene, const void* pModel, CSR_Matrix4* pMatrix);

        /**
        * Gets a scene item matching with a model or a matrix
        *@param pScene - scene from which the item should be get
        *@param pKey - search key, may be any model kind or a matrix
        *@return scene item, 0 if not found or on error
        */
        CSR_SceneItem* csrSceneGetItem(const CSR_Scene* pScene, const void* pKey);

        /**
        * Deletes a model or a matrix from the scene
        *@param pKey - key to delete, may be any model kind or a matrix
        */
        void csrSceneDeleteFrom(CSR_Scene* pScene, const void* pKey);

        /**
        * Draws a vertex buffer in a scene
        *@param pVB - vertex buffer to draw
        *@param pShader - shader to use to draw the vertex buffer
        *@param pMatrixList - matrices to use, one for each vertex buffer drawing. If 0, the model
        *                     matrix currently connected in the shader will be used
        *@note The shader must be first enabled with the csrShaderEnable() function
        */
        void csrSceneDrawVertexBuffer(const CSR_VertexBuffer* pVB,
                                      const CSR_Shader*       pShader,
                                      const CSR_MatrixList*   pMatrixList);

        /**
        * Draws a mesh in a scene
        *@param pMesh - mesh to draw
        *@param pShader - shader to use to draw the mesh
        *@param pMatrixList - matrices to use, one for each vertex buffer drawing. If 0, the model
        *                     matrix currently connected in the shader will be used
        */
        void csrSceneDrawMesh(const CSR_Mesh*       pMesh,
                              const CSR_Shader*     pShader,
                              const CSR_MatrixList* pMatrixList);

        /**
        * Draws a model in a scene
        *@param pModel - model to draw
        *@param index - model mesh index
        *@param pShader - shader to use to draw the mesh
        *@param pMatrixList - matrices to use, one for each vertex buffer drawing. If 0, the model
        *                     matrix currently connected in the shader will be used
        */
        void csrSceneDrawModel(const CSR_Model*      pModel,
                                     size_t          index,
                               const CSR_Shader*     pShader,
                               const CSR_MatrixList* pMatrixList);

        /**
        * Draws a MDL model in a scene
        *@param pMDL - MDL model to draw
        *@param pShader - shader to use to draw the model
        *@param pMatrixList - matrices to use, one for each vertex buffer drawing. If 0, the model
        *                     matrix currently connected in the shader will be used
        *@param textureIndex - texture index
        *@param modelIndex - model index
        *@param meshIndex - mesh index
        */
        void csrSceneDrawMDL(const CSR_MDL*        pMDL,
                             const CSR_Shader*     pShader,
                             const CSR_MatrixList* pMatrixList,
                                   size_t          textureIndex,
                                   size_t          modelIndex,
                                   size_t          meshIndex);

        /**
        * Draws a scene
        *@param pScene - scene to draw
        *@param pContext - scene context
        */
        void csrSceneDraw(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "CSR_Renderer.c"
#endif

#endif
