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
int csrSoundCreate(const ALCdevice*   pOpenALDevice,
                   const ALCcontext*  pOpenALContext,
                         CSR_Buffer*  pBuffer,
                         unsigned int sampling,
                         ALuint*      pBufferID,
                         ALuint*      pID)
{
    ALuint bufferID;
    ALuint id;

    // no sound file to load?
    if (!pBuffer->m_pData || !pBuffer->m_Length)
        return 0;

    // no OpenAL device?
    if (!pOpenALDevice)
        return 0;

    // no OpenAL context?
    if (!pOpenALContext)
        return 0;

    // grab a buffer ID from openAL
    alGenBuffers(1, &bufferID);

    // jam the audio data into the new buffer
    alBufferData(bufferID, AL_FORMAT_STEREO16, pBuffer->m_pData, pBuffer->m_Length, sampling);

    // grab a source ID from openAL
    alGenSources(1, &id);

    // attach the buffer to the source
    alSourcei(id, AL_BUFFER, bufferID);

    // set some basic source preferences
    alSourcef(id, AL_GAIN,  1.0f);
    alSourcef(id, AL_PITCH, 1.0f);

    *pBufferID = bufferID;
    *pID       = id;

    return 1;
}
//---------------------------------------------------------------------------
int csrSoundPlay(ALuint id)
{
    if (id == M_OPENAL_ERROR_ID)
        return 0;

    alSourcePlay(id);
    return 1;
}
//---------------------------------------------------------------------------
int csrSoundPause(ALuint id)
{
    if (id == M_OPENAL_ERROR_ID)
        return 0;

    alSourcePause(id);
    return 1;
}
//---------------------------------------------------------------------------
int csrSoundStop(ALuint id)
{
    if (id == M_OPENAL_ERROR_ID)
        return 0;

    alSourceStop(id);
    return 1;
}
//---------------------------------------------------------------------------
int csrSoundIsPlaying(ALuint id)
{
    ALenum state;

    if (id == M_OPENAL_ERROR_ID)
        return 0;

    alGetSourcei(id, AL_SOURCE_STATE, &state);

    if (state == AL_PLAYING)
        return 1;

    return 0;
}
//---------------------------------------------------------------------------
int csrSoundChangePitch(ALuint id, float value)
{
    if (id == M_OPENAL_ERROR_ID)
        return 0;

    if (value >= 0.0f && value <= 1.0f)
    {
        alSourcef(id, AL_PITCH, value);
        return 1;
    }

    return 0;
}
//---------------------------------------------------------------------------
int csrSoundChangeVolume(ALuint id, float value)
{
    if (id == M_OPENAL_ERROR_ID)
        return 0;

    if (value >= 0.0f && value <= 1.0f)
    {
        alSourcef(id, AL_GAIN, value);
        return 1;
    }

    return 0;
}
//---------------------------------------------------------------------------
int csrSoundChangeVolumeMin(ALuint id, float value)
{
    if (id == M_OPENAL_ERROR_ID)
        return 0;

    if (value >= 0.0f && value <= 1.0f)
    {
        alSourcef(id, AL_MIN_GAIN, value);
        return 1;
    }

    return 0;
}
//---------------------------------------------------------------------------
int csrSoundChangeVolumeMax(ALuint id, float value)
{
    if (id == M_OPENAL_ERROR_ID)
        return 0;

    if (value >= 0.0f && value <= 1.0f)
    {
        alSourcef(id, AL_MAX_GAIN, value);
        return 1;
    }

    return 0;
}
//---------------------------------------------------------------------------
int csrSoundChangePosition(ALuint id, const CSR_Vector3* pPos)
{
    ALfloat position[3];

    if (id == M_OPENAL_ERROR_ID)
        return 0;

    position[0] = pPos->m_X;
    position[1] = pPos->m_Y;
    position[2] = pPos->m_Z;

    alSourcefv(id, AL_POSITION, position);
    return 1;
}
//---------------------------------------------------------------------------
void csrSoundLoop(ALuint id, int value)
{
    if (id != M_OPENAL_ERROR_ID)
        if (value)
            alSourcei(id, AL_LOOPING, AL_TRUE);
        else
            alSourcei(id, AL_LOOPING, AL_FALSE);
}
//---------------------------------------------------------------------------
void csrSoundRelease(ALuint bufferID, ALuint id)
{
    // delete source
    if (id != M_OPENAL_ERROR_ID)
        alDeleteSources(1, &id);

    // delete buffer
    if (bufferID != M_OPENAL_ERROR_ID)
        alDeleteBuffers(1, &bufferID);
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
