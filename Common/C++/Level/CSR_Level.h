/****************************************************************************
 * ==> CSR_Level -----------------------------------------------------------*
 ****************************************************************************
 * Description : This module represents a game level. It is the keeper of   *
 *               his important data, and manages his resources              *
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

#ifndef CSR_LevelH
#define CSR_LevelH

// std
#include <vector>
#include <map>

// compactStar engine
#include "CSR_Geometry.h"
#include "CSR_Renderer_OpenGL.h"
#include "CSR_Scene.h"
#include "CSR_Sound.h"

// classes
#include "CSR_OpenGLHelper.h"
#include "CSR_PostProcessingEffect_OilPainting.h"

/**
* Level manager
*@author Jean-Milost Reymond
*/
class CSR_Level
{
    public:
        /**
        * Item types
        */
        enum IEItemType
        {
            IE_IT_Unknown = 0,
            IE_IT_Surface,
            IE_IT_Box,
            IE_IT_Sphere,
            IE_IT_Cylinder,
            IE_IT_Disk,
            IE_IT_Ring,
            IE_IT_Spiral,
            IE_IT_Landscape,
            IE_IT_WaveFront,
            IE_IT_MDL
        };

        typedef std::vector<std::string>  IFileNames;
        typedef std::vector<CSR_Matrix4*> IMatrices;

        /**
        * Item files
        */
        struct IItemFiles
        {
            std::string m_Texture;
            std::string m_BumpMap;
            std::string m_LandscapeMap;
            std::string m_Model;

            IItemFiles();
            virtual ~IItemFiles();
        };

        /**
        * Item resources
        */
        struct IItemResources
        {
            IItemFiles m_Files;
            int        m_Faces;
            int        m_Slices;
            int        m_Stacks;
            int        m_Radius;
            int        m_DeltaMin;
            int        m_DeltaMax;
            int        m_DeltaZ;
            bool       m_RepeatTextureOnEachFace;

            IItemResources();
            virtual ~IItemResources();
        };

        /**
        * Level item
        */
        struct IItem
        {
            IEItemType     m_Type;
            IItemResources m_Resources;
            IMatrices      m_Matrices;

            IItem();
            virtual ~IItem();

            /**
            * Adds a new matrix in the item
            *@return newly added matrix item
            */
            virtual std::size_t AddMatrix();

            /**
            * Deletes an existing matrix
            *@param index - matrix index to delete
            */
            virtual void DeleteMatrix(std::size_t index);
        };

        /**
        * Skybox and background
        */
        struct ISkybox
        {
            std::string m_Left;
            std::string m_Top;
            std::string m_Right;
            std::string m_Bottom;
            std::string m_Front;
            std::string m_Back;

            ISkybox();
            virtual ~ISkybox();

            /**
            * Clears the data
            */
            virtual void Clear();
        };

        /**
        * Ambient sound
        */
        struct ISound
        {
            std::string m_FileName;

            ISound();
            virtual ~ISound();

            /**
            * Clears the data
            */
            virtual void Clear();
        };

        ISkybox          m_Skybox;
        ISound           m_Sound;
        CSR_SceneContext m_SceneContext;

        /**
        * Called when a cubemap texture should be loaded
        *@param fileNames - textures file names to load
        *@return texture identifier on the GPU, M_CSR_Error_Code on error
        */
        typedef GLuint (*ITfLoadCubemap)(const IFileNames& fileNames);

        /**
        * Called when a texture should be loaded
        *@param fileName - textures file name to load
        *@return texture identifier on the GPU, M_CSR_Error_Code on error
        */
        typedef GLuint (*ITfLoadTexture)(const std::string& fileName);

        /**
        * Called when the level designer should be updated
        *@param pKey - item key to update
        *@param index - model matrix index
        *@param modelLength - model length on each axis
        */
        typedef void (*ITfOnUpdateDesigner)(void* pKey, int index, const CSR_Vector3& modelLength);

        /**
        * Called when a model should be selected on the designer
        *@param pKey - item key to select
        *@param index - model matrix index
        */
        typedef void (*ITfSelectModel)(void* pKey, int index);

        CSR_Level();
        virtual ~CSR_Level();

        /**
        * Clears the level manager
        */
        virtual void Clear();

        /**
        * Builds the scene shader
        *@return true on success, otherwise false
        */
        virtual bool BuildSceneShader();

        /**
        * Creates the viewport
        *@param w - viewport width
        *@param h - viewport height
        */
        virtual void CreateViewport(float w, float h);

        /**
        * Adds the skybox from files
        *@param fileNames - skybox file names to load
        *@param fLoadCubemap - callback function to use to load the cubemap texture
        *@return true on success, otherwise false
        *@note The file names should be sorted as follow:
        *      1. The skybox right image
        *      2. The skybox left image
        *      3. The skybox top image
        *      4. The skybox bottom image
        *      5. The skybox front image
        *      6. The skybox back image
        */
        virtual bool AddSkybox(const IFileNames& fileNames, ITfLoadCubemap fLoadCubemap);

        /**
        * Adds a box in the level
        *@param matrix - model matrix
        *@param textureName - box texture file name to use
        *@param repeatTexOnEachFace - if true, the whole texture is repeated on each face
        *@param collisionType - collision type to apply to the model
        *@param fLoadTexture - callback function to use to load a texture
        *@param fSelectModel - callback function to select the newly added model on designer
        *@return newly added item key, NULL on error
        */
        virtual void* AddBox(const CSR_Matrix4&       matrix,
                             const std::string&       textureName,
                                   bool               repeatTexOnEachFace,
                                   CSR_ECollisionType collisionType,
                                   ITfLoadTexture     fLoadTexture,
                                   ITfSelectModel     fSelectModel);

        /**
        * Adds a cylinder in the level
        *@param matrix - model matrix
        *@param textureName - cylinder texture file name to use
        *@param faces - faces count
        *@param collisionType - collision type to apply to the model
        *@param fLoadTexture - callback function to use to load a texture
        *@param fSelectModel - callback function to select the newly added model on designer
        *@return newly added item key, NULL on error
        */
        virtual void* AddCylinder(const CSR_Matrix4&       matrix,
                                  const std::string&       textureName,
                                        int                faces,
                                        CSR_ECollisionType collisionType,
                                        ITfLoadTexture     fLoadTexture,
                                        ITfSelectModel     fSelectModel);

        /**
        * Adds a disk in the level
        *@param matrix - model matrix
        *@param textureName - disk texture file name to use
        *@param slices - slices count
        *@param collisionType - collision type to apply to the model
        *@param fLoadTexture - callback function to use to load a texture
        *@param fSelectModel - callback function to select the newly added model on designer
        *@return newly added item key, NULL on error
        */
        virtual void* AddDisk(const CSR_Matrix4&       matrix,
                              const std::string&       textureName,
                                    int                slices,
                                    CSR_ECollisionType collisionType,
                                    ITfLoadTexture     fLoadTexture,
                                    ITfSelectModel     fSelectModel);

        /**
        * Adds a ring in the level
        *@param matrix - model matrix
        *@param textureName - ring texture file name to use
        *@param slices - slices count
        *@param radius - ring internal radius in percent (between 0 and 100)
        *@param collisionType - collision type to apply to the model
        *@param fLoadTexture - callback function to use to load a texture
        *@param fSelectModel - callback function to select the newly added model on designer
        *@return newly added item key, NULL on error
        */
        virtual void* AddRing(const CSR_Matrix4&       matrix,
                              const std::string&       textureName,
                                    int                slices,
                                    int                radius,
                                    CSR_ECollisionType collisionType,
                                    ITfLoadTexture     fLoadTexture,
                                    ITfSelectModel     fSelectModel);

        /**
        * Adds a sphere in the level
        *@param matrix - model matrix
        *@param textureName - sphere texture file name to use
        *@param slices - slices count
        *@param stacks - stacks count
        *@param collisionType - collision type to apply to the model
        *@param fLoadTexture - callback function to use to load a texture
        *@param fSelectModel - callback function to select the newly added model on designer
        *@return newly added item key, NULL on error
        */
        virtual void* AddSphere(const CSR_Matrix4&       matrix,
                                const std::string&       textureName,
                                      int                slices,
                                      int                stacks,
                                      CSR_ECollisionType collisionType,
                                      ITfLoadTexture     fLoadTexture,
                                      ITfSelectModel     fSelectModel);

        /**
        * Adds a spiral in the level
        *@param matrix - model matrix
        *@param textureName - spiral texture file name to use
        *@param radius - spiral internal radius in percent (between 0 and 100)
        *@param deltaMin - delta to apply to the minimum radius
        *@param deltaMax - delta to apply to the maximum radius
        *@param deltaZ - delta to apply to the deep
        *@param slices - slices count
        *@param stacks - stacks count
        *@param collisionType - collision type to apply to the model
        *@param fLoadTexture - callback function to use to load a texture
        *@param fSelectModel - callback function to select the newly added model on designer
        *@return newly added item key, NULL on error
        */
        virtual void* AddSpiral(const CSR_Matrix4&       matrix,
                                const std::string&       textureName,
                                      int                radius,
                                      int                deltaMin,
                                      int                deltaMax,
                                      int                deltaZ,
                                      int                slices,
                                      int                stacks,
                                      CSR_ECollisionType collisionType,
                                      ITfLoadTexture     fLoadTexture,
                                      ITfSelectModel     fSelectModel);

        /**
        * Adds a surface in the level
        *@param matrix - model matrix
        *@param textureName - surface texture file name to use
        *@param collisionType - collision type to apply to the model
        *@param fLoadTexture - callback function to use to load a texture
        *@param fSelectModel - callback function to select the newly added model on designer
        *@return newly added item key, NULL on error
        */
        virtual void* AddSurface(const CSR_Matrix4&       matrix,
                                 const std::string&       textureName,
                                       CSR_ECollisionType collisionType,
                                       ITfLoadTexture     fLoadTexture,
                                       ITfSelectModel     fSelectModel);

        /**
        * Adds a WaveFront model in the level
        *@param matrix - model matrix
        *@param fileName - model file name to load
        *@param textureName - model texture file name to use
        *@param pBuffer - optional buffer containing the model file content, ignored if NULL
        *@param collisionType - collision type to apply to the model
        *@param fLoadTexture - callback function to use to load a texture
        *@param fSelectModel - callback function to select the newly added model on designer
        *@return newly added item key, NULL on error
        */
        virtual void* AddWaveFront(const CSR_Matrix4&       matrix,
                                   const std::string&       fileName,
                                   const std::string&       textureName,
                                   const CSR_Buffer*        pBuffer,
                                         CSR_ECollisionType collisionType,
                                         ITfLoadTexture     fLoadTexture,
                                         ITfSelectModel     fSelectModel);

        /**
        * Adds a Quake I model in the level
        *@param matrix - model matrix
        *@param fileName - model file name to load
        *@param pBuffer - optional buffer containing the model file content, ignored if NULL
        *@param collisionType - collision type to apply to the model
        *@param fLoadTexture - callback function to use to load a texture
        *@param fSelectModel - callback function to select the newly added model on designer
        *@return newly added item key, NULL on error
        */
        virtual void* AddMDL(const CSR_Matrix4&       matrix,
                             const std::string&       fileName,
                             const CSR_Buffer*        pBuffer,
                                   CSR_ECollisionType collisionType,
                                   CSR_fOnApplySkin   fOnApplySkin,
                                   ITfSelectModel     fSelectModel);

        /**
        * Adds a landscape from a file
        *@param fileName - model file name to load from
        *@param textureName - model texture name
        *@param pBuffer - optional buffer containing the model file content, ignored if NULL
        *@param fOnUpdateDesigner - callback function to use to update the designer, may be NULL
        *@return newly added item key, NULL on error
        *@note All landscape model will always support the ground and custom collision types
        */
        virtual void* AddLandscape(const std::string&        fileName,
                                   const std::string&        textureName,
                                   const CSR_Buffer*         pBuffer,
                                         ITfOnUpdateDesigner fOnUpdateDesigner);

        /**
        * Adds a landscape from a grayscale bitmap
        *@param fileName - grayscale bitmap from which the model will be generated
        *@param textureName - model texture name
        *@param pBuffer - optional buffer containing the grayscale bitmap file content, ignored if NULL
        *@param fOnUpdateDesigner - callback function to use to update the designer, may be NULL
        *@return newly added item key, NULL on error
        *@note All landscape model will always support the ground and custom collision types
        */
        virtual void* AddLandscapeFromBitmap(const std::string&        fileName,
                                             const std::string&        textureName,
                                             const CSR_Buffer*         pBuffer,
                                                   ITfOnUpdateDesigner fOnUpdateDesigner);

        /**
        * Adds a duplicate of an existing model in the level
        *@param pKey - key of the model to duplicate
        *@param matrix - model matrix
        *@param fSelectModel - callback function to select the newly added model on designer
        *@return newly added duplicate matrix index, -1 on error
        */
        virtual int AddDuplicate(void* pKey, const CSR_Matrix4& matrix, ITfSelectModel fSelectModel);

        /**
        * Opens the level ambient sound
        *@param fileName - sound file name to open
        *@param pBuffer - optional buffer containing the Wav file content, ignored if NULL
        *@return true on success, otherwise false
        */
        virtual bool OpenSound(const std::string& fileName, const CSR_Buffer* pBuffer);

        /**
        * Pauses the level ambient sound
        *@param pause - if true, the sound will be paused, resumed otherwise
        */
        virtual void PauseSound(bool pause);

        /**
        * Gets the view x position
        *@return the view x position
        */
        virtual float GetViewXPos() const;

        /**
        * Gets the view y position
        *@return the view y position
        */
        virtual float GetViewYPos() const;

        /**
        * Gets the view z position
        *@return the view z position
        */
        virtual float GetViewZPos() const;

        /**
        * Creates a default scene
        *@return the newly created scene, NULL if no scene
        */
        virtual CSR_Scene* CreateScene();

        /**
        * Gets the scene
        *@return the scene, NULL if no scene
        */
        virtual CSR_Scene* GetScene() const;

        /**
        * Enables the main level shader
        */
        virtual void EnableShader() const;

        /**
        * Enables or disables the scene multisample antialiasing
        *@param value - if true, antialiasing is enabled, disabled otherwise
        */
        virtual void EnableMSAA(bool value);

        /**
        * Enables or disables the scene oil painting effect
        *@param value - if true, oil painting is enabled, disabled otherwise
        */
        virtual void EnableOilPainting(bool value);

        /**
        * Adds a texture to the OpenGL resources
        *@param pKey - key for which the texture should be added
        *@param textureID - texture identifier to add
        */
        virtual void AddTexture(const void* pKey, GLuint textureID);

        /**
        * Deletes a texture from the OpenGL resources
        *@param pKey - key for which the texture should be delete
        */
        virtual void DeleteTexture(const void* pKey);

        /**
        * Adds a new item in the level manager, or a new matrix in an existing item
        *@param pKey - key for which the item should be added
        *@return the item containing the newly added matrix, NULL on error
        *@note The item will be added if still not exists, but the matching item will be returned if
        *      the key already exists. In all case the last item matrix is always the one added for
        *      the new item
        */
        virtual IItem* Add(void* pKey);

        /**
        * Deletes an item from the level manager, or a matrix from an existing item
        *@param pKey - key for which the item should be deleted
        *@param index - item matrix index to delete
        *@note The matrix will be deleted in the item at the provided index. In case the item no
        *      longer contains matrix, it will be deleted from the manager
        */
        virtual void Delete(void* pKey, std::size_t index);

        /**
        * Deletes an item from the level manager
        *@param pKey - key for which the item should be deleted
        */
        virtual void Delete(void* pKey);

        /**
        * Gets the item in the level manager matching with a key
        *@param pKey - key for which the item should be get
        *@return the item matching with the key, NULL if not found or on error
        */
        virtual IItem* Get(void* pKey) const;

        /**
        * Gets the scene item containing the main landscape
        *@return the scene item containing the main landscape, NULL if no landscape or on error
        */
        virtual CSR_SceneItem* GetLandscape() const;

        /**
        * Gets a scene item
        *@return the scene item, NULL if not found or on error
        */
        virtual CSR_SceneItem* GetSceneItem(void* pKey) const;

        /**
        * Calculates a matrix where to put the point of view to lie on the ground
        *@param pBoundingSphere - sphere surrounding the point of view
        *@param angle - point of view direction angle on the y axis
        *@param oldPos - the previous point of view position before calculation
        */
        virtual void ApplyGroundCollision(CSR_Sphere*  pBoundingSphere,
                                          float        angle,
                                    const CSR_Vector3& oldPos) const;

        /**
        * Draws the level
        */
        virtual void Draw() const;

        /**
        * Called when a skin should be applied to a model
        *@param index - skin index (in case the model contains several skins)
        *@param pSkin - skin
        *@param[in, out] pCanRelease - if 1, the skin content may be released after the skin is applied
        */
        virtual void OnApplySkin(size_t index, const CSR_Skin* pSkin, int* pCanRelease);

        /**
        * Called when a shader should be get for a model
        *@param pModel - model for which the shader shoudl be get
        *@param type - model type
        *@return shader to use to draw the model, 0 if no shader
        *@note The model will not be drawn if no shader is returned
        */
        virtual void* OnGetShader(const void* pModel, CSR_EModelType type);

        /**
        * Called when a resource identifier should be get from a key
        *@param pKey - key for which the resource identifier should be get
        *@return identifier, 0 on error or if not found
        */
        virtual void* OnGetID(const void* pKey);

        /**
        * Called when scene begins
        *@param pScene - scene to begin
        *@param pContext - scene context
        */
        virtual void OnSceneBegin(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

        /**
        * Called when scene ends
        *@param pScene - scene to end
        *@param pContext - scene context
        */
        virtual void OnSceneEnd(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

        /**
        * Called when a texture should be deleted
        *@param pTexture - texture to delete
        */
        virtual void OnDeleteTexture(const CSR_Texture* pTexture);

    private:
        // IMPORTANT NOTE don't use a CSR_SceneItem as key for the map, because this pointer may
        // change during the level lifecycle, whereas the model pointer not
        typedef std::map<void*, IItem*> IItems;

        ALCdevice*                            m_pOpenALDevice;
        ALCcontext*                           m_pOpenALContext;
        CSR_Scene*                            m_pScene;
        IItems                                m_Items;
        CSR_OpenGLShader*                     m_pShader;
        CSR_OpenGLShader*                     m_pSkyboxShader;
        CSR_OpenGLHelper::IResources          m_OpenGLResources;
        CSR_PostProcessingEffect_OilPainting* m_pEffect;
        CSR_OpenGLMSAA*                       m_pMSAA;
        CSR_Sound*                            m_pSound;
        void*                                 m_pLandscapeKey;
        bool                                  m_UseMSAA;
        bool                                  m_UseOilPainting;

        /**
        * Builds a default scene
        */
        void BuildScene();

        /**
        * Clears the scene
        */
        void ClearScene();

        /**
        * Loads a sound
        *@param fileName - sound file name to load
        *@param pBuffer - optional buffer containing the grayscale bitmap file content, ignored if NULL
        *@return loaded sound on success, otherwise NULL
        *@note The returned sound should be released once useless
        */
        CSR_Sound* LoadSound(const std::string& fileName, const CSR_Buffer* pBuffer) const;

        /**
        * Clears the sound
        */
        void ClearSound();

        /**
        * Calculates the length of a model on each axis
        *@param pItemIndex - item index containing the model
        *@return the model length
        */
        CSR_Vector3 CalculateModelLength(const CSR_SceneItem* pSceneItem) const;

        /**
        * Calculates a matrix where to put the point of view to lie on the ground
        *@param pBoundingSphere - sphere surrounding the point of view
        *@param angle - view direction angle on the y axis
        *@param[out] pMatrix - resulting view matrix
        *@return true if new position is valid, otherwise false
        */
        bool ApplyGroundCollision(const CSR_Sphere*  pBoundingSphere,
                                        float        angle,
                                        CSR_Matrix4* pMatrix) const;
};
#endif
