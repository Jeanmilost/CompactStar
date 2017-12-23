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

#include "CSR_Sound.h"

// std
#include <stdio.h>
#include <stdlib.h>

//---------------------------------------------------------------------------
// Sound functions
//---------------------------------------------------------------------------
int csrSoundInitializeOpenAL(ALCdevice** pOpenALDevice, ALCcontext** pOpenALContext)
{
    // select the "preferred device"
    *pOpenALDevice = alcOpenDevice(0);

    // found it?
    if (!pOpenALDevice)
        return 0;

    // use the device to make a context
    *pOpenALContext = alcCreateContext(*pOpenALDevice, 0);

    // found it?
    if (!(*pOpenALContext))
        return 0;

    // set context to the currently active one
    alcMakeContextCurrent(*pOpenALContext);

    return 1;
}
//---------------------------------------------------------------------------
CSR_Sound* csrSoundCreate(const ALCdevice*   pOpenALDevice,
                          const ALCcontext*  pOpenALContext,
                                CSR_Buffer*  pBuffer,
                                unsigned int sampling)
{
    CSR_Sound* pSound;

    // no sound file to load?
    if (!pBuffer->m_pData || !pBuffer->m_Length)
        return 0;

    // no OpenAL device?
    if (!pOpenALDevice)
        return 0;

    // no OpenAL context?
    if (!pOpenALContext)
        return 0;

    // create a new sound container
    pSound = (CSR_Sound*)malloc(sizeof(CSR_Sound));

    // succeeded?
    if (!pSound)
        return 0;

    // grab a buffer ID from openAL
    alGenBuffers(1, &pSound->m_BufferID);

    // succeeded?
    if (alGetError() != AL_NO_ERROR)
    {
        pSound->m_ID       = M_OPENAL_ERROR_ID;
        pSound->m_BufferID = M_OPENAL_ERROR_ID;
        return pSound;
    }

    // jam the audio data into the new buffer
    alBufferData(pSound->m_BufferID,
                 AL_FORMAT_STEREO16,
                 pBuffer->m_pData,
                 pBuffer->m_Length,
                 sampling);

    // succeeded?
    if (alGetError() != AL_NO_ERROR)
    {
        alDeleteBuffers(1, &pSound->m_BufferID);

        pSound->m_ID       = M_OPENAL_ERROR_ID;
        pSound->m_BufferID = M_OPENAL_ERROR_ID;
        return pSound;
    }

    // grab a source ID from openAL
    alGenSources(1, &pSound->m_ID);

    // succeeded?
    if (alGetError() != AL_NO_ERROR)
    {
        alDeleteBuffers(1, &pSound->m_BufferID);

        pSound->m_ID       = M_OPENAL_ERROR_ID;
        pSound->m_BufferID = M_OPENAL_ERROR_ID;
        return pSound;
    }

    // attach the buffer to the source
    alSourcei(pSound->m_ID, AL_BUFFER, pSound->m_BufferID);

    // succeeded?
    if (alGetError() != AL_NO_ERROR)
    {
        alDeleteSources(1, &pSound->m_ID);
        alDeleteBuffers(1, &pSound->m_BufferID);

        pSound->m_ID       = M_OPENAL_ERROR_ID;
        pSound->m_BufferID = M_OPENAL_ERROR_ID;
        return pSound;
    }

    // set some basic source preferences
    alSourcef(pSound->m_ID, AL_GAIN,  1.0f);
    alSourcef(pSound->m_ID, AL_PITCH, 1.0f);

    return pSound;
}
//---------------------------------------------------------------------------
CSR_Sound* csrSoundOpen(const ALCdevice*   pOpenALDevice,
                        const ALCcontext*  pOpenALContext,
                              const char*  pFileName,
                              unsigned int sampling)
{
    CSR_Buffer* pBuffer;
    CSR_Sound*  pSound;

    // open the sound file
    pBuffer = csrFileOpen(pFileName);

    // succeeded?
    if (!pBuffer || !pBuffer->m_Length)
        return 0;

    // create the sound from file content
    pSound = csrSoundCreate(pOpenALDevice, pOpenALContext, pBuffer, sampling);

    // release the file buffer (no longer required)
    csrBufferRelease(pBuffer);

    return pSound;
}
//---------------------------------------------------------------------------
int csrSoundPlay(CSR_Sound* pSound)
{
    if (!pSound || pSound->m_ID == M_OPENAL_ERROR_ID)
        return 0;

    alSourcePlay(pSound->m_ID);
    return 1;
}
//---------------------------------------------------------------------------
int csrSoundPause(CSR_Sound* pSound)
{
    if (!pSound || pSound->m_ID == M_OPENAL_ERROR_ID)
        return 0;

    alSourcePause(pSound->m_ID);
    return 1;
}
//---------------------------------------------------------------------------
int csrSoundStop(CSR_Sound* pSound)
{
    if (!pSound || pSound->m_ID == M_OPENAL_ERROR_ID)
        return 0;

    alSourceStop(pSound->m_ID);
    return 1;
}
//---------------------------------------------------------------------------
int csrSoundIsPlaying(CSR_Sound* pSound)
{
    ALenum state;

    if (!pSound || pSound->m_ID == M_OPENAL_ERROR_ID)
        return 0;

    alGetSourcei(pSound->m_ID, AL_SOURCE_STATE, &state);

    if (state == AL_PLAYING)
        return 1;

    return 0;
}
//---------------------------------------------------------------------------
int csrSoundChangePitch(CSR_Sound* pSound, float value)
{
    if (!pSound || pSound->m_ID == M_OPENAL_ERROR_ID)
        return 0;

    if (value >= 0.0f && value <= 1.0f)
    {
        alSourcef(pSound->m_ID, AL_PITCH, value);
        return 1;
    }

    return 0;
}
//---------------------------------------------------------------------------
int csrSoundChangeVolume(CSR_Sound* pSound, float value)
{
    if (!pSound || pSound->m_ID == M_OPENAL_ERROR_ID)
        return 0;

    if (value >= 0.0f && value <= 1.0f)
    {
        alSourcef(pSound->m_ID, AL_GAIN, value);
        return 1;
    }

    return 0;
}
//---------------------------------------------------------------------------
int csrSoundChangeVolumeMin(CSR_Sound* pSound, float value)
{
    if (!pSound || pSound->m_ID == M_OPENAL_ERROR_ID)
        return 0;

    if (value >= 0.0f && value <= 1.0f)
    {
        alSourcef(pSound->m_ID, AL_MIN_GAIN, value);
        return 1;
    }

    return 0;
}
//---------------------------------------------------------------------------
int csrSoundChangeVolumeMax(CSR_Sound* pSound, float value)
{
    if (!pSound || pSound->m_ID == M_OPENAL_ERROR_ID)
        return 0;

    if (value >= 0.0f && value <= 1.0f)
    {
        alSourcef(pSound->m_ID, AL_MAX_GAIN, value);
        return 1;
    }

    return 0;
}
//---------------------------------------------------------------------------
int csrSoundChangePosition(CSR_Sound* pSound, const CSR_Vector3* pPos)
{
    ALfloat position[3];

    if (!pSound || pSound->m_ID == M_OPENAL_ERROR_ID)
        return 0;

    position[0] = pPos->m_X;
    position[1] = pPos->m_Y;
    position[2] = pPos->m_Z;

    alSourcefv(pSound->m_ID, AL_POSITION, position);
    return 1;
}
//---------------------------------------------------------------------------
void csrSoundLoop(CSR_Sound* pSound, int value)
{
    if (!pSound || pSound->m_ID == M_OPENAL_ERROR_ID)
        return;

    if (value)
        alSourcei(pSound->m_ID, AL_LOOPING, AL_TRUE);
    else
        alSourcei(pSound->m_ID, AL_LOOPING, AL_FALSE);
}
//---------------------------------------------------------------------------
void csrSoundRelease(CSR_Sound* pSound)
{
    // no sound to release?
    if (!pSound)
        return;

    // delete source
    if (pSound->m_ID != M_OPENAL_ERROR_ID)
        alDeleteSources(1, &pSound->m_ID);

    // delete buffer
    if (pSound->m_BufferID != M_OPENAL_ERROR_ID)
        alDeleteBuffers(1, &pSound->m_BufferID);

    // delete the sound container
    free(pSound);
}
//---------------------------------------------------------------------------
void csrSoundReleaseOpenAL(ALCdevice* pOpenALDevice, ALCcontext* pOpenALContext)
{
    // release context
    if (pOpenALContext)
        alcDestroyContext(pOpenALContext);

    // close device
    if (pOpenALDevice)
        alcCloseDevice(pOpenALDevice);
}
//---------------------------------------------------------------------------
