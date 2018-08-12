/****************************************************************************
 * ==> CSR_Weather_Rain ----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a rain effect                         *
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

#include "CSR_Weather_Rain.h"

// std
#include <stdlib.h>
#include <stdexcept>
#include <memory>
#include <math.h>

//---------------------------------------------------------------------------
// CSR_Weather_Rain
//---------------------------------------------------------------------------
CSR_Weather_Rain::CSR_Weather_Rain(      CSR_Scene*   pScene,
                                   const CSR_Box&     rainBox,
                                         float        cloudHeight,
                                   const CSR_Vector3& windForce,
                                         std::size_t  count,
                                         float        dropLength) :
    CSR_Weather(pScene),
    m_RainBox(rainBox),
    m_WindForce(windForce),
    m_CloudHeight(cloudHeight),
    m_DropLength(dropLength)
{
    // initialize the random number generator
    std::srand(0);

    // was the particle engine successfully created?
    if (m_pParticles)
    {
        // calculate the box size
        const float width  = (m_RainBox.m_Max.m_X - m_RainBox.m_Min.m_X) * 1000.0f;
        const float height = -cloudHeight                                * 1000.0f;
        const float depth  = (m_RainBox.m_Max.m_Z - m_RainBox.m_Min.m_Z) * 1000.0f;

        // check if values should be inverted
        const bool invX = width  < 0.0f;
        const bool invY = height < 0.0f;
        const bool invZ = depth  < 0.0f;

        // iterate through the particles to create
        for (std::size_t i = 0; i < count; ++i)
        {
            // add a new particle and set his mass (+/- 4mg for a rain drop)
            CSR_Particle* pParticle    = csrParticlesAdd(m_pParticles);
            pParticle->m_pBody->m_Mass = 0.004f;

            // create a line which will represent the rain drop
            std::auto_ptr<CSR_Line> pLine(new CSR_Line());
            csrLineInit(pLine.get());

            // calculate the particle start position
            const float x = m_RainBox.m_Min.m_X + (float(std::rand() % int(std::fabs(width)))  * (invX ? -0.001 : 0.001));
            const float y = m_RainBox.m_Min.m_Y + (float(std::rand() % int(std::fabs(height))) * (invY ? -0.001 : 0.001));
            const float z = m_RainBox.m_Min.m_Z + (float(std::rand() % int(std::fabs(depth)))  * (invZ ? -0.001 : 0.001));

            // configure the line position
            pLine->m_Start.m_X = x;
            pLine->m_Start.m_Y = y;
            pLine->m_Start.m_Z = z;
            pLine->m_End.m_X   = x;
            pLine->m_End.m_Y   = y - m_DropLength;
            pLine->m_End.m_Z   = z;
            pLine->m_Width     = 1.0f;

            // calculate the particle initial force
            pParticle->m_pBody->m_Velocity.m_X = -10.0f * float(std::rand() % 20)       * windForce.m_X;
            pParticle->m_pBody->m_Velocity.m_Y =  8.0f  + (((2.0f - (y - 1.0f)) * 2.0f) * windForce.m_Y);
            pParticle->m_pBody->m_Velocity.m_Z = -10.0f * float(std::rand() % 20)       * windForce.m_Z;

            // add the line to the scene
            CSR_SceneItem* pItem = csrSceneAddLine(m_pScene, pLine.get(), 0);
            pLine.release();

            // keep the particle model key
            pParticle->m_pKey = pItem->m_pModel;
        }
    }
}
//---------------------------------------------------------------------------
CSR_Weather_Rain::~CSR_Weather_Rain()
{}
//---------------------------------------------------------------------------
void CSR_Weather_Rain::OnCalculateMotion(const CSR_Particles* pParticles,
                                               CSR_Particle*  pParticle,
                                               float          elapsedTime)
{
    // no particle?
    if (!pParticle)
        return;

    // get the rain drop line
    CSR_Line* pLine = static_cast<CSR_Line*>(pParticle->m_pKey);

    // found it?
    if (!pLine)
        return;

    CSR_Vector3 velocity;

    // calculate the particle velocity
    velocity.m_X = pParticle->m_pBody->m_Velocity.m_X * (elapsedTime * 0.001f);
    velocity.m_Y = M_CSR_Gravitation                  * (elapsedTime * 0.3f);
    velocity.m_Z = pParticle->m_pBody->m_Velocity.m_Z * (elapsedTime * 0.001f);

    // calculate the new particle position
    pLine->m_Start.m_X -= velocity.m_X;
    pLine->m_End.m_X   -= velocity.m_X;
    pLine->m_Start.m_Y -= velocity.m_Y;
    pLine->m_End.m_Y   -= velocity.m_Y;
    pLine->m_Start.m_Z -= velocity.m_Z;
    pLine->m_End.m_Z   -= velocity.m_Z;

    // was the particle gone out of the rain box bottom?
    if (pLine->m_Start.m_Y < m_RainBox.m_Max.m_Y)
    {
        // calculate a new start position
        const float height = -m_CloudHeight * 1000.0f;
        const bool  invY   =  height < 0.0f;
        const float y      =  m_RainBox.m_Min.m_Y + (float(std::rand() % int(std::fabs(height))) * (invY ? -0.001 : 0.001));

        // reset the particle
        pParticle->m_pBody->m_Velocity.m_Y = 8.0f + (m_WindForce.m_Y * ((m_CloudHeight - (y - (m_RainBox.m_Min.m_Y - m_CloudHeight))) * 2.0f));
        pLine->m_Start.m_Y                 = y;
        pLine->m_End.m_Y                   = y - m_DropLength;
    }

    // was the particle gone out of the rain box left or right?
    if (pLine->m_Start.m_X < m_RainBox.m_Min.m_X)
    {
        // gone out left, put it on the right side
        pLine->m_Start.m_X = m_RainBox.m_Max.m_X;
        pLine->m_End.m_X   = m_RainBox.m_Max.m_X;
    }
    else
    if (pLine->m_Start.m_X > m_RainBox.m_Max.m_X)
    {
        // gone out right, put it on the left side
        pLine->m_Start.m_X = m_RainBox.m_Min.m_X;
        pLine->m_End.m_X   = m_RainBox.m_Min.m_X;
    }

    // was the particle gone out of the rain box front or back?
    if (pLine->m_Start.m_Z > m_RainBox.m_Min.m_Z)
    {
        // gone out front, put it on the back side
        pLine->m_Start.m_Z = m_RainBox.m_Max.m_Z;
        pLine->m_End.m_Z   = m_RainBox.m_Max.m_Z;
    }
    else
    if (pLine->m_Start.m_Z < m_RainBox.m_Max.m_Z)
    {
        // gone out back, put it on the front side
        pLine->m_Start.m_Z = m_RainBox.m_Min.m_Z;
        pLine->m_End.m_Z   = m_RainBox.m_Min.m_Z;
    }
}
//---------------------------------------------------------------------------
