/****************************************************************************
 * ==> AABB Tree tool ------------------------------------------------------*
 ****************************************************************************
 * Description : Tool to visualize the Aligned-Axis Bounding Box around     *
 *               shapes or models                                           *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2019, this file is part of the CompactStar Engine.  *
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

// std
#include <vector>
#include <string>

// compactStar engine
#include "CSR_Model.h"
#include "CSR_Mdl.h"
#include "CSR_Collision.h"
#include "CSR_Renderer_OpenGL.h"

// classes
#include "CSR_OpenGLHelper.h"

/**
* AABB Tree tool main form
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

        void __fastcall FormCreate(TObject* pSender);
        void __fastcall FormShow(TObject* pSender);
        void __fastcall FormResize(TObject* pSender);
        void __fastcall FormMouseWheel(TObject* pSender, TShiftState shift, int wheelDelta,
                TPoint& mousePos, bool& handled);
        void __fastcall spMainViewMoved(TObject* pSender);
        void __fastcall btLoadModelClick(TObject* pSender);

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
        * Called when a skin should be applied to a model
        *@param index - skin index (in case the model contains several skins)
        *@param pSkin - skin
        *@param[in, out] pCanRelease - if 1, the skin content may be released after the skin is applied
        */
        static void OnApplySkinCallback(size_t index, const CSR_Skin* pSkin, int* pCanRelease);

        /**
        * Called when a resource identifier should be get from a key
        *@param pKey - key for which the resource identifier should be get
        *@return identifier, 0 on error or if not found
        */
        static void* OnGetIDCallback(const void* pKey);

        /**
        * Called when a texture should be deleted
        *@param pTexture - texture to delete
        */
        static void OnDeleteTextureCallback(const CSR_Texture* pTexture);

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
        struct ITreeStats
        {
            std::size_t m_PolyToCheckCount;
            std::size_t m_MaxPolyToCheckCount;
            std::size_t m_HitBoxCount;
            std::size_t m_HitPolygonCount;
            std::size_t m_FPS;

            ITreeStats();
            ~ITreeStats();

            /**
            * Clears the stats
            */
            void Clear();
        };

        typedef std::vector<CSR_AABBNode*> IAABBTrees;

        HDC                          m_hDC;
        HGLRC                        m_hRC;
        ITreeStats                   m_Stats;
        CSR_Color                    m_Background;
        CSR_OpenGLHelper::IResources m_OpenGLResources;
        CSR_OpenGLShader*            m_pShader_ColoredMesh;
        CSR_OpenGLShader*            m_pShader_TexturedMesh;
        CSR_OpenGLMSAA*              m_pMSAA;
        CSR_Mesh*                    m_pBoxMesh;
        CSR_Mesh*                    m_pMesh;
        CSR_MDL*                     m_pMDL;
        IAABBTrees                   m_AABBTrees;
        CSR_Matrix4                  m_ProjectionMatrix;
        CSR_Matrix4                  m_ViewMatrix;
        CSR_Matrix4                  m_ModelMatrix;
        CSR_Ray3                     m_Ray;
        std::wstring                 m_LastSelectedFile;
        int                          m_LastSelectedModel;
        float                        m_PosY;
        float                        m_AngleX;
        float                        m_AngleY;
        float                        m_PolygonArray[21];
        double                       m_pTextureLastTime;
        double                       m_pModelLastTime;
        double                       m_pMeshLastTime;
        std::size_t                  m_TextureIndex;
        std::size_t                  m_ModelIndex;
        std::size_t                  m_MeshIndex;
        std::size_t                  m_FrameCount;
        unsigned __int64             m_StartTime;
        unsigned __int64             m_PreviousTime;
        bool                         m_Initialized;
        TWndMethod                   m_fViewWndProc_Backup;

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
        void ClearModelsAndMeshes();

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
        * Resolves the AABB tree against the mouse ray and draw the polygons intersecting the ray
        */
        void ResolveTreeAndDrawPolygons();

        /**
        * Draws the AABB tree boxes
        *@param pTree - root tree node, or parent node from which the boxes should be drawn
        */
        void DrawTreeBoxes(const CSR_AABBNode* pTree);

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
        * Get AABB tree index to use
        *@return AABB tree index
        */
        std::size_t GetAABBTreeIndex() const;

        /**
        * Shows the stats
        */
        void ShowStats() const;

        /**
        * Calculates the model y position from his bounding box
        *@param pTree - tree containing the bounding box to use to calculate the y position
        *@param rotated - if true, the model is shown rotated 90� on the x axis
        *@return the y position
        */
        float CalculateYPos(const CSR_AABBNode* pTree, bool rotated) const;

        /**
        * Called when a skin should be applied to a model
        *@param index - skin index (in case the model contains several skins)
        *@param pSkin - skin
        *@param[in, out] pCanRelease - if 1, the skin content may be released after the skin is applied
        */
        void OnApplySkin(size_t index, const CSR_Skin* pSkin, int* pCanRelease);

        /**
        * Called when a resource identifier should be get from a key
        *@param pKey - key for which the resource identifier should be get
        *@return identifier, 0 on error or if not found
        */
        void* OnGetID(const void* pKey);

        /**
        * Called when a texture should be deleted
        *@param pTexture - texture to delete
        */
        void OnDeleteTexture(const CSR_Texture* pTexture);

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
