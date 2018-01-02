#include <vcl.h>
#pragma hdrstop
#include "Main.h"

#include "CSR_Common.h"
#include "CSR_Sound.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OpenAL32E.lib"
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
