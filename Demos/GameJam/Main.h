/****************************************************************************
 * ==> Cave game jam main form ---------------------------------------------*
 ****************************************************************************
 * Description : Cave game jame main form                                   *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2019, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

#ifndef MainH
#define MainH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.AppEvnts.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.pngimage.hpp>

// std
#include <string>
#include <vector>
#include <map>

// compactStar engine
#include "CSR_Renderer_OpenGL.h"
#include "CSR_Scene.h"
#include "CSR_Physics.h"
#include "CSR_AI.h"
#include "CSR_Sound.h"

// classes
#include "CSR_OpenGLHelper.h"
#include "CSR_VCLHelper.h"

/**
* Cave game jam main form
*@author Jean-Milost Reymond
*/
class TMainForm : public TForm
{
    __published:
        TApplicationEvents *aeEvents;
        TPanel *paFirstItem;
        TPanel *paSecondItem;
        TImage *imFirstItem;
        TImage *imSecondItem;
        TPanel *paMessage;
        TLabel *laMessage;
        TTimer *tmHideMsg;
        TPanel *paWarning;
        TImage *imWarning;
        TPanel *paMainMenu;
        TPanel *paGameOverMsg;
        TLabel *laGameOverMsg;
        TLabel *laGameOverInfoMsg;
        TLabel *laStartGameWithoutBot;
        TLabel *laExit;
        TTimer *tmCloseGameOver;
        TLabel *laStartGameWithBot;
        TLabel *laTitle;
        TImage *imBackground;

        void __fastcall FormShow(TObject* pSender);
        void __fastcall FormResize(TObject* pSender);
        void __fastcall aeEventsMessage(tagMSG& msg, bool& handled);
        void __fastcall tmHideMsgTimer(TObject* pSender);
        void __fastcall tmCloseGameOverTimer(TObject* pSender);
        void __fastcall laMenuItemMouseEnter(TObject* pSender);
        void __fastcall laMenuItemMouseLeave(TObject* pSender);
        void __fastcall laStartGameWithoutBotClick(TObject* pSender);
        void __fastcall laStartGameWithBotClick(TObject* pSender);
        void __fastcall laExitClick(TObject* pSender);

    public:
        /**
        * Constructor
        *@param pOwner - form owner
        */
        __fastcall TMainForm(TComponent* pOwner);

        /**
        * Destructor
        */
        virtual __fastcall ~TMainForm();

        /**
        * Called when a task changed
        *@param pTask - the new task to execute
        *@param elapsedTime - elapsed time since last frame
        */
        static void OnTaskChangeCallback(CSR_Task* pTask, double elapsedTime);

        /**
        * Called when a task should be executed
        *@param pTask - the running task
        *@param elapsedTime - elapsed time since last frame
        */
        static int OnTaskRunCallback(CSR_Task* pTask, double elapsedTime);

        /**
        * Called when a shader should be get for a model
        *@param pModel - model for which the shader shoudl be get
        *@param type - model type
        *@return shader to use to draw the model, 0 if no shader
        *@note The model will not be drawn if no shader is returned
        */
        static void* OnGetShaderCallback(const void* pModel, CSR_EModelType type);

        /**
        * Called when scene begins
        *@param pScene - scene to begin
        *@param pContext - scene context
        */
        static void OnSceneBeginCallback(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

        /**
        * Called when scene ends
        *@param pScene - scene to end
        *@param pContext - scene context
        */
        static void OnSceneEndCallback(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

        /**
        * Called when a skin should be applied to a model
        *@param index - skin index (in case the model contains several skins)
        *@param pSkin - skin
        *@param[in, out] pCanRelease - if 1, the skin content may be released after the skin is applied
        */
        static void OnApplySkinCallback(std::size_t index, const CSR_Skin* pSkin, int* pCanRelease);

        /**
        * Called when a resource identifier should be get from a key
        *@param pKey - key for which the resource identifier should be get
        *@return identifier, 0 on error or if not found
        */
        static void* OnGetIDCallback(const void* pKey);

        /**
        * Called when the MDL model indexes should be get
        *@param pMDL - MDL model for which the indexes should be get
        *@param pSkinIndex - skin index
        *@param pModelIndex - model index
        *@param pMeshIndex - mesh index
        */
        static void OnGetMDLIndexCallback(const CSR_MDL*     pMDL,
                                                std::size_t* pSkinIndex,
                                                std::size_t* pModelIndex,
                                                std::size_t* pMeshIndex);

        /**
        * Called when a texture should be deleted
        *@param pTexture - texture to delete
        */
        static void OnDeleteTextureCallback(const CSR_Texture* pTexture);

    private:
        /**
        * Artificial intelligence tasks enumeration
        */
        enum IEBotTask
        {
            E_BT_Watching,
            E_BT_Searching,
            E_BT_Attacking,
            E_BT_Dying
        };

        /**
        * Bot dying sequence
        */
        enum IEDyingSequence
        {
            E_DS_None,
            E_DS_Dying,
            E_DS_FadeOut,
            E_DS_FadeIn
        };

        /**
        * Bot
        */
        struct IBot
        {
            CSR_MDL*        m_pModel;
            CSR_Sphere      m_Geometry;
            CSR_Vector2     m_StartPosition;
            CSR_Vector2     m_EndPosition;
            CSR_Vector3     m_Dir;
            CSR_Matrix4     m_Matrix;
            IEDyingSequence m_DyingSequence;
            float           m_Angle;
            float           m_Velocity;
            float           m_MovePos;

            IBot();
        };

        /**
        * Collectible object item (may be dynamite, matches, ...)
        */
        struct ICollectible
        {
            CSR_Mesh*   m_pModel;
            CSR_Sphere  m_Geometry;
            CSR_Matrix4 m_Matrix;

            ICollectible();
        };

        /**
        * Terrain limits
        */
        typedef struct
        {
            CSR_Vector2 m_Min;
            CSR_Vector2 m_Max;
        } CSR_TerrainLimits;

        typedef std::vector<std::string> IFileNames;

        HDC                          m_hDC;
        HGLRC                        m_hRC;
        HBRUSH                       m_BackgroundBrush;
        CSR_OpenGLHelper::IResources m_OpenGLResources;
        ALCdevice*                   m_pOpenALDevice;
        ALCcontext*                  m_pOpenALContext;
        CSR_Scene*                   m_pScene;
        CSR_TaskManager*             m_pTaskManager;
        CSR_OpenGLShader*            m_pShader;
        CSR_OpenGLShader*            m_pSkyboxShader;
        TBitmap*                     m_pBackground;
        IBot                         m_Bot;
        ICollectible                 m_Dynamite;
        ICollectible                 m_Matches;
        ICollectible                 m_Door;
        void*                        m_pLandscapeKey;
        float                        m_MapHeight;
        float                        m_MapScale;
        float                        m_Angle;
        float                        m_RotationSpeed;
        float                        m_StepTime;
        float                        m_StepInterval;
        float                        m_PosVelocity;
        float                        m_DirVelocity;
        const float                  m_ControlRadius;
        float                        m_Time;
        float                        m_Interval;
        double                       m_TextureLastTime;
        double                       m_ModelLastTime;
        double                       m_MeshLastTime;
        double                       m_WatchingTime;
        double                       m_ElapsedTime;
        std::size_t                  m_AnimIndex;
        std::size_t                  m_TextureIndex;
        std::size_t                  m_ModelIndex;
        std::size_t                  m_MeshIndex;
        std::size_t                  m_LastKnownIndex;
        std::size_t                  m_FrameCount;
        int                          m_AlternateStep;
        CSR_SceneContext             m_SceneContext;
        CSR_TaskContext              m_TaskContext;
        CSR_Sphere                   m_BoundingSphere;
        CSR_Matrix4                  m_LandscapeMatrix;
        CSR_Matrix4                  m_FaderMatrix;
        CSR_Vector2                  m_TouchOrigin;
        CSR_Vector2                  m_TouchPosition;
        CSR_Color                    m_Color;
        CSR_TerrainLimits            m_TerrainLimits;
        GLuint                       m_AlphaSlot;
        CSR_Sound*                   m_pCaveAmbientSound;
        CSR_Sound*                   m_pFootStepLeftSound;
        CSR_Sound*                   m_pFootStepRightSound;
        CSR_Sound*                   m_pHitSound;
        CSR_Sound*                   m_pDingSound;
        CSR_Sound*                   m_pWarningSound;
        std::string                  m_SceneDir;
        double                       m_FPS;
        float                        m_BotAlpha;
        int                          m_BotShowPlayer;
        int                          m_BotHitPlayer;
        int                          m_BotDying;
        int                          m_PlayerDying;
        unsigned __int64             m_StartTime;
        unsigned __int64             m_PreviousTime;
        bool                         m_Initialized;
        bool                         m_ShowBot;
        bool                         m_GameOver;

        /**
        * Loads a texture
        *@param fileName - texture file name to load
        *@return texture identifier on the GPU, M_CSR_Error_Code on error
        */
        GLuint LoadTexture(const std::string& fileName) const;

        /**
        * Loads a cubemap texture
        *@param fileNames - textures file names to load
        *@return texture identifier on the GPU, M_CSR_Error_Code on error
        */
        GLuint LoadCubemap(const IFileNames fileNames) const;

        /**
        * Initializes the scene
        *@param w - scene width
        *@param h - scene height
        */
        void InitScene(int w, int h);

        /**
        * Deletes the scene
        */
        void DeleteScene();

        /**
        * Updates the scene
        *@param elapsedTime - elapsed time since last update, in milliseconds
        */
        void UpdateScene(float elapsedTime);

        /**
        * Draws the scene
        */
        void DrawScene();

        /**
        * Loads a landscape from a bitmap image
        *@param fileName - bitmap file name
        *@return 1 on success, otherwise 0
        */
        int LoadLandscapeFromBitmap(const char* fileName);

        /**
        * Builds the bot matrix
        */
        void BuildBotMatrix();

        /**
        * Randomize the item positions in the scene
        */
        void RandomizeItems();

        /**
        * Calculates the ground collision point
        *@param pBoundingSphere - bounding sphere for which the ground collision point should be found
        *@param pMatrix - matrix of the model surrounded by the bounding sphere
        *@return 1 if a collision was found, otherwise 0
        */
        int ApplyGroundCollision(const CSR_Sphere* pBoundingSphere, CSR_Matrix4* pMatrix) const;

        /**
        * Checks if the bot may show the player
        *@return 1 if the bot shows the player, otherwise 0
        */
        int CheckPlayerVisible();

        /**
        * Checks if the bot hits the player
        *@return 1 if the bot hits the player, otherwise 0
        */
        int CheckBotHitPlayer();

        /**
        * Called when the watching task should be prepared
        */
        void OnPrepareWatching();

        /**
        * Called when the watching task should be applied
        *@param elapsedTime - elapsed time since last frame
        *@return 1 if the task was sucessfully terminated, otherwise 0
        */
        int OnWatching(double elapsedTime);

        /**
        * Called when the searching task should be prepared
        */
        void OnPrepareSearching();

        /**
        * Called when the searching task should be applied
        *@param elapsedTime - elapsed time since last frame
        *@return 1 if the task was sucessfully terminated, otherwise 0
        */
        int OnSearching(double elapsedTime);

        /**
        * Called when the attacking task should be prepared
        */
        void OnPrepareAttacking();

        /**
        * Called when the attacking task should be applied
        *@param elapsedTime - elapsed time since last frame
        *@return 1 if the task was sucessfully terminated, otherwise 0
        */
        int OnAttacking(double elapsedTime);

        /**
        * Called when the dying task should be prepared
        */
        void OnPrepareDying();

        /**
        * Called when the dying task should be applied
        *@param elapsedTime - elapsed time since last frame
        *@return 1 if the task was sucessfully terminated, otherwise 0
        */
        int OnDying(double elapsedTime);

        /**
        * Called when a task changed
        *@param pTask - the new task to execute
        *@param elapsedTime - elapsed time since last frame
        */
        void OnTaskChange(CSR_Task* pTask, double elapsedTime);

        /**
        * Called when a task should be executed
        *@param pTask - the running task
        *@param elapsedTime - elapsed time since last frame
        */
        int OnTaskRun(CSR_Task* pTask, double elapsedTime);

        /**
        * Called when the scene should be drawn
        *@param resize - if true, the scene should be repainted during a resize
        */
        void OnDrawScene(bool resize);

        /**
        * Called when a shader should be get for a model
        *@param pModel - model for which the shader shoudl be get
        *@param type - model type
        *@return shader to use to draw the model, 0 if no shader
        *@note The model will not be drawn if no shader is returned
        */
        void* OnGetShader(const void* pModel, CSR_EModelType type);

        /**
        * Called when scene begins
        *@param pScene - scene to begin
        *@param pContext - scene context
        */
        void OnSceneBegin(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

        /**
        * Called when scene ends
        *@param pScene - scene to end
        *@param pContext - scene context
        */
        void OnSceneEnd(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

        /**
        * Called when a skin should be applied to a model
        *@param index - skin index (in case the model contains several skins)
        *@param pSkin - skin
        *@param[in, out] pCanRelease - if 1, the skin content may be released after the skin is applied
        */
        void OnApplySkin(std::size_t index, const CSR_Skin* pSkin, int* pCanRelease);

        /**
        * Called when a resource identifier should be get from a key
        *@param pKey - key for which the resource identifier should be get
        *@return identifier, 0 on error or if not found
        */
        void* OnGetID(const void* pKey);

        /**
        * Called when the MDL model indexes should be get
        *@param pMDL - MDL model for which the indexes should be get
        *@param pSkinIndex - skin index
        *@param pModelIndex - model index
        *@param pMeshIndex - mesh index
        */
        void OnGetMDLIndex(const CSR_MDL*     pMDL,
                                 std::size_t* pSkinIndex,
                                 std::size_t* pModelIndex,
                                 std::size_t* pMeshIndex);

        /**
        * Called when a texture should be deleted
        *@param pTexture - texture to delete
        */
        void OnDeleteTexture(const CSR_Texture* pTexture);

        /**
        * Called when the game is over
        *@param failed - if true, the player has failed
        */
        void OnGameOver(bool failed);

        /**
        * Called while application is idle
        *@param pSender - event sender
        *@param[in, out] done - if true, event is done and will no longer be called
        */
        void __fastcall OnIdle(TObject* pSender, bool& done);
};
extern PACKAGE TMainForm *MainForm;
#endif
