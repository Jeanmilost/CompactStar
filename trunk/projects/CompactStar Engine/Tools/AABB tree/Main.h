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
        TLabel *laOptionsCaption;
        TCheckBox *ckWireFrame;
        TCheckBox *ckShowLeafOnly;
        TTrackBar *tbTransparency;
        TLabel *laTransparency;
        TButton *btLoadModel;
        TBevel *blOptionsSeparator;
        TBevel *blFilesSeparator;
        TLabel *laFilesCaption;
        TButton *btSaveTree;
        TBevel *blStatsSeparator;
        TLabel *laStatsCaption;
        TCheckBox *ckShowCollidingBoxesOnly;
        TCheckBox *ckShowBoxes;
        TCheckBox *ckShowCollidingPolygons;

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
            std::size_t m_PolyToCheckCount;
            std::size_t m_MaxPolyToCheckCount;
            std::size_t m_HitBoxCount;
            std::size_t m_HitPolygonCount;

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
        CSR_Shader*      m_pShader_ColoredMesh;
        CSR_Shader*      m_pShader_TexturedMesh;
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

        /**
        * Draws the AABB tree boxes
        *@param pTree - root tree node, or parent node from which the boxes should be drawn
        */
        void DrawTreeBoxes(const CSR_AABBNode* pTree);

        /**
        * Creates a colored box mesh, which is on the physical position specified by min and max
        *@param min - box min vertex
        *@param max - box max vertex
        *@param color - box color
        *@return mesh
        *@note The caller is responsible to delete the mesh whenever useless
        */
        CSR_Mesh* CreateBox(const CSR_Vector3& min, const CSR_Vector3& max, unsigned color) const;

        /**
        * Converts a mouse position to a viewport coordinate
        *@param mousePos - mouse position to convert
        *@param viewRect - viewport rectangle
        *@return the viewport coordinate (before the transformation to put it in the 3D world)
        */
        CSR_Vector3 MousePosToViewportPos(const TPoint& mousePos, const CSR_Rect& viewRect);

        /**
        * Calculates and updates the ray coordinate in the 3D viewport
        */
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
