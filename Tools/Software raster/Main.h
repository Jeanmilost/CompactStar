/****************************************************************************
 * ==> Software rasterizer -------------------------------------------------*
 ****************************************************************************
 * Description : This is a tool showing how to load and draw a model using  *
 *               the software rasterizer                                    *
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
#include <Vcl.Dialogs.hpp>

// std
#include <string>

// compactStart engine
#include "CSR_Geometry.h"
#include "CSR_Collision.h"
#include "CSR_Vertex.h"
#include "CSR_Model.h"
#include "CSR_Mdl.h"
#include "CSR_SoftwareRaster.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>

/**
* Software rasterizer tool main form
*@author Jean-Milost Reymond
*/
class TMainForm : public TForm
{
    __published:
        TPanel *paView;
        TSplitter *spMainView;
        TPanel *paControls;
        TLabel *laOptionsCaption;
        TBevel *blOptionsSeparator;
        TBevel *blFilesSeparator;
        TLabel *laFilesCaption;
        TBevel *blStatsSeparator;
        TLabel *laStatsCaption;
        TLabel *laFPS;
        TLabel *laPolygonCount;
        TLabel *laRotationSpeedCaption;
        TLabel *laAnimationSpeedCaption;
        TLabel *laAnimationNbCaption;
        TLabel *laModelDistance;
        TButton *btLoadModel;
        TTrackBar *tbRotationSpeed;
        TTrackBar *tbAnimationSpeed;
        TCheckBox *ckPauseModelAnimation;
        TTrackBar *tbAnimationNb;
        TTrackBar *tbModelDistance;
        TOpenDialog *odOpen;

        void __fastcall FormShow(TObject* pSender);
        void __fastcall FormResize(TObject* pSender);
        void __fastcall FormMouseWheel(TObject* pSender, TShiftState shift, int wheelDelta,
                TPoint& mousePos, bool& handled);
        void __fastcall btLoadModelClick(TObject* pSender);

    public:
        __fastcall TMainForm(TComponent* pOwner);

        virtual __fastcall ~TMainForm();

    private:
        /**
        * Statistics
        */
        struct IStats
        {
            std::size_t m_FPS;

            IStats();
            ~IStats();

            /**
            * Clears the stats
            */
            void Clear();
        };

        IStats           m_Stats;
        CSR_Raster       m_Raster;
        CSR_Matrix4      m_ProjectionMatrix;
        CSR_Matrix4      m_Matrix;
        CSR_MDL*         m_pModel;
        CSR_PixelBuffer* m_pModelTexture;
        CSR_FrameBuffer* m_pFrameBuffer;
        CSR_DepthBuffer* m_pDepthBuffer;
        float            m_zNear;
        float            m_zFar;
        float            m_PosY;
        float            m_AngleX;
        float            m_AngleY;
        double           m_pTextureLastTime;
        double           m_pModelLastTime;
        double           m_pMeshLastTime;
        std::size_t      m_TextureIndex;
        std::size_t      m_ModelIndex;
        std::size_t      m_MeshIndex;
        std::size_t      m_FrameCount;
        unsigned __int64 m_StartTime;
        unsigned __int64 m_PreviousTime;
        bool             m_Initialized;

        /**
        * Gets the models directory
        *@return the model directory, empty string on error
        */
        UnicodeString GetModelsDir() const;

        /**
        * Loads a model
        *@param fileName - model file name to load
        *@return true on success, otherwise false
        */
        bool LoadModel(const std::string& fileName);

        /**
        * Creates the viewport
        *@param w - scene width
        *@param h - scene height
        */
        void CreateViewport(int w, int h);

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
        * Shows the stats
        */
        void ShowStats() const;

        /**
        * Calculates the model y position from his bounding box
        *@param pTree - tree containing the bounding box to use to calculate the y position
        *@param rotated - if true, the model is shown rotated 90° on the x axis
        *@return the y position
        */
        float CalculateYPos(const CSR_AABBNode* pTree, bool rotated) const;

        /**
        * Called when a skin should be applied to a model
        *@param index - skin index (in case the model contains several skins)
        *@param pSkin - skin
        *@param[in, out] pCanRelease - if 1, the skin content may be released after the skin is applied
        */
        static void OnApplySkinCallback(size_t index, const CSR_Skin* pSkin, int* pCanRelease);

        /**
        * Called from rasterizer engine when the fragment shader should be applied
        *@param pMatrix - transformation matrix
        *@param pPolygon - polygon at which the pixel belongs
        *@param pST - s and t texture coordinates
        *@param pSampler - fragment sampler containing the barycentric coordinates of the pixel
        *@param z - pixel z depth
        *@param[out] pColor - pixel color to write in the frame buffer
        */
        static void OnApplyFragmentShaderCallback(const CSR_Matrix4*  pMatrix,
                                                  const CSR_Polygon3* pPolygon,
                                                  const CSR_Vector2*  pST,
                                                  const CSR_Vector3*  pSampler,
                                                        float         z,
                                                        CSR_Color*    pColor);

        /**
        * Called when a skin should be applied to a model
        *@param index - skin index (in case the model contains several skins)
        *@param pSkin - skin
        *@param[in, out] pCanRelease - if 1, the skin content may be released after the skin is applied
        */
        void OnApplySkin(size_t index, const CSR_Skin* pSkin, int* pCanRelease);

        /**
        * Called when the fragment shader should be applied
        *@param pMatrix - transformation matrix
        *@param pPolygon - polygon at which the pixel belongs
        *@param pST - s and t texture coordinates
        *@param pSampler - fragment sampler containing the barycentric coordinates of the pixel
        *@param z - pixel z depth
        *@param[out] pColor - pixel color to write in the frame buffer
        */
        void OnApplyFragmentShader(const CSR_Matrix4*  pMatrix,
                                   const CSR_Polygon3* pPolygon,
                                   const CSR_Vector2*  pST,
                                   const CSR_Vector3*  pSampler,
                                         float         z,
                                         CSR_Color*    pColor);

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
extern PACKAGE TMainForm *MainForm;
#endif
