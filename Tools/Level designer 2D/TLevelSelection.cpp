/****************************************************************************
 * ==> TLevelSelection -----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a dialog box to save the landscape    *
 *               level into a file                                          *
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
#include "TLevelSelection.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
TLevelSelection* LevelSelection;
//---------------------------------------------------------------------------
__fastcall TLevelSelection::TLevelSelection(TComponent* pOwner, const std::wstring& initialDir) :
    TForm(pOwner)
{
    // set the initial dir
    sdSave->InitialDir = initialDir.c_str();
}
//---------------------------------------------------------------------------
void __fastcall TLevelSelection::btBrowseClick(TObject* pSender)
{
    // prompt the user about which file name to use to save the level
    if (!sdSave->Execute())
        return;

    // show the newly selected file name
    edFileName->Text = sdSave->FileName;
}
//---------------------------------------------------------------------------
void __fastcall TLevelSelection::btCancelClick(TObject* pSender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TLevelSelection::btOkClick(TObject* pSender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
