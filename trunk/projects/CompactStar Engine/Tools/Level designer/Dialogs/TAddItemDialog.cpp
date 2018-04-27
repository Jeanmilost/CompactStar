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

#include <vcl.h>
#pragma hdrstop
#include "TAddItemDialog.h"

// std
#include <memory>

// classes
#include "CSR_VCLHelper.h"

#pragma package(smart_init)
#pragma link "TTextureSelectionFrame"
#pragma link "TVertexColorFrame"
#pragma link "TFileNameFrame"
#pragma link "TScreenshotFrame"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
// TAddItemDialog
//---------------------------------------------------------------------------
TAddItemDialog* AddItemDialog;
//---------------------------------------------------------------------------
__fastcall TAddItemDialog::TAddItemDialog(TComponent* pOwner) :
    TForm(pOwner),
    m_ModelType(IE_MT_Unknown)
{
    // hide the wizard tabs
    CSR_VCLHelper::ChangeTabsVisibility(pcWizard, tsSelectItem, false);

    // to compensate the hidden tabs height
    Height = Height - 26;

    ffModelFile->Set_OnFileSelected(OnFileSelected);
}
//---------------------------------------------------------------------------
__fastcall TAddItemDialog::~TAddItemDialog()
{}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::FormShow(TObject* pSender)
{
    pcWizard->ClientHeight = btSelectItemAddModel->Top             +
                             btSelectItemAddModel->Height          +
                             btSelectItemAddModel->Margins->Bottom +
                             8;
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::btCancelClick(TObject* pSender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::btBackClick(TObject* pSender)
{
    btOK->Enabled = false;

    if (pcWizard->ActivePage == tsConfig)
    {
        if (m_ModelType == IE_MT_Model)
        {
            btNext->Enabled      = false;
            pcWizard->ActivePage = tsModel;
            return;
        }

        m_ModelType          = IE_MT_Unknown;
        btBack->Enabled      = false;
        btNext->Enabled      = true;
        pcWizard->ActivePage = tsSelectItem;
    }
    else
    if (pcWizard->ActivePage == tsModel)
    {
        m_ModelType          = IE_MT_Unknown;
        btBack->Enabled      = false;
        btNext->Enabled      = true;
        pcWizard->ActivePage = tsSelectItem;
    }
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::OnNextClick(TObject* pSender)
{
    // search for selected page
    if (pcWizard->ActivePage == tsSelectItem)
    {
        btBack->Enabled = true;
        btOK->Enabled   = false;

        // search for selected item
        if (btSelectItemAddSurface->Down)
        {
            m_ModelType          = IE_MT_Surface;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddBox->Down)
        {
            m_ModelType          = IE_MT_Box;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddSphere->Down)
        {
            m_ModelType          = IE_MT_Sphere;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddCylinder->Down)
        {
            m_ModelType          = IE_MT_Cylinder;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddDisk->Down)
        {
            m_ModelType          = IE_MT_Disk;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddRing->Down)
        {
            m_ModelType          = IE_MT_Ring;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddSpiral->Down)
        {
            m_ModelType          = IE_MT_Spiral;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddModel->Down)
        {
            m_ModelType          = IE_MT_Model;
            btNext->Enabled      = ModelFileExists();
            pcWizard->ActivePage = tsModel;
        }
    }
    else
    if (pcWizard->ActivePage == tsModel)
        pcWizard->ActivePage = tsConfig;
    else
    if (pcWizard->ActivePage == tsConfig)
    {
        pcWizard->ActivePage = tsIcon;
        btNext->Enabled      = false;
        btOK->Enabled        = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::btOKClick(TObject* pSender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::OnSelectItemButtonClick(TObject* pSender)
{
    btNext->Enabled =
            (pcWizard->ActivePage == tsSelectItem || (pcWizard->ActivePage == tsModel && ModelFileExists()));
}
//---------------------------------------------------------------------------
bool TAddItemDialog::GetIcon(TBitmap* pBitmap) const
{
    // FIXME
}
//---------------------------------------------------------------------------
TAddItemDialog::IEModelType TAddItemDialog::GetModelType() const
{
    return m_ModelType;
}
//---------------------------------------------------------------------------
bool TAddItemDialog::ModelFileExists() const
{
    return (!ffModelFile->edFileName->Text.IsEmpty() &&
           ::FileExists(ffModelFile->edFileName->Text.c_str(), false));
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::OnFileSelected(TObject* pSender, const std::wstring& fileName)
{
    // model file exists and can be loaded
    if (ModelFileExists() && sfScreenshot->LoadModel(AnsiString(UnicodeString(fileName.c_str())).c_str()))
    {
        btNext->Enabled = true;
        return;
    }

    btNext->Enabled = false;

    ::MessageDlg(L"The model you're trying to load is invalid.\n\nPlease check the model file and try again.",
                 mtError,
                 TMsgDlgButtons() << mbOK,
                 0);
}
//---------------------------------------------------------------------------
