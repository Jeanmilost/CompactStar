/****************************************************************************
 * ==> Spaceship game demo -------------------------------------------------*
 ****************************************************************************
 * Description : A spaceship game demo                                      *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2022, this file is part of the CompactStar Engine.  *
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
#include "CSR_Particles.h"
#include "CSR_Renderer_OpenGL.h"
#include "CSR_Scene.h"
#include "CSR_Sound.h"

// classes
#include "CSR_OpenGLHelper.h"
#include "CSR_VCLHelper.h"

//----------------------------------------------------------------------------
// Global defines
//----------------------------------------------------------------------------
#define TEXTURE_COUNT 4
#define STAR_COUNT    50
#define METEORE_COUNT 20
//----------------------------------------------------------------------------

/**
* Spaceship game demo main form
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
        * Called when a star motion should be calculated
        *@param pParticles - stars particle system
        *@param pParticle - star for which the motion should be calculated
        *@param elapsedTime - elapsed time since last calculation
        */
        static void OnCalculateStarMotionCallback(const CSR_Particles* pParticles,
                                                        CSR_Particle*  pParticle,
                                                        float          elapsedTime);

        /**
        * Called when a meteore motion should be calculated
        *@param pParticles - meteores particle system
        *@param pParticle - meteore for which the motion should be calculated
        *@param elapsedTime - elapsed time since last calculation
        */
        static void OnCalculateMeteoreMotionCallback(const CSR_Particles* pParticles,
                                                           CSR_Particle*  pParticle,
                                                           float          elapsedTime);

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
        * Called when a texture should be deleted
        *@param pTexture - texture to delete
        */
        static void OnDeleteTextureCallback(const CSR_Texture* pTexture);

    private:
        /**
        * Spaceship
        */
        struct ISpaceship
        {
            void*       m_pKey;
            CSR_Matrix4 m_Matrix;
            CSR_Rect    m_Bounds;
        };

        /**
        * Meteore
        */
        struct IMeteore
        {
            CSR_Vector3 m_StartPos;
            CSR_Vector3 m_Scaling;
            CSR_Vector3 m_Rotation;
            CSR_Vector3 m_RotationVelocity;
        };

        HDC                          m_hDC;
        HGLRC                        m_hRC;
        ALCdevice*                   m_pOpenALDevice;
        ALCcontext*                  m_pOpenALContext;
        CSR_Scene*                   m_pScene;
        CSR_SceneContext             m_SceneContext;
        CSR_OpenGLShader*            m_pShader;
        CSR_OpenGLShader*            m_pFlameShader;
        CSR_OpenGLHelper::IResources m_OpenGLResources;
        float                        m_Angle;
        float                        m_Alpha;
        float                        m_MinAlpha;
        float                        m_MaxAlpha;
        float                        m_AlphaOffset;
        float                        m_ShipAlpha;
        float                        m_LaserTime;
        float                        m_LaserInterval;
        float                        m_RotateVelocity;
        float                        m_Aspect;
        bool                         m_LaserVisible;
        bool                         m_ShipDying;
        ISpaceship                   m_Spaceship;
        IMeteore                     m_Meteores[METEORE_COUNT];
        CSR_MDL*                     m_pMeteore;
        CSR_Mesh*                    m_pFlame;
        CSR_Mesh*                    m_pLaser;
        CSR_Mesh*                    m_pStar;
        CSR_Box                      m_StarBox;
        CSR_Particles*               m_pStars;
        CSR_Particles*               m_pMeteores;
        CSR_Circle                   m_SpaceshipBoundingCircle;
        CSR_Circle                   m_MeteoreBoundingCircle;
        CSR_Ray2                     m_LaserRay;
        CSR_Vector2                  m_ScreenOrigin;
        CSR_Matrix4                  m_FlameMatrix;
        CSR_Matrix4                  m_LaserMatrix;
        CSR_Matrix4                  m_Background;
        GLuint                       m_AlphaSlot;
        GLuint                       m_TexAlphaSlot;
        CSR_Sound*                   m_pFireSound;
        CSR_Sound*                   m_pExplosionSound;
        CSR_OpenGLID                 m_ID[TEXTURE_COUNT];
        std::string                  m_SceneDir;
        std::size_t                  m_FrameCount;
        double                       m_FPS;
        unsigned __int64             m_StartTime;
        unsigned __int64             m_PreviousTime;
        bool                         m_Initialized;

        /**
        * Loads a texture
        *@param fileName - texture file name to load
        *@return texture identifier on the GPU, M_CSR_Error_Code on error
        */
        GLuint LoadTexture(const std::string& fileName) const;

        /**
        * Checks if the laser hits a meteore
        *@param pMeteoreBoundingCircle - meteore bounding circle
        *@param pLaserRay - laser ray
        *@return true if laser hits a meteore, otherwise false
        */
        bool CheckLaserCollision(const CSR_Circle* pMeteoreBoundingCircle,
                                 const CSR_Ray2*   pLaserRay) const;

        /**
        * Checks if the spaceship hits a meteore
        *@param pMeteoreBoundingCircle - meteore bounding circle
        *@param pSpaceshipBoundingCircle - spaceship bounding circle
        *@return true if spaceship hits a meteore, otherwise false
        */
        bool CheckSpaceshipCollision(const CSR_Circle* pMeteoreBoundingCircle,
                                     const CSR_Circle* pSpaceshipBoundingCircle) const;

        /**
        * Builds the spaceship matrix
        *@param angle - spaceship direction
        */
        void BuildSpaceshipMatrix(float angle);

        /**
        * Builds the flame matrix
        *@param angle - spaceship direction
        */
        void BuildFlameMatrix(float angle);

        /**
        * Builds the meteore matrix
        *@param pTranslation - translation
        *@param pRotation - rotation
        *@param pScaling - scaling
        *@param[in, out] pMeteoreMatrix - meteore matrix
        */
        void BuildMeteoreMatrix(const CSR_Vector3* pTranslation,
                                const CSR_Vector3* pRotation,
                                const CSR_Vector3* pScaling,
                                      CSR_Matrix4* pMeteoreMatrix) const;

        /**
        * Builds the laser matrix
        *@param angle - spaceship direction
        */
        void BuildLaserMatrix(float angle);

        /**
        * Calculates the next star position
        *@param pParticles - stars particle system
        *@param pParticle - star for which the motion should be calculated
        *@param elapsedTime - elapsed time since last calculation
        */
        void OnCalculateStarMotion(const CSR_Particles* pParticles,
                                         CSR_Particle*  pParticle,
                                         float          elapsedTime) const;

        /**
        * Calculates the next meteore position
        *@param pParticles - meteores particle system
        *@param pParticle - meteore for which the motion should be calculated
        *@param elapsedTime - elapsed time since last calculation
        */
        void OnCalculateMeteoreMotion(const CSR_Particles* pParticles,
                                            CSR_Particle*  pParticle,
                                            float          elapsedTime);

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
        * Called when a texture should be deleted
        *@param pTexture - texture to delete
        */
        void OnDeleteTexture(const CSR_Texture* pTexture);

        /**
        * Called while application is idle
        *@param pSender - event sender
        *@param[in, out] done - if true, event is done and will no longer be called
        */
        void __fastcall OnIdle(TObject* pSender, bool& done);
};
extern PACKAGE TMainForm* MainForm;
#endif
