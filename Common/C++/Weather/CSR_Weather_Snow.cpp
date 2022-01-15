/****************************************************************************
 * ==> CSR_Weather_Snow ----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a snow effect                         *
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

#include "CSR_Weather_Snow.h"

// std
#include <stdlib.h>
#include <stdexcept>
#include <memory>
#include <math.h>

// compactStar engine
#include "CSR_Model.h"

//---------------------------------------------------------------------------
// CSR_Weather_Snow
//---------------------------------------------------------------------------
CSR_Weather_Snow::CSR_Weather_Snow(      CSR_Scene*   pScene,
                                   const CSR_Box&     snowBox,
                                         float        cloudHeight,
                                   const CSR_Vector3& windForce,
                                         std::size_t  count,
                                         bool         useSphere) :
    CSR_Weather(pScene),
    m_SnowBox(snowBox),
    m_WindForce(windForce),
    m_CloudHeight(cloudHeight)
{
    // initialize the random number generator
    std::srand(0);

    // was the particle engine successfully created?
    if (m_pParticles)
    {
        std::auto_ptr<CSR_Mesh> pMesh;

        // create the snow particle model
        if (!useSphere)
            pMesh.reset(csrShapeCreateDisk(0.0f, 0.0f, 0.01f, 5, 0, 0, 0, 0));
        else
            pMesh.reset(csrShapeCreateSphere(0.01f, 5, 5, 0, 0, 0, 0));

        // add it to the scene
        CSR_SceneItem* pItem = csrSceneAddMesh(m_pScene, pMesh.get(), 0, 0);

        // succeeded?
        if (!pItem)
            std::runtime_error("Could not create the snow particle model");

        // calculate the box size
        const float width  = (m_SnowBox.m_Max.m_X - m_SnowBox.m_Min.m_X) * 1000.0f;
        const float height = -cloudHeight                                * 1000.0f;
        const float depth  = (m_SnowBox.m_Max.m_Z - m_SnowBox.m_Min.m_Z) * 1000.0f;

        // check if values should be inverted
        const bool invX = width  < 0.0f;
        const bool invY = height < 0.0f;
        const bool invZ = depth  < 0.0f;

        // iterate through the particles to create
        for (std::size_t i = 0; i < count; ++i)
        {
            // add a new particle and set his mass (+/- 4mg for a snowflake)
            CSR_Particle* pParticle    = csrParticlesAdd(m_pParticles);
            pParticle->m_pBody->m_Mass = 0.004f;

            // calculate the particle start position
            const float x = m_SnowBox.m_Min.m_X + (float(std::rand() % int(std::fabs(width)))  * (invX ? -0.001 : 0.001));
            const float y = m_SnowBox.m_Min.m_Y + (float(std::rand() % int(std::fabs(height))) * (invY ? -0.001 : 0.001));
            const float z = m_SnowBox.m_Min.m_Z + (float(std::rand() % int(std::fabs(depth)))  * (invZ ? -0.001 : 0.001));

            // calculate the particle initial force
            pParticle->m_pBody->m_Velocity.m_X = -10.0f *  windForce.m_X * float(std::rand() % 20);
            pParticle->m_pBody->m_Velocity.m_Y =  12.0f + (windForce.m_Y * ((m_CloudHeight - (y - (m_SnowBox.m_Min.m_Y - m_CloudHeight))) * 2.0f));
            pParticle->m_pBody->m_Velocity.m_Z = -10.0f *  windForce.m_Z * float(std::rand() % 20);

            // configure the particle matrix (was set to identity while the particle was created)
            pParticle->m_pMatrix->m_Table[3][0] = x;
            pParticle->m_pMatrix->m_Table[3][1] = y;
            pParticle->m_pMatrix->m_Table[3][2] = z;

            // add it to the scene
            csrSceneAddModelMatrix(m_pScene, pMesh.get(), pParticle->m_pMatrix);

            // keep the particle model key
            pParticle->m_pKey = pItem->m_pModel;
        }

        pMesh.release();
    }
}
//---------------------------------------------------------------------------
CSR_Weather_Snow::~CSR_Weather_Snow()
{}
//---------------------------------------------------------------------------
void CSR_Weather_Snow::OnCalculateMotion(const CSR_Particles* pParticles,
                                               CSR_Particle*  pParticle,
                                               float          elapsedTime)
{
    // no particle?
    if (!pParticle)
        return;

    // calculate the new particle position
    pParticle->m_pMatrix->m_Table[3][0] += pParticle->m_pBody->m_Velocity.m_X * (elapsedTime * 0.001f);
    pParticle->m_pMatrix->m_Table[3][1] -= M_CSR_Gravitation                  * (elapsedTime * 0.1f);
    pParticle->m_pMatrix->m_Table[3][2] += pParticle->m_pBody->m_Velocity.m_Z * (elapsedTime * 0.001f);

    // was the particle gone out of the snow box bottom?
    if (pParticle->m_pMatrix->m_Table[3][1] < m_SnowBox.m_Max.m_Y)
    {
        // calculate a new start position
        const float height = -m_CloudHeight * 1000.0f;
        const bool  invY   =  height < 0.0f;
        const float y      =  m_SnowBox.m_Min.m_Y + (float(std::rand() % int(std::fabs(height))) * (invY ? -0.001 : 0.001));

        // reset the particle
        pParticle->m_pBody->m_Velocity.m_Y =
                12.0f + (m_WindForce.m_Y * ((m_CloudHeight - (y - (m_SnowBox.m_Min.m_Y - m_CloudHeight))) * 2.0f));
        pParticle->m_pMatrix->m_Table[3][1]        = y;
    }

    // was the particle gone out of the rain box left or right?
    if (pParticle->m_pMatrix->m_Table[3][0] < m_SnowBox.m_Min.m_X)
        // gone out left, put it on the right side
        pParticle->m_pMatrix->m_Table[3][0] = m_SnowBox.m_Max.m_X;
    else
    if (pParticle->m_pMatrix->m_Table[3][0] > m_SnowBox.m_Max.m_X)
        // gone out right, put it on the left side
        pParticle->m_pMatrix->m_Table[3][0] = m_SnowBox.m_Min.m_X;

    // was the particle gone out of the rain box front or back?
    if (pParticle->m_pMatrix->m_Table[3][2] > m_SnowBox.m_Min.m_Z)
        // gone out front, put it on the back side
        pParticle->m_pMatrix->m_Table[3][2] = m_SnowBox.m_Max.m_Z;
    else
    if (pParticle->m_pMatrix->m_Table[3][2] < m_SnowBox.m_Max.m_Z)
        // gone out back, put it on the front side
        pParticle->m_pMatrix->m_Table[3][2] = m_SnowBox.m_Min.m_Z;
}
//---------------------------------------------------------------------------
