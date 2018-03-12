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

    private:
        typedef std::vector<CSR_Shader*> IShaders;

        CSR_OpenGLHelper                    m_OpenGLHelper;
        std::unique_ptr<CSR_VCLControlHook> m_pDesignerViewXHook;
        std::unique_ptr<CSR_VCLControlHook> m_pDesignerViewYHook;
        std::unique_ptr<CSR_VCLControlHook> m_pDesignerViewZHook;
        std::unique_ptr<CSR_VCLControlHook> m_pDesigner3DViewHook;
        std::unique_ptr<CSR_VCLControlHook> m_p3DViewHook;
        CSR_Scene*                          m_pScene;
        IShaders                            m_Shaders;
        CSR_Shader*                         m_pCurrentShader;
        CSR_Mesh*                           m_pSphere;
        CSR_Mesh*                           m_pBox;
        CSR_AABBNode*                       m_pAABBTree;
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
        * Called while application is idle
        *@param pSender - event sender
        *@param[in, out] done - if true, event is done and will no longer be called
        */
        void __fastcall OnIdle(TObject* pSender, bool& done);
};
extern PACKAGE TMainForm* MainForm;
#endif
