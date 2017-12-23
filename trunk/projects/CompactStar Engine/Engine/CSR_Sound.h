/****************************************************************************
 * ==> CSR_Sound -----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the functions to play sound and music *
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

#ifndef CSR_SoundH
#define CSR_SoundH

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"

// OpenAL library
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>
#elif defined(__APPLE__)
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>
#elif defined (__CODEGEARC__) || defined (__GNUC__)
    #include <al.h>
    #include <alc.h>
#endif

//---------------------------------------------------------------------------
// Global defines
//---------------------------------------------------------------------------
#define M_OPENAL_ERROR_ID 0xFFFFFFFF

#ifdef __cplusplus
    extern "C"
    {
#endif

        //-------------------------------------------------------------------
        // Sound functions
        //-------------------------------------------------------------------

        /**
        * Initializes OpenAL library
        *@param[out] pOpenALDevice - newly created OpenAL device
        *@param[out] pOpenALContext - newly created OpenAL context
        *@return 1 on success, otherwise 0
        */
        int csrSoundInitializeOpenAL(ALCdevice** pOpenALDevice, ALCcontext** pOpenALContext);

        /**
        * Creates a sound form a wav buffer
        *@param pOpenALDevice - OpenAL device to use
        *@param pOpenALContext - OpenAL context to use
        *@param pBuffer - buffer containing wav sound
        *@param sampling - sampling to use (standard values are e.g. 48000, 44100, ...)
        *@param[out] pBufferID - newly created OpenAL sound buffer identifier (needed to delete the sound)
        *@param[out] pID - newly created OpenAL sound identifier
        *@return 1 on success, otherwise 0
        */
        int csrSoundCreate(const ALCdevice*   pOpenALDevice,
                           const ALCcontext*  pOpenALContext,
                                 CSR_Buffer*  pBuffer,
                                 unsigned int sampling,
                                 ALuint*      pBufferID,
                                 ALuint*      pID);

        /**
        * Plays sound
        *@param id - sound identifier to play
        *@return 1 on success, otherwise 0
        */
        int csrSoundPlay(ALuint id);

        /**
        * Pauses sound
        *@param id - sound identifier to pause
        *@return 1 on success, otherwise 0
        */
        int csrSoundPause(ALuint id);

        /**
        * Stops sound
        *@param id - sound identifier to stop
        *@return 1 on success, otherwise 0
        */
        int csrSoundStop(ALuint id);

        /**
        * Checks if sound is currently playing
        *@param id - sound identifier to check
        *@return 1 if sound is currently playing, otherwise 0
        */
        int csrSoundIsPlaying(ALuint id);

        /**
        * Changes sound pitch
        *@param id - sound identifier for which pitch should be modified
        *@param value - new pitch value (see OpenAL documentation for correct value range)
        *@return 1 on success, otherwise 0
        */
        int csrSoundChangePitch(ALuint id, float pValue);

        /**
        * Changes sound volume
        *@param id - sound identifier for which volume should be modified
        *@param value - new volume value (see OpenAL documentation for correct value range)
        *@return 1 on success, otherwise 0
        */
        int csrSoundChangeVolume(ALuint id, float value);

        /**
        * Changes sound minimum volume limit
        *@param id - sound identifier for which limit should be modified
        *@param value - new minimum volume limit value (see OpenAL documentation for correct value range)
        *@return 1 on success, otherwise 0
        */
        int csrSoundChangeVolumeMin(ALuint id, float value);

        /**
        * Changes sound maximum volume limit
        *@param id - sound identifier for which limit should be modified
        *@param value - new maximum volume limit value (see OpenAL documentation for correct value range)
        *@return 1 on success, otherwise 0
        */
        int csrSoundChangeVolumeMax(ALuint id, float value);

        /**
        * Set sound source position in 3D environment (e.g. sound can be played on the left)
        *@param id - sound identifier to set
        *@param pPos - sound source position
        *@return 1 on success, otherwise 0
        */
        int csrSoundChangePosition(ALuint id, const CSR_Vector3* pPos);

        /**
        * Loops sound when end is reached
        *@param id - sound identifier to loop
        *@param value - if 1, sound will be looped on end reached, otherwise sound will be stopped
        *@return 1 on success, otherwise 0
        */
        void csrSoundLoop(ALuint id, int value);

        /**
        * Releases sound
        *@param bufferID - sound buffer identifier to delete
        *@param id - sound identifier to delete
        */
        void csrSoundRelease(ALuint bufferID, ALuint id);

        /**
        * Releases OpenAL
        *@param pOpenALDevice - OpenAL device to release
        *@param pOpenALContext - OpenAL context to release
        */
        void csrSoundReleaseOpenAL(ALCdevice* pOpenALDevice, ALCcontext* pOpenALContext);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "MiniPlayer.c"
#endif

#endif
