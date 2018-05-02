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

// frames
#include "TTextureSelectionFrame.h"
#include "TVertexColorFrame.h"
#include "TFileNameFrame.h"
#include "TScreenshotFrame.h"

// classes
#include "CSR_DesignerHelper.h"

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
        TBevel *blModelSeparator1;
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

        void __fastcall FormShow(TObject* pSender);
        void __fastcall rbIconDefaultClick(TObject* pSender);
        void __fastcall rbIconScreenshotClick(TObject* pSender);
        void __fastcall rbImageClick(TObject* pSender);
        void __fastcall btCancelClick(TObject* pSender);
        void __fastcall btBackClick(TObject* pSender);
        void __fastcall btOKClick(TObject* pSender);
        void __fastcall OnNextClick(TObject* pSender);
        void __fastcall OnSelectItemButtonClick(TObject* pSender);

    public:
        /**
        * Constructor
        *@aram pOwner - form owner
        */
        __fastcall TAddItemDialog(TComponent* pOwner);

        virtual __fastcall ~TAddItemDialog();

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
        * Gets the vertex color
        *@return the vertex color
        */
        unsigned GetVertexColor() const;

    private:
        CSR_DesignerHelper::IEModelType m_ModelType;

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
