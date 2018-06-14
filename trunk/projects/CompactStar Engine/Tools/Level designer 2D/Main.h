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

// std
#include <string>
#include <vector>
#include <map>

// compactStar engine
#include "CSR_Model.h"
#include "CSR_Collision.h"
#include "CSR_Shader.h"
#include "CSR_Renderer.h"
#include "CSR_Scene.h"
#include "CSR_Sound.h"

// classes
#include "CSR_DesignerView.h"
#include "CSR_PostProcessingEffect_OilPainting.h"

// interface
#include "TVector3Frame.h"

/**
* Ground collision tool main form
*@author Jean-Milost Reymond
*/
class TMainForm : public TForm
{
    __published:
        TPanel *paEngineView;
        TPanel *paProperties;
        TSplitter *spMainView;
        TApplicationEvents *aeEvents;
        TMainMenu *mmMainMenu;
        TPanel *paDesignerView;
        TPanel *paViews;
        TSplitter *spViews;
        TMenuItem *miFile;
        TMenuItem *miFileNew;
        TMenuItem *miLandscape;
        TMenuItem *miAdd;
        TMenuItem *miAddBox;
        TMenuItem *miPostProcessing;
        TMenuItem *miSound;
        TMenuItem *miSoundOpen;
        TMenuItem *miSoundPause;
        TMenuItem *miLandscapeResetViewport;
        TMenuItem *miLandscapeSeparator;
        TMenuItem *miPostProcessingAntialiasing;
        TMenuItem *miPostProcessingOilPainting;
        TMenuItem *miPostProcessingNone;
        TCategoryPanelGroup *cgProperties;

        void __fastcall FormShow(TObject* pSender);
        void __fastcall FormResize(TObject* pSender);
        void __fastcall miFileNewClick(TObject* pSender);
        void __fastcall miAddBoxClick(TObject* pSender);
        void __fastcall miLandscapeResetViewportClick(TObject* pSender);
        void __fastcall miSoundOpenClick(TObject* pSender);
        void __fastcall miSoundPauseClick(TObject* pSender);
        void __fastcall spMainViewMoved(TObject* pSender);
        void __fastcall spViewsMoved(TObject* pSender);
        void __fastcall aeEventsMessage(tagMSG& msg, bool& handled);
        void __fastcall OnViewClick(TObject* pSender);

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

    protected:
        /**
        * Called when a Windows message is sent to the engine view
        *@param pControl - hooked designer view control
        *@param message - Windows message
        *@param fCtrlOriginalProc - control original Windows procedure
        *@return true if the message was resolved and should no longer be handled, otherwise false
        */
        bool OnEngineViewMessage(TControl* pControl, TMessage& message, TWndMethod fCtrlOriginalProc);

    private:
        /**
        * Dynamically created designer properties
        */
        struct IDesignerProperties
        {
            TVector3Frame* m_pTransformTranslate;
            TVector3Frame* m_pTransformRotate;
            TVector3Frame* m_pTransformScale;

            IDesignerProperties();
            ~IDesignerProperties();

            /**
            * Clears the designer properties content
            */
            void Clear();
        };

        /**
        * Designer item
        */
        struct IDesignerItem
        {
            CSR_Matrix4 m_Matrix;

            IDesignerItem();
            ~IDesignerItem();
        };

        // IMPORTANT NOTE don't use a CSR_SceneItem as key for the map, because this pointer may
        // change during the document lifecycle, whereas the model pointer not
        typedef std::vector<IDesignerItem*>     IDesignerItems;
        typedef std::map<void*, IDesignerItems> IDesigner;

        std::auto_ptr<CSR_VCLControlHook>     m_pEngineViewHook;
        std::auto_ptr<CSR_DesignerView>       m_pDesignerView;
        IDesignerProperties                   m_DesignerProperties;
        HDC                                   m_hDC;
        HGLRC                                 m_hRC;
        ALCdevice*                            m_pOpenALDevice;
        ALCcontext*                           m_pOpenALContext;
        CSR_Sound*                            m_pSound;
        CSR_Color                             m_Background;
        CSR_Shader*                           m_pShader;
        CSR_Scene*                            m_pScene;
        CSR_SceneContext                      m_SceneContext;
        IDesigner                             m_Designer;
        void*                                 m_pLandscapeKey;
        void*                                 m_pSelectedObjectKey;
        CSR_PostProcessingEffect_OilPainting* m_pEffect;
        CSR_MSAA*                             m_pMSAA;
        CSR_Matrix4                           m_ProjectionMatrix;
        CSR_Sphere                            m_ViewSphere;
        std::string                           m_SceneDir;
        std::size_t                           m_FrameCount;
        int                                   m_PrevOrigin;
        float                                 m_Angle;
        float                                 m_PosVelocity;
        float                                 m_DirVelocity;
        double                                m_FPS;
        unsigned __int64                      m_StartTime;
        unsigned __int64                      m_PreviousTime;
        bool                                  m_Initialized;

        /**
        * Opens a new landscape document
        */
        bool OpenDocument();

        /**
        * Closes a landscape document
        */
        void CloseDocument();

        /**
        * Loads a landscape from a file
        *@param fileName - model file name to load from
        *@return true on success, otherwise false
        */
        bool LoadLandscape(const std::string& fileName);

        /**
        * Loads a landscape from a grayscale bitmap
        *@param fileName - grayscale bitmap from which the model will be generated
        *@return true on success, otherwise false
        */
        bool LoadLandscapeFromBitmap(const std::string& fileName);

        /**
        * Loads a texture
        *@param fileName - texture file name to load
        *@return texture identifier on the GPU, M_CSR_Error_Code on error
        */
        GLuint LoadTexture(const std::wstring& fileName) const;

        /**
        * Loads a sound
        *@param fileName - sound file name to load
        *@return loaded sound on success, otherwise NULL
        *@note The returned sound should be released once useless
        */
        CSR_Sound* LoadSound(const std::string& fileName) const;

        /**
        * Clears the properties view
        */
        void ClearProperties();

        /**
        * Refreshes the properties view
        */
        void RefreshProperties();

        /**
        * Initializes the viewpoint
        */
        void InitializeViewPoint();

        /**
        * Creates the viewport
        *@param w - viewport width
        *@param h - viewport height
        */
        void CreateViewport(float w, float h);

        /**
        * Creates a default scene
        */
        void CreateScene();

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
        * Calculates a matrix where to put the point of view to lie on the ground
        *@param pBoundingSphere - sphere surrounding the point of view
        *@param[out] pMatrix - resulting view matrix
        *@return true if new position is valid, otherwise false
        */
        bool ApplyGroundCollision(const CSR_Sphere* pBoundingSphere, CSR_Matrix4* pMatrix);

        /**
        * Called when a value changed
        *@param pSender - event sender
        *@param x - x value
        *@param y - y value
        *@param z - z value
        */
        void OnPropertiesValueChanged(TObject* pSender, float x, float y, float z);

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
