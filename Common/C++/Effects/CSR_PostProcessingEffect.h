/****************************************************************************
 * ==> CSR_PostProcessingEffect --------------------------------------------*
 ****************************************************************************
 * Description : This module provides a base class allowing to create a     *
 *               post-processing effect to apply to the whole scene         *
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

#ifndef CSR_PostProcessingEffectH
#define CSR_PostProcessingEffectH

#include <string>

// compactStar engine
#include "CSR_Renderer_OpenGL.h"

class CSR_PostProcessingEffect
{
    public:
        CSR_PostProcessingEffect();
        virtual ~CSR_PostProcessingEffect();

        /**
        * Changes the effect viewport size
        *@param width - new width to apply
        *@param height - new height to apply
        *@return true on success, otherwise false
        */
        virtual bool ChangeSize(std::size_t width, std::size_t height) = 0;

        /**
        * Begins a draw process at the end of which the effect will be applied
        *@param pColor - scene background color
        *@param pMSAA - multisample antialiasing to apply
        */
        virtual void DrawBegin(const CSR_Color* pColor) const = 0;

        /**
        * Ends the draw process and apply the post processing effect
        */
        virtual void DrawEnd() const = 0;

    protected:
        CSR_OpenGLShader*       m_pShader;
        CSR_OpenGLStaticBuffer* m_pStaticBuffer;
        GLuint                  m_FrameBufferID;
        GLuint                  m_RenderBufferID;
        GLuint                  m_TextureBufferID;
        GLuint                  m_TextureID;

        /**
        * Links a surface on which the effect will be drawn
        *@param pShader - shader on which the surface will be linked
        *@param pParent - parent effect class
        */
        static void OnLinkStaticSurface(const CSR_OpenGLShader* pShader, const void* pParent);
};

#endif
