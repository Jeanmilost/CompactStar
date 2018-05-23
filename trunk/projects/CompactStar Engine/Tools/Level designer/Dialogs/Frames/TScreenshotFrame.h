/*****************************************************************************
 * ==> TScreenshotFrame -----------------------------------------------------*
 *****************************************************************************
 * Description : This module contains a frame that allows the user to create *
 *               a screenshot from a model                                   *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#ifndef TScreenshotFrameH
#define TScreenshotFrameH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.AppEvnts.hpp>
#include <Vcl.Menus.hpp>

// compactStar engine
#include "CSR_Collision.h"
#include "CSR_Model.h"
#include "CSR_Scene.h"

// classes
#include "CSR_DesignerHelper.h"
#include "CSR_OpenGLHelper.h"

/**
* Frame allowing to create a screenshot from a model
*@author Jean-Milost Reymond
*/
class TScreenshotFrame : public TFrame
{
    __published:
        TPanel *paBackground;
        TPanel *paLeft;
        TImage *imScreenshot;
        TPanel *paRight;
        TPanel *paColor;
        TLabel *laColorCaption;
        TPanel *paColorValue;
        TColorDialog *cdColor;
        TPanel *paCameraType;
        TRadioButton *rbArcball;
        TRadioButton *rbFirstViewPerson;
        TBevel *blSeparator;
        TLabel *laCameraTitle;
        TPanel *paCameraCaption;
        TPanel *paCameraUp;
        TPanel *paCameraDown;
        TSpeedButton *btCameraLeft;
        TSpeedButton *btCameraUp;
        TSpeedButton *btCameraBack;
        TSpeedButton *btCameraRight;
        TSpeedButton *btCameraDown;
        TSpeedButton *btCameraFront;
        TLabel *laCameraX;
        TLabel *laCameraY;
        TLabel *laCameraZ;
        TPanel *paCamera;
        TPanel *paCameraTitle;
        TSpeedButton *btConfig;
        TPopupMenu *pmConfig;
        TMenuItem *miResetScene;

        void __fastcall FrameResize(TObject* pSender);
        void __fastcall btCameraLeftClick(TObject* pSender);
        void __fastcall btCameraRightClick(TObject* pSender);
        void __fastcall btCameraUpClick(TObject* pSender);
        void __fastcall btCameraDownClick(TObject* pSender);
        void __fastcall btCameraBackClick(TObject* pSender);
        void __fastcall btCameraFrontClick(TObject* pSender);
        void __fastcall btConfigClick(TObject* pSender);
        void __fastcall miResetSceneClick(TObject* pSender);
        void __fastcall rbArcballClick(TObject* pSender);
        void __fastcall rbFirstViewPersonClick(TObject* pSender);
        void __fastcall paColorValueClick(TObject* pSender);

    public:
        /**
        * Constructor
        *@param pOwner - frame owner
        */
        __fastcall TScreenshotFrame(TComponent* pOwner);

        __fastcall ~TScreenshotFrame();

        /**
        * Enables or disables the frame
        *@param value - if true, the frame is enabled, disabled otherwise
        */
        void Enable(bool value);

        /**
        * Loads the model for which the screenshot should be created
        *@param type - model type to create
        *@param fileName - model file name to load, ignored if model type isn't set to IE_MT_Model
        *@param textureFileName - model texture file name, ignored if empty
        *@param bumpMapFileName - model bump map file name, ignored if empty
        *@param color - vertex color
        *@param landscapeHeight - landscape height
        *@param landscapeFactor - landscape scale factor
        *@return true on success, otherwise false
        */
        bool LoadModel(      CSR_DesignerHelper::IEModelType type,
                       const std::wstring&                   fileName,
                       const std::wstring&                   textureFileName,
                       const std::wstring&                   bumpMapFileName,
                             unsigned                        color,
                             float                           landscapeHeight,
                             float                           landscapeFactor);

        /**
        * Gets a screenshot of the model belonging to the item
        *@param pBitmap - bitmap in which the screenshot should be drawn
        *@return true on success, otherwise false
        */
        bool GetScreenshot(TBitmap* pBitmap) const;

    private:
        CSR_OpenGLHelper::IContext m_SceneContext;
        TForm*                     m_pSceneOverlayForm;
        CSR_Shader*                m_pSceneShader;
        CSR_Color                  m_SceneColor;
        CSR_Mesh*                  m_pMesh;
        CSR_Model*                 m_pModel;
        CSR_MDL*                   m_pMDL;
        CSR_AABBNode*              m_pAABBTree;
        CSR_ArcBall                m_ArcBall;
        CSR_Camera                 m_Camera;
        CSR_Matrix4                m_ModelMatrix;
        int                        m_AntialiasingFactor;
        float                      m_Offset;
        float                      m_ArcBallOffset;
        bool                       m_Reseting;

        /**
        * Creates an offscreen scene in which the model can be drawn to be exported as a screenshot
        *@param type - model type to export as a screenshot
        *@param width - screenshot width
        *@param height - screenshot height
        *@param color - vertex color
        *@return true on success, otherwise false
        */
        bool CreateScene(CSR_DesignerHelper::IEModelType type,
                         int                             width,
                         int                             height,
                         unsigned                        color);

        /**
        * Creates an offscreen scene in which the model can be drawn to be exported as a screenshot
        *@param fileName - model file name to export as a screenshot
        *@param width - screenshot width
        *@param height - screenshot height
        *@param color - vertex color
        *@return true on success, otherwise false
        */
        bool CreateScene(const std::string& fileName,
                               int          width,
                               int          height,
                               unsigned     color);

        /**
        * Creates an offscreen scene in which a landscape can be drawn to be exported as a screenshot
        *@param pBitmap - grayscale bitmap from which the landscape will be generated
        *@param landscapeHeight - landscape height
        *@param landscapeFactor - landscape scale factor
        *@param color - vertex color
        *@return true on success, otherwise false
        */
        bool CreateScene(const CSR_PixelBuffer* pBitmap,
                               int              width,
                               int              height,
                               float            landscapeHeight,
                               float            landscapeFactor,
                               unsigned         color);

        /**
        * Resets the scene to his default value
        */
        void ResetScene();

        /**
        * Initializes a screenshot scene
        *@param width - screenshot width
        *@param height - screenshot height
        *@return true on success, otherwise false
        */
        bool InitializeScene(int width, int height);

        /**
        * Releases a previously created screenshot scene
        */
        void ReleaseScene();

        /**
        * Draws a screenshot scene in a bitmap
        *@param pBitmap - bitmap in which the screenshot should be drawn
        *@return true on success, otherwise false
        */
        bool DrawScene(TBitmap* pBitmap) const;

        /**
        * Draws a screenshot scene and show it in the preview image
        *@return true on success, otherwise false
        */
        bool DrawScene() const;

        /**
        * Sets a texture for the currently opened model
        *@param width - texture width in pixels
        *@param height - texture height in pixels
        *@param pixelType - pixel type to use
        *@param bumpMap - if true, the texture to set is a bumpmap
        *@param pPixels - pixel array
        *@return true on success, otherwise false
        */
        bool SetTexture(      int            width,
                              int            height,
                              int            pixelType,
                              bool           bumpMap,
                        const unsigned char* pPixels) const;
};
extern PACKAGE TScreenshotFrame* ScreenshotFrame;
#endif
