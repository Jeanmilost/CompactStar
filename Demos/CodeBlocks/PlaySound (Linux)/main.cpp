/****************************************************************************
 * ==> Main ----------------------------------------------------------------*
 ****************************************************************************
 * Description : Simple sound playing demo                                  *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2022, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

// std
#include <string>
#include <iostream>
#include <unistd.h>

// compactstar engine
#include "CSR_Sound.h"

// sound file to load
#define SOUND_FILE "Resources/sample.wav"

//---------------------------------------------------------------------------
// Global variables
//---------------------------------------------------------------------------
ALCdevice*  g_pOpenALDevice  = NULL;
ALCcontext* g_pOpenALContext = NULL;
CSR_Sound*  g_pSound         = NULL;
//---------------------------------------------------------------------------
// Main entry point
//---------------------------------------------------------------------------
int main()
{
    // initialize OpenAL
    csrSoundInitializeOpenAL(&g_pOpenALDevice, &g_pOpenALContext);

    // load the sound to play
    g_pSound = csrSoundOpenWavFile(g_pOpenALDevice, g_pOpenALContext, SOUND_FILE);

    // failed?
    if (!g_pSound)
    {
        std::cout << "Failed to initialize sound" << std::endl;

        // release OpenAL interface
        csrSoundReleaseOpenAL(g_pOpenALDevice, g_pOpenALContext);

        return 1;
    }

    // play the sound but don't loop it
    csrSoundLoop(g_pSound, false);
    csrSoundPlay(g_pSound);

    // wait until the sound is played completely
    while (csrSoundIsPlaying(g_pSound))
        usleep(10000);

    // stop and release the loaded sound
    csrSoundStop(g_pSound);
    csrSoundRelease(g_pSound);

    // release OpenAL interface
    csrSoundReleaseOpenAL(g_pOpenALDevice, g_pOpenALContext);

    return 0;
}
//---------------------------------------------------------------------------
