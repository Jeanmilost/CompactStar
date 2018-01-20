/****************************************************************************
 * ==> AABB Tree tool model selection form ---------------------------------*
 ****************************************************************************
 * Description : Aligned-Axis Bounding Box tool selection form              *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2018, this file is part of the CompactStar Engine.  *
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

#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
TModelSelection* ModelSelection;
//---------------------------------------------------------------------------
__fastcall TModelSelection::TModelSelection(TComponent* pOwner) :
    TForm(pOwner)
{
    // build the model dir from the application exe
    UnicodeString modelDir = ::ExtractFilePath(Application->ExeName);
                  modelDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(modelDir));
                  modelDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(modelDir));
                  modelDir = ::ExcludeTrailingPathDelimiter(modelDir) + L"\\Common\\Models";

    // dir exists?
    if (!::DirectoryExists(modelDir))
    {
        // build the model dir from the application exe, in his /Debug or /Release path
        modelDir = ::ExtractFilePath(Application->ExeName);
        modelDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(modelDir));
        modelDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(modelDir));
        modelDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(modelDir));
        modelDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(modelDir));
        modelDir = ::ExcludeTrailingPathDelimiter(modelDir) + L"\\Common\\Models";
    }

    // set the default dir in open dialog, if exists
    if (::DirectoryExists(modelDir))
        odOpen->InitialDir = modelDir;
}
//---------------------------------------------------------------------------
void __fastcall TModelSelection::rgShapesClick(TObject* pSender)
{
    // clear the model values
    edMDLFilelName->Text = L"";
    m_ModelFileName.clear();
}
//---------------------------------------------------------------------------
void __fastcall TModelSelection::edSlicesAndStacksChange(TObject* pSender)
{
    // clear the model values
    edMDLFilelName->Text = L"";
    m_ModelFileName.clear();
}
//---------------------------------------------------------------------------
void __fastcall TModelSelection::btOpenFileClick(TObject* pSender)
{
    // show the open dialog box
    if (!odOpen->Execute())
        return;

    // update the model values
    edMDLFilelName->Text = odOpen->FileName;
    m_ModelFileName      = odOpen->FileName.c_str();
}
//---------------------------------------------------------------------------
void __fastcall TModelSelection::btSelectClick(TObject* pSender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
std::wstring TModelSelection::GetModelFileName() const
{
    return m_ModelFileName;
}
//---------------------------------------------------------------------------
