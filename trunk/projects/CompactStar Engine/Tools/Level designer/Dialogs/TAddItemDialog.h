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
#include <Vcl.Dialogs.hpp>
#include <Vcl.ImgList.hpp>

// frames
#include "TTextureSelectionFrame.h"
#include "TVertexColorFrame.h"
#include "TFileNameFrame.h"
#include "TScreenshotFrame.h"

// classes
#include "CSR_DesignerHelper.h"
#include "TGrayscaleMapSelectionFrame.h"

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
        TTextureSelectionFrame *tsConfigTexture;
        TLabel *laConfigTextureTitle;
        TLabel *laConfigBumpTitle;
        TTextureSelectionFrame *tsConfigBump;
        TCheckListBox *clConfigOptions;
        TLabel *laConfigOptions;
        TBevel *blConfigSeparator2;
        TBevel *blConfigSeparator3;
        TBevel *blConfigSeparator4;
        TTabSheet *tsModel;
        TButton *btNext;
        TVertexColorFrame *vcConfigVertexColor;
        TFileNameFrame *ffModelFile;
        TBevel *blModelSeparator;
        TLabel *laModelTitle;
        TLabel *laModelFileNameCaption;
        TColorDialog *cdColor;
        TPanel *paSelectItemLeft;
        TPanel *paSelectItemRight;
        TImage *imSelectItemIcon;
        TTabSheet *tsIcon;
        TBevel *blIconSeparator;
        TLabel *laIconTitle;
        TScreenshotFrame *sfScreenshot;
        TRadioButton *rbIconDefault;
        TRadioButton *rbIconScreenshot;
        TRadioButton *rbImage;
        TFileNameFrame *fnIconImageFile;
        TImageList *ilDefaultIcons;
        TSpeedButton *btSelectItemAddLandscape;
        TTabSheet *tsLandscape;
        TLabel *laLandscapeSelectBitmap;
        TBevel *blLandscapeSeparator;
        TLabel *laLandscapeTitle;
        TBevel *blConfigSeparator5;
        TLabel *laConfigCollisionTitle;
        TPanel *paConfigCollision;
        TLabel *laConfigCollisionType;
        TComboBox *cbConfigCollisionType;
        TGrayscaleMapSelectionFrame *msLandscapeBitmap;
        TBevel *blLandscapeSeparator2;
        TLabel *laLandscapeOptions;
        TPanel *paLandscapeOptions;
        TLabel *laLandscapeOptionHeight;
        TEdit *edLandscapeOptionHeight;
        TLabel *laLandscapeOptionScaleFactor;
        TEdit *edLandscapeOptionScaleFactor;

        void __fastcall FormShow(TObject* pSender);
        void __fastcall clConfigOptionsClickCheck(TObject* pSender);
        void __fastcall rbIconDefaultClick(TObject* pSender);
        void __fastcall rbIconScreenshotClick(TObject* pSender);
        void __fastcall rbImageClick(TObject* pSender);
        void __fastcall btCancelClick(TObject* pSender);
        void __fastcall btBackClick(TObject* pSender);
        void __fastcall btOKClick(TObject* pSender);
        void __fastcall OnNextClick(TObject* pSender);
        void __fastcall OnValueChange(TObject* pSender);

    public:
        /**
        * Model options, may be combinated
        */
        enum IEModelOptions
        {
            IE_MO_None               = 0x0,
            IE_MO_UseNormal          = 0x1,
            IE_MO_UseTexture         = 0x2,
            IE_MO_UsePerVertexColor  = 0x4,
            IE_MO_PreCalculatedLight = 0x8
        };

        /**
        * Constructor
        *@aram pOwner - form owner
        */
        __fastcall TAddItemDialog(TComponent* pOwner);

        virtual __fastcall ~TAddItemDialog();

        /**
        * Gets the default item icon to show in the toolbox
        *@param pBitmap - bitmap to fill with the icon
        *@param color - icon background color
        *@return true on success, otherwise false
        */
        bool GetDefaultIcon(TBitmap* pBitmap, TColor color = clSilver) const;

        /**
        * Gets the item icon to show in the toolbox
        *@param pBitmap - bitmap to fill with the icon
        *@return true on success, otherwise false
        */
        bool GetIcon(TBitmap* pBitmap) const;

        /**
        * Gets the selected model type to create
        *@return the model type
        */
        CSR_DesignerHelper::IEModelType GetModelType() const;

        /**
        * Gets the model file name
        *@return the model file name
        */
        std::string GetModelFileName() const;

        /**
        * Gets the vertex color
        *@return the vertex color
        */
        unsigned GetVertexColor() const;

        /**
        * Gets the model texture
        *@param pBitmap - bitmap that will contain the texture
        *@return true on success, otherwise false
        */
        bool GetTexture(TBitmap* pBitmap) const;

        /**
        * Gets the model texture
        *@param pBitmap - bitmap that will contain the texture
        *@return true on success, otherwise false
        */
        bool GetBumpMap(TBitmap* pBitmap) const;

        /**
        * Gets the model options
        *@return the model options
        */
        IEModelOptions GetModelOptions() const;

    private:
        typedef std::map<TObject*, std::wstring> IValueDictionary;

        CSR_DesignerHelper::IEModelType m_ModelType;
        IValueDictionary                m_LastKnowValidValueDictionary;

        /**
        * Checks if a model file exists
        *@return true if the model file exists, otherwise false
        */
        bool ModelFileExists() const;

        /**
        * Checks if a grayscale bitmap file exists
        *@return true if the grayscale bitmap file exists, otherwise false
        */
        bool BitmapFileExists() const;

        /**
        * Called when a texture file was selected
        *@param pSender - event sender
        *@param fileName - newly selected file name
        */
        void __fastcall OnTextureFileSelected(TObject* pSender, const std::wstring& fileName);

        /**
        * Called when a model file was selected
        *@param pSender - event sender
        *@param fileName - newly selected file name
        */
        void __fastcall OnModelFileSelected(TObject* pSender, const std::wstring& fileName);

        /**
        * Called when a grayscape bitmap file was selected
        *@param pSender - event sender
        *@param fileName - newly selected file name
        */
        void __fastcall OnBitmapFileSelected(TObject* pSender, const std::wstring& fileName);
};
extern PACKAGE TAddItemDialog* AddItemDialog;
#endif
