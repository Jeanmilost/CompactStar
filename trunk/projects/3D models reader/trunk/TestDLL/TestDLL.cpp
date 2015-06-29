//---------------------------------------------------------------------------
// C++Builder
// Copyright (c) 1995-2010 Embarcadero Technologies, Inc.

// You may only use this software if you are an authorized licensee
// of Delphi, C++Builder or RAD Studio (Embarcadero Products).
// This software is considered a Redistributable as defined under
// the software license agreement that comes with the Embarcadero Products
// and is subject to that software license agreement.

#include <vcl.h>
#pragma hdrstop
#pragma package(smart_init) // madExcept
#pragma link "madExcept"
#pragma link "madLinkDisAsm"
#pragma link "madListHardware"
#pragma link "madListProcesses"
#pragma link "madListModules"
USEFORM("Main.cpp", MainForm);
//---------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  try
  {
    System::FSetExceptMask(System::femALLEXCEPT);
    Application->Initialize();
    Application->CreateForm(__classid(TMainForm), &MainForm);
         Application->Run();
  }
  catch (Exception &exception)
  {
    Application->ShowException(&exception);
  }
  return 0;
}
//---------------------------------------------------------------------------
