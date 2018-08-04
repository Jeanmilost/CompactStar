/****************************************************************************
 * ==> Main ----------------------------------------------------------------*
 ****************************************************************************
 * Description : Cross football game main form                              *
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

#ifndef MainH
#define MainH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.AppEvnts.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Dialogs.hpp>

// std
#include <string>
#include <vector>
#include <map>

// compactStar engine
#include "CSR_Shader.h"
#include "CSR_Scene.h"
#include "CSR_Physics.h"
#include "CSR_Sound.h"

// classes
#include "CSR_VCLHelper.h"

/**
* Ground collision tool main form
*@author Jean-Milost Reymond
*/
class TMainForm : public TForm
{
    __published:
        TApplicationEvents *aeEvents;

        void __fastcall FormShow(TObject* pSender);
        void __fastcall FormResize(TObject* pSender);
        void __fastcall aeEventsMessage(tagMSG& msg, bool& handled);

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
        * Called when a shader should be get for a model
        *@param pModel - model for which the shader shoudl be get
        *@param type - model type
        *@return shader to use to draw the model, 0 if no shader
        *@note The model will not be drawn if no shader is returned
        */
        static CSR_Shader* OnGetShaderCallback(const void* pModel, CSR_EModelType type);

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

    private:
        /**
        * Structure representing a physical ball
        */
        struct CSR_Ball
        {
            void*       m_pKey;
            CSR_Matrix4 m_Matrix;
            CSR_Sphere  m_Geometry;
            CSR_Body    m_Body;
        };

        typedef std::vector<std::string> IFileNames;

        HDC              m_hDC;
        HGLRC            m_hRC;
        ALCdevice*       m_pOpenALDevice;
        ALCcontext*      m_pOpenALContext;
        CSR_Scene*       m_pScene;
        CSR_SceneContext m_SceneContext;
        CSR_Shader*      m_pShader;
        CSR_Shader*      m_pSkyboxShader;
        CSR_Sphere       m_ViewSphere;
        CSR_Ball         m_Ball;
        CSR_Vector3      m_FrictionForce;
        CSR_Matrix4      m_LandscapeMatrix;
        CSR_Sound*       m_pSound;
        void*            m_pLandscapeKey;
        std::string      m_SceneDir;
        std::size_t      m_FrameCount;
        int              m_PrevOrigin;
        float            m_Angle;
        float            m_RollAngle;
        float            m_BallDirAngle;
        float            m_BallOffset;
        float            m_PosVelocity;
        float            m_DirVelocity;
        float            m_StepTime;
        float            m_StepInterval;
        double           m_FPS;
        unsigned __int64 m_StartTime;
        unsigned __int64 m_PreviousTime;
        bool             m_Initialized;

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

        int LoadLandscapeFromBitmap(const char* fileName);

        int ApplyGroundCollision(const CSR_Sphere*  pBoundingSphere,
                                       float        dir,
                                       CSR_Matrix4* pMatrix,
                                       CSR_Plane*   pGroundPlane) const;

        void ApplyPhysics(float elapsedTime);

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
        CSR_Shader* OnGetShader(const void* pModel, CSR_EModelType type);

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
        * Called while application is idle
        *@param pSender - event sender
        *@param[in, out] done - if true, event is done and will no longer be called
        */
        void __fastcall OnIdle(TObject* pSender, bool& done);
};
extern PACKAGE TMainForm* MainForm;
#endif
