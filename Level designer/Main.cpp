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
        pBuffer = csrFileOpen("C:\\Private\\Devel\\MiniAPI\\Mobile C Compiler\\Resources\\fire_and_reload.wav");
        ALuint bufferID;
        ALuint id;
        if (csrSoundCreate(pOpenALDevice, pOpenALContext, pBuffer, 48000, &bufferID, &id))
        {
            csrSoundPlay(id);

            for (std::size_t i = 0; i < 200; ++i)
            {
                Application->ProcessMessages();
                ::Sleep(10);
            }
        }
        csrBufferRelease(pBuffer);
        csrSoundRelease(bufferID, id);
        csrSoundReleaseOpenAL(pOpenALDevice, pOpenALContext);
    }
}
//---------------------------------------------------------------------------
