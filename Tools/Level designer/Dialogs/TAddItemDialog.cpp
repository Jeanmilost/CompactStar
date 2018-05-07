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
#include "CSR_MessageHelper.h"
#include "CSR_VCLHelper.h"

// dialogs
#include "TImageDuplicateDialog.h"

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
    m_ModelType(CSR_DesignerHelper::IE_MT_Unknown)
{
    // hide the wizard tabs
    CSR_VCLHelper::ChangeTabsVisibility(pcWizard, tsSelectItem, false);

    // to compensate the hidden tabs height
    Height = Height - 26;

    // set the default interface state
    sfScreenshot->Enable(false);
    fnIconImageFile->Enable(false);

    // set the callbacks
    tsConfigTexture->Set_OnFileSelected(OnTextureFileSelected);
    ffModelFile->Set_OnFileSelected(OnModelFileSelected);
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
void __fastcall TAddItemDialog::clConfigOptionsClickCheck(TObject* pSender)
{
    // the normals must be enabled if pre-calculated lighting is used
    if (clConfigOptions->Checked[3])
        clConfigOptions->Checked[0] = true;
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::rbIconDefaultClick(TObject* pSender)
{
    // set the default interface state
    sfScreenshot->Enable(false);
    fnIconImageFile->Enable(false);
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::rbIconScreenshotClick(TObject* pSender)
{
    // set the default interface state
    sfScreenshot->Enable(true);
    fnIconImageFile->Enable(false);
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::rbImageClick(TObject* pSender)
{
    // set the default interface state
    sfScreenshot->Enable(false);
    fnIconImageFile->Enable(true);
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::btCancelClick(TObject* pSender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::btBackClick(TObject* pSender)
{
    // ok button is always disabled in this case
    btOK->Enabled = false;

    // select the action to apply depending on the currently shown tab
    if (pcWizard->ActivePage == tsConfig)
    {
        // configuring a model?
        if (m_ModelType == CSR_DesignerHelper::IE_MT_Model)
        {
            btNext->Enabled      = ModelFileExists();
            pcWizard->ActivePage = tsModel;
            return;
        }

        m_ModelType          = CSR_DesignerHelper::IE_MT_Unknown;
        btBack->Visible      = false;
        btNext->Visible      = false;
        btBack->Enabled      = false;
        btNext->Enabled      = false;
        pcWizard->ActivePage = tsSelectItem;
    }
    else
    if (pcWizard->ActivePage == tsModel)
    {
        m_ModelType          = CSR_DesignerHelper::IE_MT_Unknown;
        btBack->Visible      = false;
        btNext->Visible      = false;
        btBack->Enabled      = false;
        btNext->Enabled      = false;
        pcWizard->ActivePage = tsSelectItem;
    }
    else
    if (pcWizard->ActivePage == tsIcon)
    {
        btNext->Enabled      = true;
        pcWizard->ActivePage = tsConfig;
    }
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::btOKClick(TObject* pSender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::OnNextClick(TObject* pSender)
{
    // select the action to apply depending on the currently shown tab
    if (pcWizard->ActivePage == tsSelectItem)
    {
        btNext->Visible = true;
        btBack->Visible = true;
        btNext->Enabled = true;
        btBack->Enabled = true;
        btOK->Enabled   = false;

        // search for selected item
        if (btSelectItemAddSurface->Down)
        {
            m_ModelType          = CSR_DesignerHelper::IE_MT_Surface;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddBox->Down)
        {
            m_ModelType          = CSR_DesignerHelper::IE_MT_Box;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddSphere->Down)
        {
            m_ModelType          = CSR_DesignerHelper::IE_MT_Sphere;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddCylinder->Down)
        {
            m_ModelType          = CSR_DesignerHelper::IE_MT_Cylinder;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddDisk->Down)
        {
            m_ModelType          = CSR_DesignerHelper::IE_MT_Disk;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddRing->Down)
        {
            m_ModelType          = CSR_DesignerHelper::IE_MT_Ring;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddSpiral->Down)
        {
            m_ModelType          = CSR_DesignerHelper::IE_MT_Spiral;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddModel->Down)
        {
            m_ModelType          = CSR_DesignerHelper::IE_MT_Model;
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
        try
        {
            Screen->Cursor = crHourGlass;

            // take a screenshot of the model. This also will check if the model can be built
            if (m_ModelType != CSR_DesignerHelper::IE_MT_Model)
            {
                // load a shape
                if (!sfScreenshot->LoadModel(m_ModelType,
                                             L"",
                                             tsConfigTexture->edTextureFile->Text.c_str(),
                                             tsConfigBump->edTextureFile->Text.c_str(),
                                             GetVertexColor()))
                {
                    ::MessageDlg(CSR_MessageHelper::Get()->GetError_OpeningModel().c_str(),
                                 mtError,
                                 TMsgDlgButtons() << mbOK,
                                 0);

                    return;
                }
            }
            else
            {
                // load a model from file
                if (!sfScreenshot->LoadModel(CSR_DesignerHelper::IE_MT_Model,
                                             ffModelFile->edFileName->Text.c_str(),
                                             tsConfigTexture->edTextureFile->Text.c_str(),
                                             tsConfigBump->edTextureFile->Text.c_str(),
                                             GetVertexColor()))
                {
                    ::MessageDlg(CSR_MessageHelper::Get()->GetError_LoadingModel().c_str(),
                                 mtError,
                                 TMsgDlgButtons() << mbOK,
                                 0);
                    return;
                }
            }
        }
        __finally
        {
            Screen->Cursor = crDefault;
        }

        pcWizard->ActivePage = tsIcon;
        btNext->Enabled      = false;
        btOK->Enabled        = true;
    }
}
//---------------------------------------------------------------------------
bool TAddItemDialog::GetDefaultIcon(TBitmap* pBitmap) const
{
}
//---------------------------------------------------------------------------
bool TAddItemDialog::GetIcon(TBitmap* pBitmap) const
{
    // no bitmap to load to?
    if (!pBitmap)
        return false;

    // search for the icon source to get from
    if (rbIconDefault->Checked)
        return GetDefaultIcon(pBitmap);
    else
    if (rbIconScreenshot->Checked)
        return sfScreenshot->GetScreenshot(pBitmap);
    else
    if (rbImage->Checked)
    {
        // no icon file or file does not exists?
        if (fnIconImageFile->edFileName->Text.IsEmpty() || !::FileExists(fnIconImageFile->edFileName->Text, false))
            return GetDefaultIcon(pBitmap);

        // load the texture image
        std::auto_ptr<TPicture> pPicture(new TPicture());
        pPicture->LoadFromFile(fnIconImageFile->edFileName->Text);

        // convert it to bitmap
        pBitmap->Assign(pPicture->Graphic);

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
CSR_DesignerHelper::IEModelType TAddItemDialog::GetModelType() const
{
    return m_ModelType;
}
//---------------------------------------------------------------------------
unsigned TAddItemDialog::GetVertexColor() const
{
    return csrColorBGRToRGBA(::ColorToRGB(vcConfigVertexColor->paColor->Color)) |
            (((vcConfigVertexColor->tbOpacity->Position * 255) / 100) & 0xFF);
}
//---------------------------------------------------------------------------
bool TAddItemDialog::ModelFileExists() const
{
    return (!ffModelFile->edFileName->Text.IsEmpty() && ::FileExists(ffModelFile->edFileName->Text, false));
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::OnTextureFileSelected(TObject* pSender, const std::wstring& fileName)
{
    // auto-check the use texture checkbox if a valid texture file was selected
    clConfigOptions->Checked[1] = !fileName.empty() && ::FileExists(UnicodeString(fileName.c_str()), false);
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::OnModelFileSelected(TObject* pSender, const std::wstring& fileName)
{
    const bool fileExists = ModelFileExists();

    btNext->Enabled = fileExists;

    if (!fileExists)
        return;

    // for MDL models, try to extract the textures. For other models, do nothing
    if (fileName.rfind(L"mdl") != fileName.length() - 3)
        return;

    std::vector<TBitmap*> textures;

    // extract the textures from the model
    if (!CSR_DesignerHelper::ExtractTexturesFromMDL(AnsiString(ffModelFile->edFileName->Text).c_str(),
                                                    NULL,
                                                    textures))
        // show a warning message to user
        ::MessageDlg(CSR_MessageHelper::Get()->GetWarn_ModelTextures().c_str(),
                     mtWarning,
                     TMsgDlgButtons() << mbOK,
                     0);

    // get the texture count
    const std::size_t textureCount = textures.size();

    // no texture?
    if (!textureCount)
        return;

    // model contains too many textures?
    if (textureCount > 1)
        // show a warning message to user
        ::MessageDlg(CSR_MessageHelper::Get()->GetWarn_UnsupportedTextureCount().c_str(),
                     mtWarning,
                     TMsgDlgButtons() << mbOK,
                     0);

    // get the default texture directory
    const std::wstring textureDir = CSR_DesignerHelper::GetTexturesDir();

    // default texture directory should exists
    if (::DirectoryExists(UnicodeString(textureDir.c_str(), false)))
    {
        // get the model name from his file
        const std::wstring modelName =
                ::ChangeFileExt(::ExtractFileName(ffModelFile->edFileName->Text), L"").c_str();

        // save all textures to file
        for (std::size_t i = 0; i < textures.size(); ++i)
        {
            // build the texture file name
            std::wstring textureFileName = textureDir + modelName + L".bmp";

            bool keep = false;

            // check if another file with the same name already exists, generates another name if yes
            while (1)//::FileExists(textureFileName.c_str(), false))
            {
                std::auto_ptr<TBitmap> pCurrent(new TBitmap());
                pCurrent->LoadFromFile(textureFileName.c_str());

                // create and configure the image duplicate dialog box
                std::auto_ptr<TImageDuplicateDialog> pDialog(new TImageDuplicateDialog(this));
                pDialog->imCurrent->Picture->Assign(textures[i]);//pCurrent.get());
                pDialog->imNew->Picture->Assign(textures[i]);

                // show dialog to user and check result
                if (pDialog->ShowModal() != mrOk)
                {
                    // keep the current image
                    keep = true;
                    break;
                }

                bool validated = false;

                // search for action to apply
                switch (pDialog->rgAction->ItemIndex)
                {
                    case 0:
                        // keep the current image
                        keep      = true;
                        validated = true;
                        break;

                    case 1:
                        // keep the new image
                        validated = true;
                        break;

                    case 2:
                        // generate a duplicate file
                        if (!pDialog->edDuplicatePrefix->Text.IsEmpty())
                            // generate a new texture file name using the user prefix
                            textureFileName = textureDir +
                                              modelName  +
                                              UnicodeString(L"_"                             +
                                                            pDialog->edDuplicatePrefix->Text +
                                                            L".bmp").c_str();
                        else
                            // generate a name for the duplicate
                            textureFileName = textureDir +
                                              modelName  +
                                              UnicodeString(L"("               +
                                                            ::IntToStr(int(i)) +
                                                            L".bmp").c_str();

                        break;

                    default:
                        // by default keep the current image
                        keep      = true;
                        validated = true;
                        break;
                }

                // is file name validated?
                if (validated)
                    break;
            }

            // do keep the file?
            if (keep)
                continue;

            // save the new texture file
            textures[i]->SaveToFile(UnicodeString(textureFileName.c_str()));
        }
    }

    for (std::size_t i = 0; i < textures.size(); ++i)
        delete textures[i];

    textures.clear();
}
//---------------------------------------------------------------------------
