/****************************************************************************
 * ==> CSR_PostProcessingEffect --------------------------------------------*
 ****************************************************************************
 * Description : This module provides a base class allowing to create a     *
 *               post-processing effect to apply to the whole scene         *
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

#include "CSR_PostProcessingEffect.h"

//---------------------------------------------------------------------------
// CSR_PostProcessingEffect
//---------------------------------------------------------------------------
CSR_PostProcessingEffect::CSR_PostProcessingEffect()
{}
//---------------------------------------------------------------------------
CSR_PostProcessingEffect::~CSR_PostProcessingEffect()
{
    // enable the post processing effect shader
    if (m_pShader)
        csrShaderEnable(m_pShader);

    // delete the multisampled texture
    if (m_TextureID)
        glDeleteTextures(1, &m_TextureID);

    // delete the multisampled texture buffer
    if (m_TextureBufferID)
        glDeleteFramebuffers(1, &m_TextureBufferID);

    // delete the render buffer
    if (m_RenderBufferID)
        glDeleteRenderbuffers(1, &m_RenderBufferID);

    // delete the frame buffer
    if (m_FrameBufferID)
        glDeleteFramebuffers(1, &m_FrameBufferID);

    // delete the shader
    if (m_pShader)
        csrOpenGLShaderRelease(m_pShader);

    // delete the static buffer
    if (m_pStaticBuffer)
        csrOpenGLStaticBufferRelease(m_pStaticBuffer);
}
//---------------------------------------------------------------------------
void CSR_PostProcessingEffect::OnLinkStaticSurface(const CSR_OpenGLShader* pShader, const void* pParent)
{
    const CSR_PostProcessingEffect* pPPE    = static_cast<const CSR_PostProcessingEffect*>(pParent);
          CSR_PostProcessingEffect* pEffect = const_cast<CSR_PostProcessingEffect*>(pPPE);

    if (!pEffect)
        return;

    const float vb[] =
    {
        // x,   y,    tu,   tv
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f
    };

    CSR_Buffer buffer;

    // configure the vertex buffer
    buffer.m_pData  = (void*)vb;
    buffer.m_Length = sizeof(vb);

    // create the shape on which the final texture will be drawn
    pEffect->m_pStaticBuffer           = csrOpenGLStaticBufferCreate(pShader, &buffer);
    pEffect->m_pStaticBuffer->m_Stride = 4;
}
//---------------------------------------------------------------------------
