#include <vcl.h>
#pragma hdrstop
#include "TAddItemDialog.h"

// classes
#include "CSR_VCLHelper.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
TAddItemDialog* AddItemDialog;
//---------------------------------------------------------------------------
__fastcall TAddItemDialog::TAddItemDialog(TComponent* pOwner) :
    TForm(pOwner)
{
    CSR_VCLHelper::ChangeTabsVisibility(pcWizard, tsSelectItem, false);
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::FormShow(TObject* pSender)
{
    pcWizard->ClientHeight = btSelectItemAddModel->Top             +
                             btSelectItemAddModel->Height          +
                             btSelectItemAddModel->Margins->Bottom +
                             8;
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::btCancelClick(TObject* pSender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::btOKClick(TObject* pSender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::btSelectItemAddSurfaceClick(TObject* pSender)
{
    pcWizard->ActivePage = tsAddSurface;
}
//---------------------------------------------------------------------------
