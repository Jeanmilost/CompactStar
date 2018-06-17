/****************************************************************************
 * ==> TBoxSelection -------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a dialog box to configure a box on    *
 *               the landscape                                              *
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

#include "TBoxSelection.h"

#pragma package(smart_init)
#pragma link "TVector3Frame"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
TBoxSelection* BoxSelection;
//---------------------------------------------------------------------------
__fastcall TBoxSelection::TBoxSelection(TComponent* pOwner, const std::wstring& initialDir) :
    TForm(pOwner)
{
    // set the initial dir
    opdPicture->InitialDir = initialDir.c_str();
}
//---------------------------------------------------------------------------
void __fastcall TBoxSelection::btTextureBrowseClick(TObject* pSender)
{
    // open the load picture dialog box
    if (!opdPicture->Execute())
        return;

    // update the interface and open the picture
    edTextureFileName->Text = opdPicture->FileName;
    imTexture->Picture->LoadFromFile(edTextureFileName->Text);
}
//---------------------------------------------------------------------------
void __fastcall TBoxSelection::btCancelClick(TObject* pSender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TBoxSelection::btOkClick(TObject* pSender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
