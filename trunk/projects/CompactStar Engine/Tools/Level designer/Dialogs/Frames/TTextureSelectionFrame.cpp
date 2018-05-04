/*****************************************************************************
 * ==> TTextureSelectionFrame -----------------------------------------------*
 *****************************************************************************
 * Description : This module provides a frame to allow the user to load a    *
 *               texture for a model                                         *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#include <vcl.h>
#pragma hdrstop
#include "TTextureSelectionFrame.h"

// classes
#include "CSR_MessageHelper.h"
#include "CSR_DesignerHelper.h"

// dialogs
#include "TImageInfoDialog.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
// TTextureSelectionFrame
//---------------------------------------------------------------------------
TTextureSelectionFrame* TextureSelectionFrame;
//---------------------------------------------------------------------------
__fastcall TTextureSelectionFrame::TTextureSelectionFrame(TComponent* pOwner) :
    TFrame(pOwner),
    m_fOnFileSelected(NULL)
{
    // keep the default picture in case it should be restored
    m_pDefaultPicture.reset(new TPicture());
    m_pDefaultPicture->Assign(imTexture->Picture);

    // set the default scene items dir
    m_DefaultDir       = CSR_DesignerHelper::GetTexturesDir().c_str();
    odOpen->InitialDir = m_DefaultDir.c_str();
}
//---------------------------------------------------------------------------
void __fastcall TTextureSelectionFrame::miDeleteTextureClick(TObject* pSender)
{
    // reset the interface in his default state
    imTexture->Picture->Assign(m_pDefaultPicture.get());
    imTexture->Cursor   = crDefault;
    edTextureFile->Text = L"";

    // notify that the texture was deleted
    if (m_fOnFileSelected)
        m_fOnFileSelected(this, L"");
}
//---------------------------------------------------------------------------
void __fastcall TTextureSelectionFrame::btTextureFileOpenClick(TObject* pSender)
{
    // prompt the user to select an image file
    if (!odOpen->Execute())
        return;

    // show the file name
    edTextureFile->Text = odOpen->FileName;

    // show the file content
    imTexture->Picture->LoadFromFile(edTextureFile->Text);

    // do copy the image to the local scene dir?
    if (miCopyToLocalDir->Checked && ::ExtractFilePath(edTextureFile->Text) != UnicodeString(m_DefaultDir.c_str()))
        // copy the file
        if (!::CopyFile(edTextureFile->Text.c_str(),
                        (m_DefaultDir + ::ExtractFileName(edTextureFile->Text).c_str()).c_str(),
                        true))
        {
            // show an error message to user on failure
            ::MessageDlg((CSR_MessageHelper::Get()->GetError_CopyTexture() + m_DefaultDir).c_str(),
                          mtError,
                          TMsgDlgButtons() << mbOK,
                          0);
        }

    // update the interface
    imTexture->Cursor = crHandPoint;

    // notify that a file was selected
    if (m_fOnFileSelected)
        m_fOnFileSelected(this, edTextureFile->Text.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TTextureSelectionFrame::btInfoClick(TObject* pSender)
{
    // cannot open the picture info dialog if no picture was loaded
    if (!imTexture->Picture->Width || !imTexture->Picture->Height || edTextureFile->Text.IsEmpty())
        return;

    // create the image info dialog
    std::auto_ptr<TImageInfoDialog> pDialog
            (new TImageInfoDialog(this,
                                  imTexture->Picture,
                                  ::ExtractFileName(edTextureFile->Text).c_str()));

    // show it
    pDialog->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TTextureSelectionFrame::btConfigClick(TObject* pSender)
{
    // calculate the point where the popup menu sould be shown
    TPoint popupPos(0, btConfig->Height);

    // convert to screen coordinate system
    popupPos = btConfig->ClientToScreen(popupPos);

    // show the config popup menu
    pmConfig->Popup(popupPos.X, popupPos.Y);
}
//---------------------------------------------------------------------------
void TTextureSelectionFrame::Set_OnFileSelected(ITfOnFileSelected fHandler)
{
    m_fOnFileSelected = fHandler;
}
//---------------------------------------------------------------------------
