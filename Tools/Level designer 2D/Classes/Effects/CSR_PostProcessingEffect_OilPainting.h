/****************************************************************************
 * ==> CSR_Shader ----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides an oil painting post-processing       *
 *               effect to apply to the whole scene                         *
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

#ifndef CSR_PostProcessingEffect_OilPaintingH
#define CSR_PostProcessingEffect_OilPaintingH

// classes
#include "CSR_PostProcessingEffect.h"

/**
* Oil painting post-processing effect
*@author Jean-Milost Reymond
*/
class CSR_PostProcessingEffect_OilPainting : public CSR_PostProcessingEffect
{
    public:
        /**
        * Constructor
        *@param width - target viewport width
        *@param height - target viewport height
        *@param radius - effect radius
        */
        CSR_PostProcessingEffect_OilPainting(std::size_t width, std::size_t height, int radius);

        /**
        * Destructor
        */
        virtual ~CSR_PostProcessingEffect_OilPainting();

        /**
        * Changes the effect viewport size
        *@param width - new width to apply
        *@param height - new height to apply
        *@return true on success, otherwise false
        */
        virtual bool ChangeSize(std::size_t width, std::size_t height);

        /**
        * Begins a draw process at the end of which the effect will be applied
        *@param pColor - scene background color
        *@param pMSAA - multisample antialiasing to apply
        */
        virtual void DrawBegin(const CSR_Color* pColor) const;

        /**
        * Ends the draw process and apply the post processing effect
        */
        virtual void DrawEnd() const;

    private:
        std::size_t m_Width;
        std::size_t m_Height;
        int         m_Radius;
        GLint       m_ViewportSlot;
        GLint       m_RadiusSlot;

        /**
        * Initializes the effect
        *@param width - target viewport width
        *@param height - target viewport height
        *@param vertexShader - effect vertex shader program
        *@param vertexShader - effect fragment shader program
        *@return true on success, otherwise false
        */
        bool Initialize(std::size_t  width,
                        std::size_t  height,
                  const std::string& vertexShader,
                  const std::string& fragmentShader);

        /**
        * Configures the effect
        *@param width - target viewport width
        *@param height - target viewport height
        *@return true on success, otherwise false
        */
        bool Configure(std::size_t width, std::size_t height);
};

#endif
