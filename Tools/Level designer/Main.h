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
    TLabel *la1;

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
        *@param[in, out] pNoGPU - if 1, the texture will not be loaded on the GPU while model is loading
        */
        static void OnTextureReadCallback(      std::size_t      index,
                                          const CSR_PixelBuffer* pPixelBuffer,
                                                int*             pNoGPU);

        /**
        * Called when a model should receive a texture index to save
        *@param pModel - model for which the texture index should be get
        *@param index - model texture index, in case the model contains several textures
        *@param bumpMap - if 1, the needed texture should be a bump map, normal texture if 0
        *@return texture index from a referenced texture list, if -1 no texture will be linked
        */
        static int OnGetTextureIndexCallback(const void* pModel, size_t index, int bumpMap);

        /**
        * Called when a model should receive a shader index to save
        *@param pModel - model for which the shader index should be get
        *@return shader index from a referenced shader list, if -1 no shader will be linked
        */
        static int OnGetShaderIndexCallback(const void* pModel);

    private:
        typedef std::vector<CSR_Shader*>  IShaders;
        typedef std::vector<CSR_Matrix4*> IMatrices;

        CSR_OpenGLHelper                    m_OpenGLHelper;
        std::unique_ptr<CSR_VCLControlHook> m_pDesignerViewXHook;
        std::unique_ptr<CSR_VCLControlHook> m_pDesignerViewYHook;
        std::unique_ptr<CSR_VCLControlHook> m_pDesignerViewZHook;
        std::unique_ptr<CSR_VCLControlHook> m_pDesigner3DViewHook;
        std::unique_ptr<CSR_VCLControlHook> m_p3DViewHook;
        CSR_Scene*                          m_pScene;
        CSR_SceneContext                    m_SceneContext;
        IShaders                            m_Shaders;
        IMatrices                           m_Matrices;
        CSR_Shader*                         m_pCurrentShader;
        CSR_Matrix4*                        m_pCurrentMatrix;
        void*                               m_pLoadingModel;
        CSR_Buffer*                         m_pLoadingTexture;
        CSR_Matrix4                         m_ModelMatrix;
        CSR_Ray3                            m_Ray;
        unsigned __int64                    m_PreviousTime;
        TWndMethod                          m_fViewWndProc_Backup;

        //REM TODEL
        CSR_Vector3 MousePosToViewportPos(const TPoint& mousePos, const CSR_Rect& viewRect);

        /**
        * Calculates the ray starting from the mouse in the viewport coordinate system
        */
        void CalculateMouseRay();

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
        *@param[in, out] pNoGPU - if 1, the texture will not be loaded on the GPU while model is loading
        */
        void OnTextureRead(std::size_t index, const CSR_PixelBuffer* pPixelBuffer, int* pNoGPU);

        /**
        * Called when a model should receive a texture index to save
        *@param pModel - model for which the texture index should be get
        *@param index - model texture index, in case the model contains several textures
        *@param bumpMap - if 1, the needed texture should be a bump map, normal texture if 0
        *@return texture index from a referenced texture list, if -1 no texture will be linked
        */
        int OnGetTextureIndex(const void* pModel, size_t index, int bumpMap);

        /**
        * Called when a model should receive a shader index to save
        *@param pModel - model for which the shader index should be get
        *@return shader index from a referenced shader list, if -1 no shader will be linked
        */
        int OnGetShaderIndex(const void* pModel);

        /**
        * Called while application is idle
        *@param pSender - event sender
        *@param[in, out] done - if true, event is done and will no longer be called
        */
        void __fastcall OnIdle(TObject* pSender, bool& done);
};
extern PACKAGE TMainForm* MainForm;
#endif
