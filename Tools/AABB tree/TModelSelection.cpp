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
    // update the user interface for the selected model
    switch (rgShapes->ItemIndex)
    {
        // surface or box
        case 0:
        case 1:
            paSlices->Visible      = false;
            paStacks->Visible      = false;
            paFaces->Visible       = false;
            paMDLModel->Visible    = false;
            paSelectedColor->Color = clBlue;
            break;

        // sphere
        case 2:
            paSlices->Visible      = true;
            paStacks->Visible      = true;
            paFaces->Visible       = false;
            paMDLModel->Visible    = false;
            paSelectedColor->Color = clBlue;
            break;

        // cylinder
        case 3:
            paSlices->Visible      = false;
            paStacks->Visible      = false;
            paFaces->Visible       = true;
            paMDLModel->Visible    = false;
            paSelectedColor->Color = clBlue;
            break;

        // disk or ring
        case 4:
        case 5:
            paSlices->Visible      = true;
            paStacks->Visible      = false;
            paFaces->Visible       = false;
            paMDLModel->Visible    = false;
            paSelectedColor->Color = clBlue;
            break;

        // spiral
        case 6:
            paSlices->Visible      = true;
            paStacks->Visible      = true;
            paFaces->Visible       = false;
            paMDLModel->Visible    = false;
            paSelectedColor->Color = clBlue;
            break;

        // MDL model
        case 7:
            paSlices->Visible      = false;
            paStacks->Visible      = false;
            paFaces->Visible       = false;
            paMDLModel->Visible    = true;
            paSelectedColor->Color = clWhite;
            break;

        default:
            paSlices->Visible      = false;
            paStacks->Visible      = false;
            paFaces->Visible       = false;
            paMDLModel->Visible    = false;
            paSelectedColor->Color = clWhite;
            break;
    }

    // sort the controls from top to bottom
    IControls controls;
    controls.push_back(rgShapes);
    controls.push_back(paSlices);
    controls.push_back(paStacks);
    controls.push_back(paFaces);
    controls.push_back(paMDLModel);
    controls.push_back(paColors);
    DistributeCtrlsTopToBottom(controls);
}
//---------------------------------------------------------------------------
void __fastcall TModelSelection::paSelectedColorClick(TObject* pSender)
{
    SelectModelColor();
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
    // show the open file dialog box
    if (!odOpen->Execute())
        return;

    // update the model values
    edMDLFilelName->Text = odOpen->FileName;
    m_ModelFileName      = odOpen->FileName.c_str();
}
//---------------------------------------------------------------------------
void __fastcall TModelSelection::btSelectColorClick(TObject* pSender)
{
    SelectModelColor();
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
void TModelSelection::SelectModelColor()
{
    // select the current color
    cdColors->Color = paSelectedColor->Color;

    // show the select color dialog box
    if (!cdColors->Execute())
        return;

    // get the newly selected color
    paSelectedColor->Color = cdColors->Color;
}
//---------------------------------------------------------------------------
void TModelSelection::DistributeCtrlsTopToBottom(IControls& controls)
{
    int pos = 0;

    // iterate through controls to distribute
    for (std::size_t i = 0; i < controls.size(); ++i)
    {
        // is control visible?
        if (!controls[i]->Visible)
            continue;

        // place the control on the left
        controls[i]->Top = pos;

        // calculate the next position
        pos += controls[i]->Margins->ControlHeight;
    }
}
//---------------------------------------------------------------------------
