/****************************************************************************
 * ==> Level designer main function ----------------------------------------*
 ****************************************************************************
 * Description : Level designer main function                               *
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
#include <tchar.h>

//---------------------------------------------------------------------------
USEFORM("Dialogs\TAddItemDialog.cpp", AddItemDialog);
USEFORM("Dialogs\Frames\TVertexColorFrame.cpp", VertexColorFrame); /* TFrame: File Type */
USEFORM("Dialogs\TImageDuplicateDialog.cpp", ImageDuplicateDialog);
USEFORM("Main.cpp", MainForm);
USEFORM("Dialogs\TImageInfoDialog.cpp", ImageInfoDialog);
USEFORM("Dialogs\Frames\TFileNameFrame.cpp", FileNameFrame); /* TFrame: File Type */
USEFORM("Dialogs\Frames\TTextureSelectionFrame.cpp", TextureSelectionFrame); /* TFrame: File Type */
USEFORM("Dialogs\Frames\TScreenshotFrame.cpp", ScreenshotFrame); /* TFrame: File Type */
USEFORM("Dialogs\Frames\TGrayscaleMapSelectionFrame.cpp", GrayscaleMapSelectionFrame); /* TFrame: File Type */
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    try
    {
        Application->Initialize();
        Application->MainFormOnTaskBar = true;
        Application->CreateForm(__classid(TMainForm), &MainForm);
        Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    catch (...)
    {
        try
        {
            throw Exception("");
        }
        catch (Exception &exception)
        {
            Application->ShowException(&exception);
        }
    }

    return 0;
}
//---------------------------------------------------------------------------
