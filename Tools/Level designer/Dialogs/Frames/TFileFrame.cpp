/*****************************************************************************
 * ==> TFileFrame -----------------------------------------------------------*
 *****************************************************************************
 * Description : This module provides a frame to open a file                 *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#include <vcl.h>
#pragma hdrstop
#include "TFileFrame.h"

// classes
#include "CSR_DesignerHelper.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
TFileFrame* FileFrame;
//---------------------------------------------------------------------------
__fastcall TFileFrame::TFileFrame(TComponent* pOwner) :
    TFrame(pOwner),
    m_fOnFileSelected(NULL)
{
    // set the default scene models dir
    m_DefaultDir       = (CSR_DesignerHelper::GetSceneDir() + L"Models\\").c_str();
    odOpen->InitialDir = m_DefaultDir.c_str();
}
//---------------------------------------------------------------------------
void __fastcall TFileFrame::btBrowseClick(TObject* pSender)
{
    // prompt the user to select a model file
    if (!odOpen->Execute())
        return;

    // show the file name
    edFileName->Text = odOpen->FileName;

    if (m_fOnFileSelected)
        m_fOnFileSelected(this, edFileName->Text.c_str());
}
//---------------------------------------------------------------------------
void TFileFrame::Set_OnFileSelected(ITfOnFileSelected fHandler)
{
    m_fOnFileSelected = fHandler;
}
//---------------------------------------------------------------------------
