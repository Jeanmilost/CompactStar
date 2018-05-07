/*****************************************************************************
 * ==> TImageDuplicateDialog ------------------------------------------------*
 *****************************************************************************
 * Description : This module contains a dialog that allows the user to       *
 *               select between 2 image duplicates                           *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#include <vcl.h>
#pragma hdrstop
#include "TImageDuplicateDialog.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

//----------------------------------------------------------------------------
// TImageDuplicateDialog
//----------------------------------------------------------------------------
TImageDuplicateDialog* ImageDuplicateDialog;
//----------------------------------------------------------------------------
__fastcall TImageDuplicateDialog::TImageDuplicateDialog(TComponent* pOwner) :
    TForm(pOwner)
{}
//----------------------------------------------------------------------------
void __fastcall TImageDuplicateDialog::rgActionClick(TObject* pSender)
{
    if (rgAction->ItemIndex == 2)
    {
        laDuplicatePrefix->Enabled = true;
        edDuplicatePrefix->Enabled = true;
    }
    else
    {
        laDuplicatePrefix->Enabled = false;
        edDuplicatePrefix->Enabled = false;
        edDuplicatePrefix->Text    = L"";
    }
}
//----------------------------------------------------------------------------
void __fastcall TImageDuplicateDialog::btOkClick(TObject* pSender)
{
    ModalResult = mrOk;
}
//----------------------------------------------------------------------------
