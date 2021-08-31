/****************************************************************************
 * ==> Level designer for 2D games ----------------------------------------*
 ****************************************************************************
 * Description : CompactStar Engine level designer for 2D games             *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2019, this file is part of the CompactStar Engine.  *
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

// vcl
#include <tchar.h>

//---------------------------------------------------------------------------
USEFORM("TShapeSelection.cpp", ShapeSelection);
USEFORM("TModelSelection.cpp", ModelSelection);
USEFORM("TSkyboxSelection.cpp", SkyboxSelection);
USEFORM("TVector3Frame.cpp", Vector3Frame); /* TFrame: File Type */
USEFORM("TSoundSelection.cpp", SoundSelection);
USEFORM("TLevelSelection.cpp", LevelSelection);
USEFORM("TLandscapeSelection.cpp", LandscapeSelection);
USEFORM("Main.cpp", MainForm);
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
