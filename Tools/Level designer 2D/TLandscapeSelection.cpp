/****************************************************************************
 * ==> TLandscapeSelection -------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a dialog box to select the files      *
 *               required to generate a landscape                           *
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

#include <vcl.h>
#pragma hdrstop
#include "TLandscapeSelection.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
// TLandscapeSelection
//---------------------------------------------------------------------------
TLandscapeSelection* LandscapeSelection;
//---------------------------------------------------------------------------
__fastcall TLandscapeSelection::TLandscapeSelection(TComponent* pOwner, const std::wstring& initialDir) :
    TForm(pOwner)
{
    // set the initial dirs
    odModel->InitialDir    = initialDir.c_str();
    opdPicture->InitialDir = initialDir.c_str();
}
//---------------------------------------------------------------------------
void __fastcall TLandscapeSelection::btBitmapBrowseClick(TObject* pSender)
{
    // open the load picture dialog box
    if (!opdPicture->Execute())
        return;

    // update the interface and open the picture
    edBitmapFileName->Text = opdPicture->FileName;
    imBitmap->Picture->LoadFromFile(edBitmapFileName->Text);
}
//---------------------------------------------------------------------------
void __fastcall TLandscapeSelection::btModelBrowseClick(TObject* pSender)
{
    // open the load model dialog box
    if (!odModel->Execute())
        return;

    // update the interface
    edModelFileName->Text = odModel->FileName;
}
//---------------------------------------------------------------------------
void __fastcall TLandscapeSelection::btTextureBrowseClick(TObject* pSender)
{
    // open the load picture dialog box
    if (!opdPicture->Execute())
        return;

    // update the interface and open the picture
    edTextureFileName->Text = opdPicture->FileName;
    imTexture->Picture->LoadFromFile(edTextureFileName->Text);
}
//---------------------------------------------------------------------------
void __fastcall TLandscapeSelection::btCancelClick(TObject* pSender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TLandscapeSelection::btOkClick(TObject* pSender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TLandscapeSelection::rbSourceBitmapClick(TObject* pSender)
{
    edBitmapFileName->Enabled = true;
    btBitmapBrowse->Enabled   = true;
    edModelFileName->Enabled  = false;
    btModelBrowse->Enabled    = false;
}
//---------------------------------------------------------------------------
void __fastcall TLandscapeSelection::rbSourceModelClick(TObject* pSender)
{
    edBitmapFileName->Enabled = false;
    btBitmapBrowse->Enabled   = false;
    edModelFileName->Enabled  = true;
    btModelBrowse->Enabled    = true;
}
//---------------------------------------------------------------------------
void __fastcall TLandscapeSelection::rbSourceDontModifyClick(TObject* pSender)
{
    edBitmapFileName->Enabled = false;
    btBitmapBrowse->Enabled   = false;
    edModelFileName->Enabled  = false;
    btModelBrowse->Enabled    = false;
}
//---------------------------------------------------------------------------
