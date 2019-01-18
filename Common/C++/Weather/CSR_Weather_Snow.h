/****************************************************************************
 * ==> CSR_Weather_Snow ----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a snow effect                         *
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

#ifndef CSR_Weather_SnowH
#define CSR_Weather_SnowH

// classes
#include "CSR_Weather.h"

// compactStar engine
#include "CSR_Geometry.h"

class CSR_Weather_Snow : public CSR_Weather
{
    public:
        /**
        * Constructor
        *@param pScene - scene above which the snow effect will be drawn
        *@param snowBox - box in which the snow will fail
        *@param cloudHeight - height in the snow box where the snowflakes will be created
        *@param windForce - wind force (between 0.0 and 1.0)
        *@param count - particle count to generate
        *@param useSphere - if true, sphere will be used as model instead of disk
        */
        CSR_Weather_Snow(      CSR_Scene*   pScene,
                         const CSR_Box&     snowBox,
                               float        cloudHeight,
                         const CSR_Vector3& initialForce,
                               std::size_t  count,
                               bool         useSphere);

        virtual ~CSR_Weather_Snow();

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
        CSR_Box     m_SnowBox;
        CSR_Vector3 m_WindForce;
        float       m_CloudHeight;
};

#endif
