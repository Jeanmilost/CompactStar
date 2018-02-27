#ifndef MainH
#define MainH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>

// compactStart engine
#include "CSR_Geometry.h"
#include "CSR_Vertex.h"
#include "CSR_Model.h"
#include "CSR_SoftwareRaster.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>

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

        void __fastcall FormShow(TObject* pSender);

    public:
        __fastcall TMainForm(TComponent* pOwner);

        virtual __fastcall ~TMainForm();

    private:
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
        unsigned __int64 m_StartTime;
        unsigned __int64 m_PreviousTime;
        bool             m_Initialized;

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

        static void OnTextureReadCallback(std::size_t index, const CSR_PixelBuffer* pPixelBuffer);

        static void OnApplyFragmentShaderCallback(const CSR_Matrix4*  pMatrix,
                                                  const CSR_Polygon3* pPolygon,
                                                  const CSR_Vector2*  pST,
                                                  const CSR_Vector3*  pSampler,
                                                        float         z,
                                                        CSR_Color*    pColor);

        void OnTextureRead(std::size_t index, const CSR_PixelBuffer* pPixelBuffer);

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
