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

// std
#include <vector>
#include <string>

// compactStar engine
#include "CSR_Model.h"
#include "CSR_Collision.h"
#include "CSR_Shader.h"
#include "CSR_Renderer.h"

/**
* Ground collision tool main form
*@author Jean-Milost Reymond
*/
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
        TBevel *blStatsSeparator;
        TLabel *laStatsCaption;
        TCheckBox *ckShowCollidingBoxesOnly;
        TCheckBox *ckShowBoxes;
        TCheckBox *ckShowCollidingPolygons;
        TLabel *laHitBoxes;
        TLabel *laHitPolygons;
        TLabel *laPolygonsToCheck;
        TLabel *laMaxPolyToCheck;
        TLabel *laFPS;
        TLabel *laPolygonCount;
        TLabel *laRotationSpeedCaption;
        TTrackBar *tbRotationSpeed;
        TLabel *laAnimationSpeedCaption;
        TTrackBar *tbAnimationSpeed;
        TCheckBox *ckPauseModelAnimation;
        TLabel *laAnimationNbCaption;
        TTrackBar *tbAnimationNb;
        TLabel *laModelDistance;
        TTrackBar *tbModelDistance;
        TCheckBox *ckAntialiasing;
        TApplicationEvents *aeEvents;

        void __fastcall FormCreate(TObject* pSender);
        void __fastcall FormShow(TObject* pSender);
        void __fastcall FormResize(TObject* pSender);
        void __fastcall FormMouseWheel(TObject* pSender, TShiftState shift, int wheelDelta,
                TPoint& mousePos, bool& handled);
        void __fastcall spMainViewMoved(TObject* pSender);
        void __fastcall btLoadModelClick(TObject* pSender);
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

    protected:
        /**
        * View panel main procedure
        *@param message- Windows procedure message
        */
        void __fastcall ViewWndProc(TMessage& message);

    private:
        /**
        * Tree statistics
        */
//REM
//        struct ITreeStats
//        {
//            std::size_t m_PolyToCheckCount;
//            std::size_t m_MaxPolyToCheckCount;
//            std::size_t m_HitBoxCount;
//            std::size_t m_HitPolygonCount;
//            std::size_t m_FPS;
//
//            ITreeStats();
//            ~ITreeStats();
//
//            /**
//            * Clears the stats
//            */
//            void Clear();
//        };

        //REM typedef std::vector<CSR_AABBNode*> IAABBTrees;

        HDC              m_hDC;
        HGLRC            m_hRC;
        CSR_Color        m_Background;
        CSR_Shader*      m_pShader;
        CSR_Model*       m_pModel;
        CSR_AABBNode*    m_pTree;
        CSR_MSAA*        m_pMSAA;
        CSR_Matrix4      m_ProjectionMatrix;
        CSR_Matrix4      m_ViewMatrix;
        CSR_Matrix4      m_ModelMatrix;
        CSR_Sphere       m_BoundingSphere;
        float            m_Angle;
        float            m_PosVelocity;
        float            m_DirVelocity;
        std::string      m_SceneDir;
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
        * Clears all the previous models and meshes
        */
        //REM void ClearModelsAndMeshes();

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
        * Calculates a matrix where to put the point of view to lie on the ground
        *@param pBoundingSphere - sphere surrounding the point of view
        *@param pTree - ground model aligned-axis bounding box tree
        *@param[out] pMatrix - resulting view matrix
        */
        void ApplyGroundCollision(const CSR_Sphere*   pBoundingSphere,
                                  const CSR_AABBNode* pTree,
                                        CSR_Matrix4*  pMatrix) const;

        /**
        * Draws the scene
        */
        void DrawScene();

        /**
        * Resolves the AABB tree against the mouse ray and draw the polygons intersecting the ray
        */
        //REM void ResolveTreeAndDrawPolygons();

        /**
        * Draws the AABB tree boxes
        *@param pTree - root tree node, or parent node from which the boxes should be drawn
        */
        //REM void DrawTreeBoxes(const CSR_AABBNode* pTree);

        /**
        * Converts a mouse position to a viewport coordinate
        *@param mousePos - mouse position to convert
        *@param viewRect - viewport rectangle
        *@return the viewport coordinate (before the transformation to put it in the 3D world)
        */
        //REM CSR_Vector3 MousePosToViewportPos(const TPoint& mousePos, const CSR_Rect& viewRect);

        /**
        * Calculates and updates the ray coordinate in the 3D viewport
        */
        //REM void CalculateMouseRay();

        /**
        * Get AABB tree index to use
        *@return AABB tree index
        */
        //REM std::size_t GetAABBTreeIndex() const;

        /**
        * Shows the stats
        */
        //REM void ShowStats() const;

        /**
        * Calculates the model y position from his bounding box
        *@param pTree - tree containing the bounding box to use to calculate the y position
        *@param rotated - if true, the model is shown rotated 90° on the x axis
        *@return the y position
        */
        //REM float CalculateYPos(const CSR_AABBNode* pTree, bool rotated) const;

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

        bool SetTexture(      int            width,
                              int            height,
                              int            pixelType,
                              bool           bumpMap,
                        const unsigned char* pPixels) const;
};
extern PACKAGE TMainForm* MainForm;
#endif
