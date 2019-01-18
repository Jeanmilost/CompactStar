/****************************************************************************
 * ==> CSR_Weather ---------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a base class for weather effects      *
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

#ifndef CSR_WeatherH
#define CSR_WeatherH

// compactStar engine
#include "CSR_Scene.h"
#include "CSR_Particles.h"

/**
* Base class for weather effects
*@author Jean-Milost Reymond
*/
class CSR_Weather
{
    public:
        /**
        * Constructor
        *@param pScene - scene above which the weather effect will be drawn
        */
        CSR_Weather(CSR_Scene* pScene);

        virtual ~CSR_Weather();

        /**
        * Gets the weather particle system key
        *@return the weather particle system key
        */
        virtual void* GetKey();

        /**
        * Animates the weather
        *@param elapedTime - elapsed time since the last animation
        */
        virtual void Animate(float elapsedTime);

        /**
        * Called when a motion should be calculated for a particle
        *@param pParticles - particle system at which the particle belongs
        *@param pParticle - particle for which the motion should be calculated
        *@param elapedTime - elapsed time since last animation, in milliseconds
        */
        virtual void OnCalculateMotion(const CSR_Particles* pParticles,
                                             CSR_Particle*  pParticle,
                                             float          elapsedTime) = 0;

        /**
        * Sets OnCalculateMotion callback
        *@param fHandler - function handler
        */
        virtual void Set_OnCalculateMotion_Callback(CSR_fOnCalculateMotion fHandle);

    protected:
        CSR_Scene*     m_pScene;
        CSR_Particles* m_pParticles;
};

#endif
