/****************************************************************************
 * ==> CSR_Weather ---------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a base class for weather effects      *
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

#include "CSR_Weather.h"

//---------------------------------------------------------------------------
// CSR_Weather
//---------------------------------------------------------------------------
CSR_Weather::CSR_Weather(CSR_Scene* pScene) :
    m_pScene(pScene),
    m_pParticles(NULL)
{
    m_pParticles = csrParticlesCreate();
}
//---------------------------------------------------------------------------
CSR_Weather::~CSR_Weather()
{
    csrParticlesRelease(m_pParticles);
}
//---------------------------------------------------------------------------
void* CSR_Weather::GetKey()
{
    return m_pParticles;
}
//---------------------------------------------------------------------------
void CSR_Weather::Animate(float elapsedTime)
{
    csrParticlesAnimate(m_pParticles, elapsedTime);
}
//---------------------------------------------------------------------------
void CSR_Weather::Set_OnCalculateMotion_Callback(CSR_fOnCalculateMotion fHandle)
{
    if (!m_pParticles)
        return;

    m_pParticles->m_fOnCalculateMotion = fHandle;
}
//---------------------------------------------------------------------------
