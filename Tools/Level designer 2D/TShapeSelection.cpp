/****************************************************************************
 * ==> TShapeSelection -----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a dialog box to configure a shape on  *
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
#include "TShapeSelection.h"

// classes
#include "CSR_OpenGLHelper.h"

#pragma package(smart_init)
#pragma link "TVector3Frame"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
TShapeSelection* ShapeSelection;
//---------------------------------------------------------------------------
__fastcall TShapeSelection::TShapeSelection(TComponent* pOwner, const std::wstring& initialDir) :
    TForm(pOwner)
{
    // set the initial dir
    opdPicture->InitialDir = initialDir.c_str();
}
//---------------------------------------------------------------------------
void __fastcall TShapeSelection::btTextureBrowseClick(TObject* pSender)
{
    // open the load picture dialog box
    if (!opdPicture->Execute())
        return;

    // update the interface and open the picture
    edTextureFileName->Text = opdPicture->FileName;
    imTexture->Picture->LoadFromFile(edTextureFileName->Text);
}
//---------------------------------------------------------------------------
void __fastcall TShapeSelection::btCancelClick(TObject* pSender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TShapeSelection::btOkClick(TObject* pSender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void TShapeSelection::BuildMatrix(CSR_Matrix4* pMatrix) const
{
    CSR_OpenGLHelper::BuildMatrix(&vfPosition->GetVector(),
                                  &vfRotation->GetVector(),
                                  &vfScaling->GetVector(),
                                   pMatrix);
}
//---------------------------------------------------------------------------
