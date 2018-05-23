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
#include "CSR_Sound.h"

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
        TButton *btLoadModel;
        TBevel *blFilesSeparator;
        TLabel *laFilesCaption;
        TBevel *blStatsSeparator;
        TLabel *laStatsCaption;
        TLabel *laAltitude;
        TLabel *laFPS;
        TLabel *laPolygonCount;
        TApplicationEvents *aeEvents;
        TBevel *blOptions;
        TLabel *laOptions;
        TCheckBox *ckAntialiasing;
        TCheckBox *ckDisableSound;
        TButton *btResetViewport;

        void __fastcall FormCreate(TObject* pSender);
        void __fastcall FormShow(TObject* pSender);
        void __fastcall FormResize(TObject* pSender);
        void __fastcall spMainViewMoved(TObject* pSender);
        void __fastcall btResetViewportClick(TObject* pSender);
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
        * Statistics
        */
        struct IStats
        {
            float       m_Altitude;
            std::size_t m_FPS;
            std::size_t m_RefreshCounter;

            IStats();
            ~IStats();

            /**
            * Clears the stats
            */
            void Clear();
        };

        HDC              m_hDC;
        HGLRC            m_hRC;
        ALCdevice*       m_pOpenALDevice;
        ALCcontext*      m_pOpenALContext;
        IStats           m_Stats;
        CSR_Sound*       m_pSound;
        CSR_Color        m_Background;
        CSR_Shader*      m_pShader;
        CSR_Model*       m_pModel;
        CSR_AABBNode*    m_pTree;
        CSR_MSAA*        m_pMSAA;
        CSR_Matrix4      m_ProjectionMatrix;
        CSR_Matrix4      m_ViewMatrix;
        CSR_Matrix4      m_ModelMatrix;
        CSR_Sphere       m_BoundingSphere;
        std::size_t      m_FrameCount;
        float            m_Angle;
        float            m_PosVelocity;
        float            m_DirVelocity;
        float            m_StepTime;
        float            m_StepInterval;
        std::string      m_SceneDir;
        unsigned __int64 m_StartTime;
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
        * Loads a model from a file
        *@param fileName - model file name to load from
        *@return true on success, otherwise false
        */
        bool LoadModel(const std::string& fileName);

        /**
        * Loads a model from a grayscale bitmap
        *@param fileName - grayscale bitmap from which the model will be generated
        *@return true on success, otherwise false
        */
        bool LoadModelFromBitmap(const std::string& fileName);

        /**
        * Loads a texture
        *@param fileName - texture file name to load
        *@return texture identifier on the GPU, M_CSR_Error_Code on error
        */
        GLuint LoadTexture(const std::wstring& fileName);

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
        * Shows the stats
        */
        void ShowStats() const;

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