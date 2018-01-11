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

class TMainForm : public TForm
{
    __published:
        TPanel *paView;
        TPanel *paControls;
        TSplitter *spMainView;
        TLabel *laTreeOptionsCaption;
        TCheckBox *ckWireFrame;
        TCheckBox *ckShowLeafOnly;
        TTrackBar *tbTransparency;
        TLabel *laTransparency;
        TButton *btLoadModel;
        TBevel *blTreeControlSeparator;
        TBevel *blFilesSeparator;
        TLabel *laFilesCaption;
        TButton *btSaveTree;

        void __fastcall FormCreate(TObject* pSender);
        void __fastcall FormShow(TObject* pSender);
        void __fastcall FormResize(TObject* pSender);
        void __fastcall spMainViewMoved(TObject* pSender);

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
        struct ITreeStats
        {
            std::size_t m_HitBoxCount;

            ITreeStats();
            ~ITreeStats();

            /**
            * Clears the stats
            */
            void Clear();
        };

        HDC              m_hDC;
        HGLRC            m_hRC;
        TCanvas*         m_pDocCanvas;
        ITreeStats       m_Stats;
        CSR_Shader*      m_pShader;
        CSR_Mesh*        m_pSphere;
        CSR_AABBNode*    m_pAABBTree;
        CSR_Matrix4      m_ProjectionMatrix;
        CSR_Matrix4      m_ModelMatrix;
        CSR_Ray3         m_Ray;
        float            m_AngleY;
        float            m_PolygonArray[21];
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

        void ResolveTreeAndDrawPolygons();

        void DrawTreeBoxes(const CSR_AABBNode* pTree);

        CSR_Mesh* CreateBox(const CSR_Vector3& min, const CSR_Vector3& max, unsigned color) const;

        CSR_Vector3 MousePosToViewportPos(const TPoint&   mousePos,
                                          const TRect&    clientRect,
                                          const CSR_Rect& viewRect);

        void CalculateMouseRay();

        /**
        * Called while application is idle
        *@param pSender - event sender
        *@param[in, out] done - if true, event is done and will no longer be called
        */
        void __fastcall OnIdle(TObject* pSender, bool& done);
};
extern PACKAGE TMainForm* MainForm;
#endif
