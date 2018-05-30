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
#pragma link "TGrayscaleMapSelectionFrame"
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

    // populate the last know values dictionary (used to revert incorrect inputs on numeric edits)
    m_LastKnowValidValueDictionary[edLandscapeOptionHeight]      = edLandscapeOptionHeight->Text.c_str();
    m_LastKnowValidValueDictionary[edLandscapeOptionScaleFactor] = edLandscapeOptionScaleFactor->Text.c_str();

    // set the callbacks
    tsConfigTexture->Set_OnFileSelected(OnTextureFileSelected);
    ffModelFile->Set_OnFileSelected(OnModelFileSelected);
    msLandscapeBitmap->Set_OnFileSelected(OnBitmapFileSelected);
}
//---------------------------------------------------------------------------
__fastcall TAddItemDialog::~TAddItemDialog()
{}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::FormShow(TObject* pSender)
{
    pcWizard->ClientHeight = btSelectItemAddLandscape->Top             +
                             btSelectItemAddLandscape->Height          +
                             btSelectItemAddLandscape->Margins->Bottom +
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
    M_CSR_Try
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
            // or configuring a landscape?
            else
            if (m_ModelType == CSR_DesignerHelper::IE_MT_Landscape)
            {
                btNext->Enabled      = BitmapFileExists();
                pcWizard->ActivePage = tsLandscape;
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
        if (pcWizard->ActivePage == tsModel || pcWizard->ActivePage == tsLandscape)
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
    M_CSR_CatchShow
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::btOKClick(TObject* pSender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::OnNextClick(TObject* pSender)
{
    M_CSR_Try
    {
        // select the action to apply depending on the currently shown tab
        if (pcWizard->ActivePage == tsSelectItem)
        {
            btNext->Visible = true;
            btBack->Visible = true;
            btNext->Enabled = true;
            btBack->Enabled = true;
            btOK->Enabled   = false;

            // select no collision by default
            cbConfigCollisionType->ItemIndex = 0;

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
            else
            if (btSelectItemAddLandscape->Down)
            {
                m_ModelType          = CSR_DesignerHelper::IE_MT_Landscape;
                btNext->Enabled      = BitmapFileExists();
                pcWizard->ActivePage = tsLandscape;
            }
        }
        else
        if (pcWizard->ActivePage == tsModel)
            pcWizard->ActivePage = tsConfig;
        else
        if (pcWizard->ActivePage == tsLandscape)
        {
            // as a landscape is typically a ground model, select the ground collision type by default
            cbConfigCollisionType->ItemIndex = 1;
            pcWizard->ActivePage             = tsConfig;
        }
        else
        if (pcWizard->ActivePage == tsConfig)
        {
            try
            {
                Screen->Cursor = crHourGlass;

                // take a screenshot of the model. This also will check if the model can be built
                switch (m_ModelType)
                {
                    case CSR_DesignerHelper::IE_MT_Surface:
                    case CSR_DesignerHelper::IE_MT_Box:
                    case CSR_DesignerHelper::IE_MT_Sphere:
                    case CSR_DesignerHelper::IE_MT_Cylinder:
                    case CSR_DesignerHelper::IE_MT_Disk:
                    case CSR_DesignerHelper::IE_MT_Ring:
                    case CSR_DesignerHelper::IE_MT_Spiral:
                        // load a shape
                        if (!sfScreenshot->LoadModel(m_ModelType,
                                                     L"",
                                                     tsConfigTexture->edTextureFile->Text.c_str(),
                                                     tsConfigBump->edTextureFile->Text.c_str(),
                                                     GetVertexColor(),
                                                     0.0f,
                                                     0.0f))
                        {
                            ::MessageDlg(CSR_MessageHelper::Get()->GetError_OpeningModel().c_str(),
                                         mtError,
                                         TMsgDlgButtons() << mbOK,
                                         0);

                            return;
                        }

                        break;

                    case CSR_DesignerHelper::IE_MT_Model:
                        // load a model from file
                        if (!sfScreenshot->LoadModel(m_ModelType,
                                                     ffModelFile->edFileName->Text.c_str(),
                                                     tsConfigTexture->edTextureFile->Text.c_str(),
                                                     tsConfigBump->edTextureFile->Text.c_str(),
                                                     GetVertexColor(),
                                                     0.0f,
                                                     0.0f))
                        {
                            ::MessageDlg(CSR_MessageHelper::Get()->GetError_LoadingModel().c_str(),
                                         mtError,
                                         TMsgDlgButtons() << mbOK,
                                         0);
                            return;
                        }

                        break;

                    case CSR_DesignerHelper::IE_MT_Landscape:
                        // load a model from file
                        if (!sfScreenshot->LoadModel(m_ModelType,
                                                     msLandscapeBitmap->edBitmapFile->Text.c_str(),
                                                     tsConfigTexture->edTextureFile->Text.c_str(),
                                                     tsConfigBump->edTextureFile->Text.c_str(),
                                                     GetVertexColor(),
                                                     GetLandscapeHeight(),
                                                     GetLandscapeFactor()))
                        {
                            ::MessageDlg(CSR_MessageHelper::Get()->GetError_LoadingModel().c_str(),
                                         mtError,
                                         TMsgDlgButtons() << mbOK,
                                         0);
                            return;
                        }

                        break;

                    default:
                        ::MessageDlg(CSR_MessageHelper::Get()->GetError_LoadingModel().c_str(),
                                     mtError,
                                     TMsgDlgButtons() << mbOK,
                                     0);
                        return;
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
    M_CSR_CatchShow
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::OnValueChange(TObject* pSender)
{
    // get the edit containing the text to check
    TEdit* pEdit = dynamic_cast<TEdit*>(pSender);

    if (!pEdit)
        return;

    // is text empty or just containing a "-" value? (Meaning that further info will be entered later)
    if (pEdit->Text.IsEmpty() || pEdit->Text == L"-")
        return;

    // get the last known value for this edit
    IValueDictionary::iterator it = m_LastKnowValidValueDictionary.find(pSender);

    // is value not found? (Should never happen)
    if (it == m_LastKnowValidValueDictionary.end())
        return;

    try
    {
        // try to convert the text to float
        ::StrToFloat(pEdit->Text);
    }
    catch (...)
    {
        // get the caret position
        const int caret = pEdit->SelStart;

        // on failure revert to the last known valid value
        pEdit->Text = UnicodeString(it->second.c_str());

        pEdit->SelStart  = caret ? caret - 1 : 0;
        pEdit->SelLength = 0;
        return;
    }

    // update the last known value
    it->second = pEdit->Text.c_str();
}
//---------------------------------------------------------------------------
bool TAddItemDialog::GetDefaultIcon(TBitmap* pBitmap, TColor color) const
{
    // no bitmap to load to?
    if (!pBitmap)
        return false;

    M_CSR_Try
    {
        // set the bitmap size
        pBitmap->SetSize(ilDefaultIcons->Width, ilDefaultIcons->Height);

        // fill the background with the default color
        pBitmap->Canvas->Brush->Color = color;
        pBitmap->Canvas->Brush->Style = bsSolid;
        pBitmap->Canvas->FillRect(TRect(0, 0, ilDefaultIcons->Width, ilDefaultIcons->Height));

        // draw the default icon inside the destination bitmap
        switch (m_ModelType)
        {
            case CSR_DesignerHelper::IE_MT_Surface:   ilDefaultIcons->Draw(pBitmap->Canvas, 0, 0, 0); return true;
            case CSR_DesignerHelper::IE_MT_Box:       ilDefaultIcons->Draw(pBitmap->Canvas, 0, 0, 1); return true;
            case CSR_DesignerHelper::IE_MT_Sphere:    ilDefaultIcons->Draw(pBitmap->Canvas, 0, 0, 2); return true;
            case CSR_DesignerHelper::IE_MT_Cylinder:  ilDefaultIcons->Draw(pBitmap->Canvas, 0, 0, 3); return true;
            case CSR_DesignerHelper::IE_MT_Disk:      ilDefaultIcons->Draw(pBitmap->Canvas, 0, 0, 4); return true;
            case CSR_DesignerHelper::IE_MT_Ring:      ilDefaultIcons->Draw(pBitmap->Canvas, 0, 0, 5); return true;
            case CSR_DesignerHelper::IE_MT_Spiral:    ilDefaultIcons->Draw(pBitmap->Canvas, 0, 0, 6); return true;
            case CSR_DesignerHelper::IE_MT_Model:     ilDefaultIcons->Draw(pBitmap->Canvas, 0, 0, 7); return true;
            case CSR_DesignerHelper::IE_MT_Landscape: ilDefaultIcons->Draw(pBitmap->Canvas, 0, 0, 8); return true;
            default:                                                                                  return false;
        }
    }
    M_CSR_CatchShow

    return false;
}
//---------------------------------------------------------------------------
bool TAddItemDialog::GetIcon(TBitmap* pBitmap) const
{
    // no bitmap to load to?
    if (!pBitmap)
        return false;

    M_CSR_Try
    {
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
            if (fnIconImageFile->edFileName->Text.IsEmpty() ||
                    !::FileExists(fnIconImageFile->edFileName->Text, false))
                return GetDefaultIcon(pBitmap);

            // load the texture image
            std::auto_ptr<TPicture> pPicture(new TPicture());
            pPicture->LoadFromFile(fnIconImageFile->edFileName->Text);

            // convert it to bitmap
            pBitmap->Assign(pPicture->Graphic);

            return true;
        }
    }
    M_CSR_CatchShow

    return false;
}
//---------------------------------------------------------------------------
CSR_DesignerHelper::IEModelType TAddItemDialog::GetModelType() const
{
    return m_ModelType;
}
//---------------------------------------------------------------------------
std::string TAddItemDialog::GetModelFileName() const
{
    return AnsiString(ffModelFile->edFileName->Text).c_str();
}
//---------------------------------------------------------------------------
unsigned TAddItemDialog::GetVertexColor() const
{
    return csrColorBGRToRGBA(::ColorToRGB(vcConfigVertexColor->paColor->Color)) |
            (((vcConfigVertexColor->tbOpacity->Position * 255) / 100) & 0xFF);
}
//---------------------------------------------------------------------------
bool TAddItemDialog::GetTexture(TBitmap* pBitmap) const
{
    // to bitmap to fill with texture?
    if (!pBitmap)
        return false;

    M_CSR_Try
    {
        // no texture file or file does not exists?
        if (tsConfigTexture->edTextureFile->Text.IsEmpty() ||
                !::FileExists(tsConfigTexture->edTextureFile->Text, false))
            return false;

        // load the texture image
        std::auto_ptr<TPicture> pPicture(new TPicture());
        pPicture->LoadFromFile(tsConfigTexture->edTextureFile->Text);

        // get the texture
        pBitmap->Assign(pPicture.get());
    }
    M_CSR_CatchShow

    return false;
}
//---------------------------------------------------------------------------
bool TAddItemDialog::GetBumpMap(TBitmap* pBitmap) const
{
    // to bitmap to fill with bumpmap?
    if (!pBitmap)
        return false;

    M_CSR_Try
    {
        // no texture file or file does not exists?
        if (tsConfigBump->edTextureFile->Text.IsEmpty() ||
                !::FileExists(tsConfigBump->edTextureFile->Text, false))
            return false;

        // load the texture image
        std::auto_ptr<TPicture> pPicture(new TPicture());
        pPicture->LoadFromFile(tsConfigBump->edTextureFile->Text);

        // get the texture
        pBitmap->Assign(pPicture.get());
    }
    M_CSR_CatchShow

    return false;
}
//---------------------------------------------------------------------------
TAddItemDialog::IEModelOptions TAddItemDialog::GetModelOptions() const
{
    IEModelOptions options = IE_MO_None;

    if (clConfigOptions->Checked[0])
        options = IEModelOptions(options | IE_MO_UseNormal);

    if (clConfigOptions->Checked[1])
        options = IEModelOptions(options | IE_MO_UseTexture);

    if (clConfigOptions->Checked[2])
        options = IEModelOptions(options | IE_MO_UsePerVertexColor);

    if (clConfigOptions->Checked[3])
        options = IEModelOptions(options | IE_MO_PreCalculatedLight);

    return options;
}
//---------------------------------------------------------------------------
CSR_ECollisionType TAddItemDialog::GetCollisionType() const
{
    // search for the selected collision type
    switch (cbConfigCollisionType->ItemIndex)
    {
        case 1:  return CSR_CO_Ground;
        case 2:  return CSR_CO_Edge;
        case 3:  return CSR_ECollisionType(CSR_CO_Ground | CSR_CO_Edge);
        default: return CSR_CO_None;
    }
}
//---------------------------------------------------------------------------
std::string TAddItemDialog::GetLandscapeBitmapFileName() const
{
    return AnsiString(msLandscapeBitmap->edBitmapFile->Text).c_str();
}
//---------------------------------------------------------------------------
float TAddItemDialog::GetLandscapeHeight() const
{
    if (edLandscapeOptionHeight->Text.IsEmpty())
        return 0.0f;

    float result;

    try
    {
        // convert the landscape height value
        result = ::StrToFloat(edLandscapeOptionHeight->Text);
    }
    catch (...)
    {
        return 0.0f;
    }

    return result;
}
//---------------------------------------------------------------------------
float TAddItemDialog::GetLandscapeFactor() const
{
    if (edLandscapeOptionScaleFactor->Text.IsEmpty())
        return 0.0f;

    float result;

    try
    {
        // convert the landscape height value
        result = ::StrToFloat(edLandscapeOptionScaleFactor->Text);
    }
    catch (...)
    {
        return 0.0f;
    }

    return result;
}
//---------------------------------------------------------------------------
bool TAddItemDialog::ModelFileExists() const
{
    return (!ffModelFile->edFileName->Text.IsEmpty() && ::FileExists(ffModelFile->edFileName->Text, false));
}
//---------------------------------------------------------------------------
bool TAddItemDialog::BitmapFileExists() const
{
    return (!msLandscapeBitmap->edBitmapFile->Text.IsEmpty() && ::FileExists(msLandscapeBitmap->edBitmapFile->Text, false));
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
    M_CSR_Try
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
        if (::DirectoryExists(::ExcludeTrailingPathDelimiter(UnicodeString(textureDir.c_str())), false))
        {
            // get the model name from his file
            const std::wstring modelName =
                    ::ChangeFileExt(::ExtractFileName(ffModelFile->edFileName->Text), L"").c_str();

            // save all textures to file
            for (std::size_t i = 0; i < textures.size(); ++i)
            {
                // build the texture file name
                std::wstring textureFileName = textureDir + modelName + L".bmp";

                std::size_t index = 1;
                bool        keep  = false;

                // check if another file with the same name already exists, generates another name if yes
                while (::FileExists(textureFileName.c_str(), false))
                {
                    std::auto_ptr<TBitmap> pCurrent(new TBitmap());
                    pCurrent->LoadFromFile(textureFileName.c_str());

                    // create and configure the image duplicate dialog box
                    std::auto_ptr<TImageDuplicateDialog> pDialog(new TImageDuplicateDialog(this));
                    pDialog->laFileName->Caption = ::ExtractFileName(UnicodeString(textureFileName.c_str()));
                    pDialog->imCurrent->Picture->Assign(pCurrent.get());
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
                                                  UnicodeString(L"("                   +
                                                                ::IntToStr(int(index)) +
                                                                L").bmp").c_str();

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

                    ++index;
                }

                // do keep the current file and ignore the new one?
                if (!keep)
                    // save (and eventually replace) the new texture file
                    textures[i]->SaveToFile(textureFileName.c_str());

                // is the first texture?
                if (!i)
                {
                    // open it on the texture field
                    clConfigOptions->Checked[1]          = true;
                    tsConfigTexture->edTextureFile->Text = textureFileName.c_str();
                    tsConfigTexture->imTexture->Cursor   = crHandPoint;
                    tsConfigTexture->imTexture->Picture->Assign(textures[i]);
                }
            }
        }

        for (std::size_t i = 0; i < textures.size(); ++i)
            delete textures[i];

        textures.clear();
    }
    M_CSR_CatchShow
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::OnBitmapFileSelected(TObject* pSender, const std::wstring& fileName)
{
    btNext->Enabled = BitmapFileExists();
}
//---------------------------------------------------------------------------
