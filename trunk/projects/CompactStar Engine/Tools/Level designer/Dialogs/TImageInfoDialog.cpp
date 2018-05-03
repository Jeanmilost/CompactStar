/*****************************************************************************
 * ==> TImageInfoDialog -----------------------------------------------------*
 *****************************************************************************
 * Description : This module contains a dialog to show image info            *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#include <vcl.h>
#pragma hdrstop
#include "TImageInfoDialog.h"

// classes
#include "CSR_VCLHelper.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
// TImageInfoDialog
//---------------------------------------------------------------------------
TImageInfoDialog* ImageInfoDialog;
//---------------------------------------------------------------------------
__fastcall TImageInfoDialog::TImageInfoDialog(      TComponent*   pOwner,
                                                    TPicture*     pPicture,
                                              const std::wstring& fileName) :
    TForm(pOwner)
{
    // this dialog cannot be shown without an image
    if (!pPicture)
        throw std::invalid_argument("Trying to show info dialog from an empty image");

    // assign the picture to the dialog image
    imImage->Picture->Assign(pPicture);

    // populate the image info
    laWidthValue->Caption     = ::IntToStr(pPicture->Width);
    laHeightValue->Caption    = ::IntToStr(pPicture->Height);
    laBPPValue->Caption       = ::IntToStr(int(CSR_VCLHelper::GetBitPerPixel(pPicture)));
    laImageTypeValue->Caption = CSR_VCLHelper::ImageTypeToStr(CSR_VCLHelper::GetImageType(pPicture)).c_str();
    laFileNameValue->Caption  = fileName.length() ? fileName.c_str() : L"";
}
//---------------------------------------------------------------------------
void __fastcall TImageInfoDialog::btCloseClick(TObject* pSender)
{
    Close();
}
//---------------------------------------------------------------------------
