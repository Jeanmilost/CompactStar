/****************************************************************************
 * ==> Level designer for 2D games -----------------------------------------*
 ****************************************************************************
 * Description : CompactStar Engine level designer for 2D games             *
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

// classes
#include "CSR_Level.h"
#include "CSR_DesignerView.h"

// interface
#include "TSkyboxSelection.h"
#include "TVector3Frame.h"

/**
* 2D level designer main form
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
        TMenuItem *miAddSphere;
        TMenuItem *miAddSurface;
        TMenuItem *miAddCylinder;
        TMenuItem *miAddDisk;
        TMenuItem *miAddRing;
        TMenuItem *miAddSpiral;
        TMenuItem *miLandscapeAddSeparator;
        TMenuItem *miAddWaveFront;
        TMenuItem *miAddMDLModel;
        TMenuItem *miSkyBox;
        TMenuItem *miSkyboxAdd;
        TMenuItem *miFileLoad;
        TMenuItem *miFileSave;
        TMenuItem *miFileSeparator;
        TMenuItem *miFileExit;
        TOpenDialog *odLevelOpen;
        TMenuItem *miDesigner;
        TMenuItem *miDesignerPlayMode;
        TMenuItem *miDesignerRefresh;
        TMenuItem *miDesignerRefreshImmediately;
        TMenuItem *miDesignerRefreshEvery500ms;
        TMenuItem *miDesignerRefreshEvery1s;
        TMenuItem *miDesignerRefreshEvery2s;
        TMenuItem *miDesignerRefreshEvery5s;
        TMenuItem *miDesignerRefreshEvery10s;

        void __fastcall FormShow(TObject* pSender);
        void __fastcall FormResize(TObject* pSender);
        void __fastcall FormShortCut(TWMKey& msg, bool& handled);
        void __fastcall miFileNewClick(TObject* pSender);
        void __fastcall miFileLoadClick(TObject* pSender);
        void __fastcall miFileSaveClick(TObject* pSender);
        void __fastcall miFileExitClick(TObject* pSender);
        void __fastcall miDesignerPlayModeClick(TObject* pSender);
        void __fastcall miAddBoxClick(TObject* pSender);
        void __fastcall miAddCylinderClick(TObject* pSender);
        void __fastcall miAddDiskClick(TObject* pSender);
        void __fastcall miAddRingClick(TObject* pSender);
        void __fastcall miAddSphereClick(TObject* pSender);
        void __fastcall miAddSpiralClick(TObject* pSender);
        void __fastcall miAddSurfaceClick(TObject* pSender);
        void __fastcall miAddWaveFrontClick(TObject* pSender);
        void __fastcall miAddMDLModelClick(TObject* pSender);
        void __fastcall miLandscapeResetViewportClick(TObject* pSender);
        void __fastcall miSkyboxAddClick(TObject* pSender);
        void __fastcall miPostProcessingNoneClick(TObject* pSender);
        void __fastcall miPostProcessingAntialiasingClick(TObject* pSender);
        void __fastcall miPostProcessingOilPaintingClick(TObject* pSender);
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
        * Called when a cubemap texture should be loaded
        *@param fileNames - textures file names to load
        *@return texture identifier on the GPU, M_CSR_Error_Code on error
        */
        static GLuint OnLoadCubemap(const CSR_Level::IFileNames& fileNames);

        /**
        * Called when a texture should be loaded
        *@param fileName - textures file name to load
        *@return texture identifier on the GPU, M_CSR_Error_Code on error
        */
        static GLuint OnLoadTexture(const std::string& fileName);

        /**
        * Called when a model should be selected on the designer
        *@param pKey - item key to update
        *@param index - model matrix index
        */
        static void OnSelectModel(void* pKey, int index);

        /**
        * Called when the level designer should be updated
        *@param pKey - item key to update
        *@param index - model matrix index
        *@param modelLength - model length on each axis
        */
        static void OnUpdateDesigner(void* pKey, int index, const CSR_Vector3& modelLength);

        /**
        * Called when a skin should be applied to a model
        *@param index - skin index (in case the model contains several skins)
        *@param pSkin - skin
        *@param[in, out] pCanRelease - if 1, the skin content may be released after the skin is applied
        */
        static void OnApplySkin(size_t index, const CSR_Skin* pSkin, int* pCanRelease);

        /**
        * Called when a shader should be get for a model
        *@param pModel - model for which the shader shoudl be get
        *@param type - model type
        *@return shader to use to draw the model, 0 if no shader
        *@note The model will not be drawn if no shader is returned
        */
        static void* OnGetShader(const void* pModel, CSR_EModelType type);

        /**
        * Called when a resource identifier should be get from a key
        *@param pKey - key for which the resource identifier should be get
        *@return identifier, 0 on error or if not found
        */
        static void* OnGetID(const void* pKey);

        /**
        * Called when scene begins
        *@param pScene - scene to begin
        *@param pContext - scene context
        */
        static void OnSceneBegin(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

        /**
        * Called when scene ends
        *@param pScene - scene to end
        *@param pContext - scene context
        */
        static void OnSceneEnd(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

        /**
        * Called when a texture should be deleted
        *@param pTexture - texture to delete
        */
        static void OnDeleteTexture(const CSR_Texture* pTexture);

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
        * User interface available commands
        */
        enum IECommand
        {
            IE_C_None,
            IE_C_Cut,
            IE_C_Copy,
            IE_C_Paste
        };

        /**
        * User interface command like copy, cut or paste
        */
        struct ICommand
        {
            IECommand m_Command;
            void*     m_pKey;
            int       m_Index;

            ICommand();
            ~ICommand();

            /**
            * Clears the command
            */
            void Clear();
        };

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

        std::auto_ptr<CSR_Level>          m_pLevel;
        std::auto_ptr<CSR_VCLControlHook> m_pEngineViewHook;
        std::auto_ptr<CSR_DesignerView>   m_pDesignerView;
        IDesignerProperties               m_DesignerProperties;
        HDC                               m_hDC;
        HGLRC                             m_hRC;
        CSR_Sphere                        m_ViewSphere;
        std::string                       m_SceneDir;
        ICommand                          m_ProcessingCommand;
        std::size_t                       m_FrameCount;
        int                               m_PrevOrigin;
        float                             m_Angle;
        float                             m_PosVelocity;
        float                             m_DirVelocity;
        double                            m_FPS;
        unsigned __int64                  m_StartTime;
        unsigned __int64                  m_PreviousTime;
        unsigned __int64                  m_PrevDesignerTime;
        bool                              m_Initialized;

        /**
        * Opens a new landscape document
        *@return true on success, otherwise false
        */
        bool OpenDocument();

        /**
        * Closes a landscape document
        */
        void CloseDocument();

        /**
        * Processes a user interface command
        *@param command - command to process
        *@return true on success, otherwise false
        */
        bool ProcessCommand(const ICommand& command);

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
        GLuint LoadCubemap(const CSR_Level::IFileNames fileNames) const;

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
        *@param pMatrix - matrix from which the viewpoint should be initialized
        */
        void InitializeViewPoint(const CSR_Matrix4* pMatrix);

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
        * Called while application is idle
        *@param pSender - event sender
        *@param[in, out] done - if true, event is done and will no longer be called
        */
        void __fastcall OnIdle(TObject* pSender, bool& done);
};
extern PACKAGE TMainForm* MainForm;
#endif
