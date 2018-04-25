/*****************************************************************************
 * ==> TAddItemDialog -------------------------------------------------------*
 *****************************************************************************
 * Description : This module contains a toolbox dialog to add a scene item   *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#ifndef TAddItemDialogH
#define TAddItemDialogH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include <Vcl.CheckLst.hpp>

// compactStar engine
#include "CSR_Collision.h"
#include "CSR_Model.h"

// classes
#include "CSR_OpenGLHelper.h"

// frames
#include "TTextureSelectionFrame.h"
#include "TVertexColorFrame.h"
#include "TFileFrame.h"
#include <Vcl.Dialogs.hpp>

/**
* Add an item dialog box
*@author Jean-Milost Reymond
*/
class TAddItemDialog : public TForm
{
    __published:
        TPageControl *pcWizard;
        TTabSheet *tsSelectItem;
        TTabSheet *tsConfig;
        TBevel *blSelectItemSeparator;
        TPanel *paNavigation;
        TButton *btOK;
        TButton *btCancel;
        TLabel *laSelectItemTitle;
        TSpeedButton *btSelectItemAddSurface;
        TSpeedButton *btSelectItemAddBox;
        TSpeedButton *btSelectItemAddSphere;
        TSpeedButton *btSelectItemAddCylinder;
        TSpeedButton *btSelectItemAddDisk;
        TSpeedButton *btSelectItemAddSpiral;
        TSpeedButton *btSelectItemAddRing;
        TSpeedButton *btSelectItemAddModel;
        TPanel *paMain;
        TLabel *laConfigTitle;
        TBevel *blConfigSeparator;
        TButton *btBack;
        TTextureSelectionFrame *tsfConfigTexture;
        TLabel *laConfigTextureTitle;
        TLabel *laConfigBumpTitle;
        TTextureSelectionFrame *tsfConfigBump;
        TCheckListBox *clConfigOptions;
        TLabel *laConfigOptions;
        TBevel *blConfigSeparator2;
        TBevel *blConfigSeparator3;
        TBevel *blConfigSeparator4;
        TTabSheet *tsModel;
        TButton *btNext;
        TVertexColorFrame *vcfConfigVertexColor;
        TFileFrame *ffModelFile;
        TBevel *blModelSeparator1;
        TLabel *laModelTitle;
        TLabel *laModelFileNameCaption;
        TImage *imScreenshot;
        TPanel *paModelScreenshot;
        TLabel *laModelScreenshot;
        TPanel *paModelScreenshotLeft;
        TPanel *paModelScreenshotBgColor;
        TPanel *paModelScreenshotRight;
        TLabel *laModelScreenshotBgColorCaption;
        TPanel *paModelScreenshotBgColorValue;
        TColorDialog *cdColor;

        void __fastcall FormShow(TObject* pSender);
        void __fastcall paModelScreenshotBgColorValueClick(TObject* pSender);
        void __fastcall btCancelClick(TObject* pSender);
        void __fastcall btBackClick(TObject* pSender);
        void __fastcall OnNextClick(TObject* pSender);
        void __fastcall btOKClick(TObject* pSender);
        void __fastcall OnSelectItemButtonClick(TObject* pSender);

    public:
        /**
        * Model types
        */
        enum IEModelType
        {
            IE_MT_Unknown = 0,
            IE_MT_Surface,
            IE_MT_Box,
            IE_MT_Sphere,
            IE_MT_Cylinder,
            IE_MT_Disk,
            IE_MT_Ring,
            IE_MT_Spiral,
            IE_MT_Model
        };

        /**
        * Constructor
        *@aram pOwner - form owner
        */
        __fastcall TAddItemDialog(TComponent* pOwner);

        virtual __fastcall ~TAddItemDialog();

        /**
        * Gets a screenshot of the model belonging to the item
        *@param pBitmap - bitmap in which the screenshot should be drawn
        *@return true on success, otherwise false
        */
        bool GetScreenshot(TBitmap* pBitmap) const;

        /**
        * Gets the selected model type to create
        *@return the model type
        */
        IEModelType GetModelType() const;

    private:
        CSR_OpenGLHelper::IContext m_SceneContext;
        TForm*                     m_pSceneOverlayForm;
        CSR_Shader*                m_pSceneShader;
        CSR_Color                  m_SceneColor;
        CSR_MDL*                   m_pMDL;
        CSR_Model*                 m_pModel;
        CSR_AABBNode*              m_pAABBTree;
        CSR_Matrix4                m_ViewMatrix;
        IEModelType                m_ModelType;
        int                        m_AntialiasingFactor;

        /**
        * Creates an offscreen scene in which the model can be drawn to be exported as a screenshot
        *@param fileName - model file name to export as a screenshot
        *@param width - screenshot width
        *@param height - screenshot height
        *@return true on success, otherwise false
        */
        bool CreateScreenshotScene(const std::string& fileName, int width, int height);

        /**
        * Releases a previously created screenshot scene
        */
        void ReleaseScreenshotScene();

        /**
        * Draws a screenshot scene in a bitmap
        *@param pBitmap - bitmap in which the screenshot should be drawn
        *@return true on success, otherwise false
        */
        bool DrawScreenshotScene(TBitmap* pBitmap) const;

        /**
        * Checks if a model file exists
        *@return true if the model file exists, otherwise false
        */
        bool ModelFileExists() const;

        /**
        * Called when a file was selected
        *@param pSender - event sender
        *@param fileName - newly selected file name
        */
        void __fastcall OnFileSelected(TObject* pSender, const std::wstring& fileName);
};
extern PACKAGE TAddItemDialog* AddItemDialog;
#endif
