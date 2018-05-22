/*****************************************************************************
 * ==> TGrayscaleMapSelectionFrame ------------------------------------------*
 *****************************************************************************
 * Description : This module provides a frame to allow the user to load a    *
 *               grayscale bitmap which will be used to generate a landscape *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#include <vcl.h>
#pragma hdrstop
#include "TGrayscaleMapSelectionFrame.h"

// classes
#include "CSR_MessageHelper.h"
#include "CSR_DesignerHelper.h"

// dialogs
#include "TImageInfoDialog.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

//----------------------------------------------------------------------------
// TGrayscaleMapSelectionFrame
//----------------------------------------------------------------------------
TGrayscaleMapSelectionFrame* GrayscaleMapSelectionFrame;
//----------------------------------------------------------------------------
__fastcall TGrayscaleMapSelectionFrame::TGrayscaleMapSelectionFrame(TComponent* pOwner) :
    TFrame(pOwner),
    m_fOnFileSelected(NULL)
{
    // keep the default picture in case it should be restored
    m_pDefaultPicture.reset(new TPicture());
    m_pDefaultPicture->Assign(imBitmap->Picture);

    // set the default scene items dir
    m_DefaultDir       = CSR_DesignerHelper::GetBitmapsDir().c_str();
    odOpen->InitialDir = m_DefaultDir.c_str();
}
//----------------------------------------------------------------------------
void __fastcall TGrayscaleMapSelectionFrame::miDeleteBitmapClick(TObject* pSender)
{
    // reset the interface in his default state
    imBitmap->Picture->Assign(m_pDefaultPicture.get());
    imBitmap->Cursor    = crDefault;
    edBitmapFile->Text = L"";

    // notify that the texture was deleted
    if (m_fOnFileSelected)
        m_fOnFileSelected(this, L"");
}
//----------------------------------------------------------------------------
void __fastcall TGrayscaleMapSelectionFrame::btBitmapFileOpenClick(TObject* pSender)
{
    // prompt the user to select an image file
    if (!odOpen->Execute())
        return;

    // show the file name
    edBitmapFile->Text = odOpen->FileName;

    // show the file content
    imBitmap->Picture->LoadFromFile(edBitmapFile->Text);

    // do copy the image to the local scene dir?
    if (miCopyToLocalDir->Checked && ::ExtractFilePath(edBitmapFile->Text) != UnicodeString(m_DefaultDir.c_str()))
        // copy the file
        if (!::CopyFile(edBitmapFile->Text.c_str(),
                        (m_DefaultDir + ::ExtractFileName(edBitmapFile->Text).c_str()).c_str(),
                        true))
        {
            // show an error message to user on failure
            ::MessageDlg((CSR_MessageHelper::Get()->GetError_CopyTexture() + m_DefaultDir).c_str(),
                          mtError,
                          TMsgDlgButtons() << mbOK,
                          0);
        }

    // update the interface
    imBitmap->Cursor = crHandPoint;

    // notify that a file was selected
    if (m_fOnFileSelected)
        m_fOnFileSelected(this, edBitmapFile->Text.c_str());
}
//----------------------------------------------------------------------------
void __fastcall TGrayscaleMapSelectionFrame::btInfoClick(TObject* pSender)
{
    // cannot open the picture info dialog if no picture was loaded
    if (!imBitmap->Picture->Width || !imBitmap->Picture->Height || edBitmapFile->Text.IsEmpty())
        return;

    // create the image info dialog
    std::auto_ptr<TImageInfoDialog> pDialog
            (new TImageInfoDialog(this,
                                  imBitmap->Picture,
                                  ::ExtractFileName(edBitmapFile->Text).c_str()));

    // show it
    pDialog->ShowModal();
}
//----------------------------------------------------------------------------
void __fastcall TGrayscaleMapSelectionFrame::btConfigClick(TObject* pSender)
{
    // calculate the point where the popup menu sould be shown
    TPoint popupPos(0, btConfig->Height);

    // convert to screen coordinate system
    popupPos = btConfig->ClientToScreen(popupPos);

    // show the config popup menu
    pmConfig->Popup(popupPos.X, popupPos.Y);
}
//----------------------------------------------------------------------------
void TGrayscaleMapSelectionFrame::Set_OnFileSelected(ITfOnFileSelected fHandler)
{
    m_fOnFileSelected = fHandler;
}
//----------------------------------------------------------------------------
