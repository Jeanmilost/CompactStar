/****************************************************************************
 * ==> TModelSelection -----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a dialog box to configure a model on  *
 *               the landscape                                              *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2019, this file is part of the CompactStar Engine.  *
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
#include "TModelSelection.h"

// classes
#include "CSR_OpenGLHelper.h"

#pragma package(smart_init)
#pragma link "TVector3Frame"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
TModelSelection* ModelSelection;
//---------------------------------------------------------------------------
__fastcall TModelSelection::TModelSelection(TComponent* pOwner, const std::wstring& initialDir) :
    TForm(pOwner)
{
    // set the initial dirs
    odModel->InitialDir    = initialDir.c_str();
    opdPicture->InitialDir = initialDir.c_str();
}
//---------------------------------------------------------------------------
void __fastcall TModelSelection::btModelBrowseClick(TObject* pSender)
{
    // open the load model dialog box
    if (!odModel->Execute())
        return;

    // update the interface
    edModelFileName->Text = odModel->FileName;
}
//---------------------------------------------------------------------------
void __fastcall TModelSelection::btTextureBrowseClick(TObject* pSender)
{
    // open the load picture dialog box
    if (!opdPicture->Execute())
        return;

    // update the interface and open the picture
    edTextureFileName->Text = opdPicture->FileName;
    imTexture->Picture->LoadFromFile(edTextureFileName->Text);
}
//---------------------------------------------------------------------------
void __fastcall TModelSelection::btCancelClick(TObject* pSender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TModelSelection::btOkClick(TObject* pSender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void TModelSelection::BuildMatrix(CSR_Matrix4* pMatrix) const
{
    CSR_OpenGLHelper::BuildMatrix(&vfPosition->GetVector(),
                                  &vfRotation->GetVector(),
                                  &vfScaling->GetVector(),
                                   pMatrix);
}
//---------------------------------------------------------------------------
