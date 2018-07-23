/****************************************************************************
 * ==> Main ----------------------------------------------------------------*
 ****************************************************************************
 * Description : Ground collision tool main form                            *
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
#include "CSR_Sound.h"

// classes
#include "CSR_VCLHelper.h"
#include "CSR_Weather_Rain.h"
#include "CSR_Weather_Snow.h"
#include "CSR_PostProcessingEffect_OilPainting.h"

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
        *@param pModel - model for which the shader should be get
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

        /**
        * Called when a motion should be calculated for a particle
        *@param pParticles - particle system at which the particle belongs
        *@param pParticle - particle for which the motion should be calculated
        *@param elapedTime - elapsed time since last animation, in milliseconds
        */
        static void OnCalculateMotionCallback(const CSR_Particles* pParticles,
                                                    CSR_Particle*  pParticle,
                                                    float          elapsedTime);

    private:
        HDC                                   m_hDC;
        HGLRC                                 m_hRC;
        ALCdevice*                            m_pOpenALDevice;
        ALCcontext*                           m_pOpenALContext;
        CSR_Scene*                            m_pScene;
        CSR_SceneContext                      m_SceneContext;
        CSR_Weather_Rain*                     m_pRain;
        CSR_Weather_Snow*                     m_pSnow;
        CSR_Sound*                            m_pRainSound;
        CSR_Sound*                            m_pWindSound;
        CSR_Shader*                           m_pShader;
        CSR_PostProcessingEffect_OilPainting* m_pEffect;
        CSR_MSAA*                             m_pMSAA;
        std::string                           m_SceneDir;
        std::size_t                           m_FrameCount;
        int                                   m_PrevOrigin;
        float                                 m_Angle;
        float                                 m_PosVelocity;
        float                                 m_DirVelocity;
        double                                m_FPS;
        unsigned __int64                      m_StartTime;
        unsigned __int64                      m_PreviousTime;
        bool                                  m_UseOilPainting;
        bool                                  m_UseMSAA;
        bool                                  m_Initialized;

        /**
        * Creates a weather effect
        *@param rain - if true, a rain effect will be created
        *@param snow - if true, a snow effect will be created
        */
        void CreateWeather(bool rain, bool snow);

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
        *@param pModel - model for which the shader should be get
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
