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
void __fastcall TMainForm::bt1Click(TObject *Sender)
{
    CSR_Buffer* pBuffer = csrFileOpen("..\\..\\Main.h");
    if (pBuffer)
    {
        me1->Lines->SetText(UnicodeString((char*)pBuffer->m_pData).c_str());
    }
    csrBufferRelease(pBuffer);

    ALCdevice*  pOpenALDevice;
    ALCcontext* pOpenALContext;
    if (csrSoundInitializeOpenAL(&pOpenALDevice, &pOpenALContext))
    {
        CSR_Sound* pSound = csrSoundOpen(pOpenALDevice,
                                         pOpenALContext,
                                         //"C:\\Private\\Devel\\MiniAPI\\Mobile C Compiler\\Resources\\fire_and_reload.wav",
                                         "N:\\Jeanmilost\\Devel\\Projects\\MiniAPI\\trunk\\Mobile C Compiler\\Resources\\fire_and_reload.wav",
                                         48000);

        if (pSound)
        {
            csrSoundPlay(pSound);

            for (std::size_t i = 0; i < 200; ++i)
            {
                Application->ProcessMessages();
                ::Sleep(10);
            }
        }
        csrSoundRelease(pSound);
        csrSoundReleaseOpenAL(pOpenALDevice, pOpenALContext);
    }
}
//---------------------------------------------------------------------------
