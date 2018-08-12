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

#ifndef CSR_Weather_RainH
#define CSR_Weather_RainH

// classes
#include "CSR_Weather.h"

class CSR_Weather_Rain : public CSR_Weather
{
    public:
        /**
        * Constructor
        * Constructor
        *@param pScene - scene above which the rain effect will be drawn
        *@param rainBox - box in which the rain will fail
        *@param cloudHeight - height in the rain box where the rain drops will be created
        *@param windForce - wind force (between 0.0 and 1.0)
        *@param count - particle count to generate
        *@param dropLength - rain drop length
        */
        CSR_Weather_Rain(      CSR_Scene*   pScene,
                         const CSR_Box&     rainBox,
                               float        cloudHeight,
                         const CSR_Vector3& windForce,
                               std::size_t  count,
                               float        dropLength);

        virtual ~CSR_Weather_Rain();

        /**
        * Called when a motion should be calculated for a particle
        *@param pParticles - particle system at which the particle belongs
        *@param pParticle - particle for which the motion should be calculated
        *@param elapedTime - elapsed time since last animation, in milliseconds
        */
        virtual void OnCalculateMotion(const CSR_Particles* pParticles,
                                             CSR_Particle*  pParticle,
                                             float          elapsedTime);

    private:
        CSR_Box     m_RainBox;
        CSR_Vector3 m_WindForce;
        float       m_CloudHeight;
        float       m_DropLength;
};

#endif
