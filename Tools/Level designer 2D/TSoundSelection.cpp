/****************************************************************************
 * ==> TSoundSelection -----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a dialog box to select an ambient     *
 *               sound                                                      *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2022, this file is part of the CompactStar Engine.  *
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
#include "TSoundSelection.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
// TSoundSelection
//---------------------------------------------------------------------------
TSoundSelection* SoundSelection;
//---------------------------------------------------------------------------
__fastcall TSoundSelection::TSoundSelection(TComponent* pOwner, const std::wstring& initialDir) :
    TForm(pOwner)
{
    // set the sound initial dir
    odSound->InitialDir = initialDir.c_str();
}
//---------------------------------------------------------------------------
void __fastcall TSoundSelection::btSoundBrowseClick(TObject* pSender)
{
    // open the load sound dialog box
    if (!odSound->Execute())
        return;

    // update the interface
    edSoundFileName->Text = odSound->FileName;
}
//---------------------------------------------------------------------------
void __fastcall TSoundSelection::btCancelClick(TObject* pSender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TSoundSelection::btOkClick(TObject* pSender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
