#ifndef MainH
#define MainH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>

// compactStar engine
#include "CSR_Model.h"
#include "CSR_Collision.h"
#include "CSR_Shader.h"
#include <Vcl.Menus.hpp>
#include <Vcl.ToolWin.hpp>
#include <System.Actions.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.ImgList.hpp>

class TMainForm : public TForm
{
    __published:
        TPageControl *pcViews;
        TTabSheet *ts3DView;
        TPanel *pa3DView;
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

        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);

    public:
        __fastcall TMainForm(TComponent* pOwner);
        virtual __fastcall ~TMainForm();

    protected:
        /**
        * View panel main procedure
        *@param message- Windows procedure message
        */
        void __fastcall ViewWndProc(TMessage& message);

    private:
        HDC              m_hDC;
        HGLRC            m_hRC;
        CSR_Shader*      m_pShader;
        CSR_Mesh*        m_pSphere;
        CSR_Mesh*        m_pBox;
        CSR_AABBNode*    m_pAABBTree;
        unsigned __int64 m_PreviousTime;
        bool             m_Initialized;
        TWndMethod       m_fViewWndProc_Backup;

        /**
        * Enables OpenGL
        *@param hWnd - Windows handle
        *@param hDC - device context
        *@param hRC - OpenGL rendering context
        */
        void EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC);

        /**
        * Disables OpenGL
        *@param hWnd - Windows handle
        *@param hDC - device context
        *@param hRC - OpenGL rendering context
        */
        void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);

        /**
        * Creates the viewport
        *@param w - viewport width
        *@param h - viewport height
        */
        void CreateViewport(float w, float h);

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
        * Called while application is idle
        *@param pSender - event sender
        *@param[in, out] done - if true, event is done and will no longer be called
        */
        void __fastcall OnIdle(TObject* pSender, bool& done);
};
extern PACKAGE TMainForm* MainForm;
#endif
