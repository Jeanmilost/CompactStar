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
* Matrix list
*/
typedef struct
{
    CSR_Matrix4* m_pMatrix;
    size_t       m_Count;
} CSR_MatrixList;

/**
* Scene item
*/
// FIXME do own and release the objects
// FIXME do allow the possibility to draw the same model with several matrices
typedef struct
{
    void*           m_pModel;       // the model to draw
    CSR_EModelType  m_Type;         // model type (a simple mesh, a model or a complex MDL model)
    CSR_MatrixList* m_pMatrixList;  // items matrices sharing the same model, e.g. all the walls of a room
    CSR_Shader*     m_pShader;      // shader to use to draw the model
    CSR_AABBNode*   m_pAABBTree;    // aligned-axis bounding box trees owned by the model
    size_t          m_TextureIndex; // texture index to show, in case the model contains several textures
    size_t          m_ModelIndex;   // for MDL models, model index to show, in case the MDL contains several models
    size_t          m_MeshIndex;    // for models and MDL, mesh index to show, in case a model contains several meshes
    int             m_Visible;      // if 1, the model is currently visible in the scene and thus should be drawn
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
        *@param aabb - if 1, the AABB tree will be generated for the mesh
        *@return 1 on success, otherwise 0
        *@note Once successfully added, the mesh will be owned by the scene and should no longer be
        *      released from outside
        */
        int csrSceneAddMesh(CSR_Scene*  pScene,
                            CSR_Mesh*   pMesh,
                            CSR_Shader* pShader,
                            int         transparent,
                            int         aabb);

        /**
        * Adds a model to a scene
        *@param pScene - scene in which the model will be added
        *@param pModel- model to add
        *@param pShader - shader to use to draw the model
        *@param transparent - if 1, the model is transparent, if 0 the model is opaque
        *@param aabb - if 1, the AABB tree will be generated for the mesh
        *@return 1 on success, otherwise 0
        *@note Once successfully added, the model will be owned by the scene and should no longer be
        *      released from outside
        */
        int csrSceneAddModel(CSR_Scene*  pScene,
                             CSR_Model*  pModel,
                             CSR_Shader* pShader,
                             int         transparent,
                             int         aabb);

        /**
        * Adds a MDL model to a scene
        *@param pScene - scene in which the model will be added
        *@param pMDL - model to add
        *@param pShader - shader to use to draw the model
        *@param transparent - if 1, the model is transparent, if 0 the model is opaque
        *@param aabb - if 1, the AABB tree will be generated for the mesh
        *@return 1 on success, otherwise 0
        *@note Once successfully added, the MDK model will be owned by the scene and should no
        *      longer be released from outside
        */
        int csrSceneAddMDL(CSR_Scene*  pScene,
                           CSR_MDL*    pMDL,
                           CSR_Shader* pShader,
                           int         transparent,
                           int         aabb);

        /**
        * Adds a MDL model to a scene
        *@param pScene - scene in which the model will be added
        *@param pMDL - model to add
        *@param pShader - shader to use to draw the model
        *@param transparent - if 1, the model is transparent, if 0 the model is opaque
        *@param aabb - if 1, the AABB tree will be generated for the mesh
        *@return 1 on success, otherwise 0
        *@note Once successfully added, the MDK model will be owned by the scene and should no
        *      longer be released from outside
        */
        int csrSceneAddModelMatrix(CSR_Scene*   pScene,
                                   CSR_Matrix4* pMatrix,
                                   CSR_Shader*  pShader,
                                   int          transparent,
                                   int          aabb);

        // FIXME add delete functions

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
    #include "CSR_Renderer.c"
#endif

#endif
