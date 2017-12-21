#include <vcl.h>
#pragma hdrstop
#include "Main.h"

#include "CSR_Common.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::bt1Click(TObject *Sender)
{
    CSR_Buffer* pBuffer = csrFileOpen("N:\\Jeanmilost\\Devel\\Projects\\CompactStar Engine\\Engine\\CSR_Common.h");
    if (pBuffer)
    {
        me1->Lines->SetText(UnicodeString((char*)pBuffer->m_pData).c_str());
    }
    csrBufferRelease(pBuffer);
}
//---------------------------------------------------------------------------
