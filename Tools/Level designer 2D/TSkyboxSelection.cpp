/****************************************************************************
 * ==> TSkyboxSelection ----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a dialog box to select the files      *
 *               required to generate a skybox                              *
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
#include "TSkyboxSelection.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
TSkyboxSelection* SkyboxSelection;
//---------------------------------------------------------------------------
__fastcall TSkyboxSelection::TSkyboxSelection(TComponent* pOwner, const std::wstring& initialDir) :
    TForm(pOwner)
{
    // set the initial dir
    opdPicture->InitialDir = initialDir.c_str();
}
//---------------------------------------------------------------------------
void __fastcall TSkyboxSelection::OnImageClick(TObject* pSender)
{
    // open the load picture dialog box
    if (!opdPicture->Execute())
        return;

    TImage* pImage = dynamic_cast<TImage*>(pSender);

    // show the picture
    if (pImage)
        pImage->Picture->LoadFromFile(opdPicture->FileName);

    // keep the file name
    if (pSender == imLeft)
        m_LeftFileName = opdPicture->FileName.c_str();
    else
    if (pSender == imBack)
        m_BackFileName = opdPicture->FileName.c_str();
    else
    if (pSender == imRight)
        m_RightFileName = opdPicture->FileName.c_str();
    else
    if (pSender == imFront)
        m_FrontFileName = opdPicture->FileName.c_str();
    else
    if (pSender == imTop)
        m_TopFileName = opdPicture->FileName.c_str();
    else
    if (pSender == imBottom)
        m_BottomFileName = opdPicture->FileName.c_str();

    // enable the Ok button only if all the images are set
    btOk->Enabled = (!m_LeftFileName.empty()   &&
                     !m_TopFileName.empty()    &&
                     !m_RightFileName.empty()  &&
                     !m_BottomFileName.empty() &&
                     !m_FrontFileName.empty()  &&
                     !m_BackFileName.empty());
}
//---------------------------------------------------------------------------
void __fastcall TSkyboxSelection::btCancelClick(TObject* pSender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TSkyboxSelection::btOkClick(TObject* pSender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void TSkyboxSelection::GetFileNames(IFileNames& fileNames) const
{
    fileNames.push_back(m_RightFileName);
    fileNames.push_back(m_LeftFileName);
    fileNames.push_back(m_TopFileName);
    fileNames.push_back(m_BottomFileName);
    fileNames.push_back(m_FrontFileName);
    fileNames.push_back(m_BackFileName);
}
//---------------------------------------------------------------------------
