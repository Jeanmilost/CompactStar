#ifndef MainH
#define MainH

// vcl
#include <System.Classes.hpp>
#include <System.Actions.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ToolWin.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.ImgList.hpp>

// std
#include <memory>
#include <vector>
#include <map>

// compactStar engine
#include "CSR_Model.h"
#include "CSR_Collision.h"
#include "CSR_Renderer.h"
#include "CSR_Shader.h"
#include "CSR_Serializer.h"

// classes
#include "CSR_OpenGLHelper.h"
#include "CSR_VCLHelper.h"
#include "CSR_VCLControlHook.h"

class TMainForm : public TForm
{
    __published:
        TMainMenu *mmMenu;
        TMenuItem *miViews;
        TMenuItem *mi3Dview;
        TActionList *alActions;
        TAction *ac3DView;
        TImageList *ilToolbox;
        TPanel *paToolbox;
        TPanel *paInspector;
        TSplitter *spInspector;
        TPanel *paViews;
        TPageControl *pcMain;
        TTabSheet *ts3DView;
        TPanel *pa3DView;
        TTabSheet *tsDesigner;
        TPanel *paDesignerTop;
        TSplitter *spDesignerTopBottom;
        TPanel *paDesignerBottom;
        TPanel *paDesignerXView;
        TSplitter *spDesignerXY;
        TPanel *paDesignerYView;
        TPanel *paDesignerZView;
        TPanel *paDesigner3DView;
        TSplitter *spDesignerZ3D;

        void __fastcall FormShow(TObject* pSender);
        void __fastcall FormResize(TObject* pSender);
        void __fastcall OnSplitterMoved(TObject* pSender);

    public:
        __fastcall TMainForm(TComponent* pOwner);
        virtual __fastcall ~TMainForm();

    protected:
        /**
        * Called when a Windows message is sent to a designer view
        *@param pControl - hooked designer view control
        *@param message - Windows message
        *@param fCtrlOriginalProc - control original Windows procedure
        *@return true if the message was resolved and should no longer be handled, otherwise false
        */
        bool OnDesignerViewMessage(TControl* pControl, TMessage& message, TWndMethod fCtrlOriginalProc);

        /**
        * Called when a Windows message is sent to a 3d view
        *@param pControl - hooked designer view control
        *@param message - Windows message
        *@param fCtrlOriginalProc - control original Windows procedure
        *@return true if the message was resolved and should no longer be handled, otherwise false
        */
        bool On3DViewMessage(TControl* pControl, TMessage& message, TWndMethod fCtrlOriginalProc);

        /**
        * Called when a shader should be get for a model
        *@param pModel - model for which the shader shoudl be get
        *@param type - model type
        *@return shader to use to draw the model, 0 if no shader
        *@note The model will not be drawn if no shader is returned
        */
        static CSR_Shader* OnGetShaderCallback(const void* pModel, CSR_EModelType type);

        /**
        * Called when a texture was read in a model
        *@param index - texture index in the model
        *@param pPixelBuffer - pixel buffer containing the read texture
        */
        static void OnTextureReadCallback(size_t index, const CSR_PixelBuffer* pPixelBuffer);

        /**
        * Called when a texture should be serialized
        *@param pModel - model at which the texture belongs
        *@param index - texture index in the model
        *@param[in, out] pBuffer - buffer containing the texture to write
        *@return 1 if a texture is available to be written, otherwise 0
        */
        static int OnGetTextureCallback(void* pModel, size_t index, CSR_Buffer* pBuffer);

        /**
        * Called when a bumpmap should be serialized
        *@param pModel - model at which the bumpmap belongs
        *@param index - bumpmap index in the model
        *@param[in, out] pBuffer - buffer containing the bumpmap to write
        *@return 1 if a bumpmap is available to be written, otherwise 0
        */
        static int OnGetBumpMapCallback(void* pModel, size_t index, CSR_Buffer* pBuffer);

    private:
        typedef std::vector<CSR_Shader*> IShaders;

        CSR_OpenGLHelper                    m_OpenGLHelper;
        std::unique_ptr<CSR_VCLControlHook> m_pDesignerViewXHook;
        std::unique_ptr<CSR_VCLControlHook> m_pDesignerViewYHook;
        std::unique_ptr<CSR_VCLControlHook> m_pDesignerViewZHook;
        std::unique_ptr<CSR_VCLControlHook> m_pDesigner3DViewHook;
        std::unique_ptr<CSR_VCLControlHook> m_p3DViewHook;
        CSR_Scene*                          m_pScene;
        CSR_SceneContext                    m_SceneContext;
        IShaders                            m_Shaders;
        CSR_Shader*                         m_pCurrentShader;
        void*                               m_pLoadingModel;
        CSR_Buffer*                         m_pLoadingTexture;
        CSR_Matrix4                         m_ModelMatrix;
        unsigned __int64                    m_PreviousTime;
        TWndMethod                          m_fViewWndProc_Backup;

        /**
        * Initializes the scene
        */
        void InitScene();

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
        CSR_Shader* OnGetShader(const void* pModel, CSR_EModelType type);

        /**
        * Called when a texture was read in a model
        *@param index - texture index in the model
        *@param pPixelBuffer - pixel buffer containing the read texture
        */
        void OnTextureRead(size_t index, const CSR_PixelBuffer* pPixelBuffer);

        /**
        * Called when a texture should be serialized
        *@param pModel - model at which the texture belongs
        *@param index - texture index in the model
        *@param[in, out] pBuffer - buffer containing the texture to write
        *@return 1 if a texture is available to be written, otherwise 0
        */
        int OnGetTexture(void* pModel, size_t index, CSR_Buffer* pBuffer);

        /**
        * Called when a bumpmap should be serialized
        *@param pModel - model at which the bumpmap belongs
        *@param index - bumpmap index in the model
        *@param[in, out] pBuffer - buffer containing the bumpmap to write
        *@return 1 if a bumpmap is available to be written, otherwise 0
        */
        int OnGetBumpMap(void* pModel, size_t index, CSR_Buffer* pBuffer);

        /**
        * Called while application is idle
        *@param pSender - event sender
        *@param[in, out] done - if true, event is done and will no longer be called
        */
        void __fastcall OnIdle(TObject* pSender, bool& done);
};
extern PACKAGE TMainForm* MainForm;
#endif
