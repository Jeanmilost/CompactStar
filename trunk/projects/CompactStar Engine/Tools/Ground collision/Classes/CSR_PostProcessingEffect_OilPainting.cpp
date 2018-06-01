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

#include "CSR_PostProcessingEffect_OilPainting.h"

// compactStar engine
#include "CSR_Renderer.h"

//---------------------------------------------------------------------------
// CSR_PostProcessingEffect_OilPainting
//---------------------------------------------------------------------------
CSR_PostProcessingEffect_OilPainting::CSR_PostProcessingEffect_OilPainting(std::size_t width,
                                                                           std::size_t height,
                                                                           int         radius) :
    CSR_PostProcessingEffect(),
    m_Radius(radius),
    m_ViewportSlot(-1),
    m_RadiusSlot(-1)
{
    const std::string vertexShader =
        "precision mediump float;"
        "attribute vec4 csr_vVertex;"
        "void main(void)"
        "{"
        "    gl_Position = vec4(csr_vVertex.x, csr_vVertex.y, 0.0f, 1.0f);"
        "}";

    const std::string fragmentShader =
        "precision mediump float;"
        "uniform sampler2D csr_sTexture;"
        "uniform int       csr_iRadius;"
        "uniform vec2      csr_vViewport;"
        "void main(void)"
        "{"
        "    vec2  src_size = csr_vViewport;"
        "    vec2  uv       = gl_FragCoord.xy / src_size;"
        "    float n        = float((csr_iRadius + 1) * (csr_iRadius + 1));"
        "    vec3  m[4];"
        "    vec3  s[4];"
        "    for (int k = 0; k < 4; ++k)"
        "    {"
        "        m[k] = vec3(0.0);"
        "        s[k] = vec3(0.0);"
        "    }"
        "    struct Window{int x1 , y1 , x2 , y2;};"
        "    Window W[4];"
        "    W[0] = Window(-csr_iRadius, -csr_iRadius, 0,           0);"
        "    W[1] = Window( 0,           -csr_iRadius, csr_iRadius, 0);"
        "    W[2] = Window( 0,            0,           csr_iRadius, csr_iRadius);"
        "    W[3] = Window(-csr_iRadius , 0,           0,           csr_iRadius);"
        "    for (int k = 0; k < 4; ++k)"
        "    {"
        "       for (int j = W[k].y1; j <= W[k].y2; ++j)"
        "       {"
        "            for (int i = W[k].x1; i <= W[k].x2; ++i)"
        "            {"
        "                vec3 c  = texture(csr_sTexture, uv + vec2(i ,j) / src_size).rgb;"
        "                m[k]   += c;"
        "                s[k]   += c * c;"
        "            }"
        "        }"
        "    }"
        "    float min_sigma2 = 1e+2;"
        "    for (int k = 0; k < 4; ++k)"
        "    {"
        "        m[k]         /= n;"
        "        s[k]          = abs(s[k] / n - m[k] * m[k]);"
        "        float sigma2  = s[k].r + s[k].g + s[k].b;"
        "        if (sigma2 < min_sigma2)"
        "        {"
        "            min_sigma2   = sigma2;"
        "            gl_FragColor = vec4(m[k], 1.0);"
        "        }"
        "    }"
        "}";

    if (!Initialize(width, height, vertexShader, fragmentShader))
        throw std::runtime_error("Could not initialize the oil painting post-processing effect");
}
//---------------------------------------------------------------------------
CSR_PostProcessingEffect_OilPainting::~CSR_PostProcessingEffect_OilPainting()
{}
//---------------------------------------------------------------------------
bool CSR_PostProcessingEffect_OilPainting::ChangeSize(std::size_t width, std::size_t height)
{
    // delete the multisampled texture
    if (m_TextureID)
    {
        glDeleteTextures(1, &m_TextureID);
        m_TextureID = M_CSR_Error_Code;
    }

    // delete the multisampled texture buffer
    if (m_TextureBufferID)
    {
        glDeleteFramebuffers(1, &m_TextureBufferID);
        m_TextureBufferID = M_CSR_Error_Code;
    }

    // delete the render buffer
    if (m_RenderBufferID)
    {
        glDeleteRenderbuffers(1, &m_RenderBufferID);
        m_RenderBufferID = M_CSR_Error_Code;
    }

    // delete the frame buffer
    if (m_FrameBufferID)
    {
        glDeleteFramebuffers(1, &m_FrameBufferID);
        m_FrameBufferID = M_CSR_Error_Code;
    }

    // recreate the effect
    return Configure(width, height);
}
//---------------------------------------------------------------------------
void CSR_PostProcessingEffect_OilPainting::DrawBegin(const CSR_Color* pColor) const
{
    // do apply an effect?
    if (m_pShader)
    {
        // enable the effect shader
        csrShaderEnable(m_pShader);

        // enable multisampling
        glEnable(GL_MULTISAMPLE);

        // bind the frame buffer on which the final rendering should be performed
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
    }

    // begin the draw
    csrDrawBegin(pColor);
}
//---------------------------------------------------------------------------
void CSR_PostProcessingEffect_OilPainting::DrawEnd() const
{
    // end the draw
    csrDrawEnd();

    // do finalize the effect?
    if (m_pShader && m_pStaticBuffer)
    {
        // configure the culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glFrontFace(GL_CW);

        // disable the alpha blending
        glDisable(GL_BLEND);

        // set polygon mode to fill
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // enable the MSAA shader
        csrShaderEnable(m_pShader);

        // blit the multisampled buffer containing the drawing to the output texture buffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FrameBufferID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_TextureBufferID);
        glBlitFramebuffer(0,
                          0,
                          m_Width,
                          m_Height,
                          0,
                          0,
                          m_Width,
                          m_Height,
                          GL_COLOR_BUFFER_BIT,
                          GL_NEAREST);

        // unbind the frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // configure the depth testing
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        // set the viewport values
        glUniform2f(m_ViewportSlot, float(m_Width), float(m_Height));

        // set the radius value
        glUniform1i(m_RadiusSlot, m_Radius);

        // select the texture sampler to use (GL_TEXTURE0 for normal textures)
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(m_pShader->m_TextureSlot, GL_TEXTURE0);

        // bind the texure to use
        glBindTexture(GL_TEXTURE_2D, m_TextureID);

        // bind the VBO containing the shape to draw
        glBindBuffer(GL_ARRAY_BUFFER, m_pStaticBuffer->m_BufferID);

        // enable the vertices
        glEnableVertexAttribArray(m_pShader->m_VertexSlot);
        glVertexAttribPointer(m_pShader->m_VertexSlot,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              m_pStaticBuffer->m_Stride * sizeof(float),
                              0);

        // enable the texture coordinates
        glEnableVertexAttribArray(m_pShader->m_TexCoordSlot);
        glVertexAttribPointer(m_pShader->m_TexCoordSlot,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              m_pStaticBuffer->m_Stride * sizeof(float),
                              (void*)(2 * sizeof(float)));

        // draw the surface
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // disable the vertex attribute arrays
        glDisableVertexAttribArray(m_pShader->m_TexCoordSlot);
        glDisableVertexAttribArray(m_pShader->m_VertexSlot);

        // unbind the VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
//---------------------------------------------------------------------------
bool CSR_PostProcessingEffect_OilPainting::Initialize(      std::size_t  width,
                                                            std::size_t  height,
                                                      const std::string& vertexShader,
                                                      const std::string& fragmentShader)
{
    // load the shader to use for MSAA
    m_pShader = csrShaderLoadFromStr(vertexShader.c_str(),
                                     vertexShader.length(),
                                     fragmentShader.c_str(),
                                     fragmentShader.length(),
                                     OnLinkStaticSurface,
                                     this);

    // succeeded?
    if (!m_pShader)
        return false;

    // the static buffer containing the final surface to show was generated successfully?
    if (!m_pStaticBuffer)
        return false;

    // enable the effect shader
    csrShaderEnable(m_pShader);

    // get the vertex slot
    m_pShader->m_VertexSlot = glGetAttribLocation(m_pShader->m_ProgramID, "csr_vVertex");

    // found it?
    if (m_pShader->m_VertexSlot == -1)
        return false;

    // get the texture sampler slot
    m_pShader->m_TextureSlot = glGetUniformLocation(m_pShader->m_ProgramID, "csr_sTexture");

    // found it?
    if (m_pShader->m_TextureSlot == -1)
        return false;

    // get the viewport slot
    m_ViewportSlot = glGetUniformLocation(m_pShader->m_ProgramID, "csr_vViewport");

    // found it?
    if (m_ViewportSlot == -1)
        return false;

    // get the radius slot
    m_RadiusSlot = glGetUniformLocation(m_pShader->m_ProgramID, "csr_iRadius");

    // found it?
    if (m_RadiusSlot == -1)
        return false;

    // configure the effect
    return Configure(width, height);
}
//---------------------------------------------------------------------------
bool CSR_PostProcessingEffect_OilPainting::Configure(std::size_t width, std::size_t height)
{
    GLuint msTexture;
    GLuint texture;

    // enable multisampling
    glEnable(GL_MULTISAMPLE);

    // create and bind the frame buffer
    glGenFramebuffers(1, &m_FrameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

    // create a new texture
    glGenTextures(1, &msTexture);

    // bind texture to sampler
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msTexture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 1, GL_RGB, width, height, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    // add texture to the frame buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D_MULTISAMPLE,
                           msTexture,
                           0);

    // create and bind the render buffer for depth and stencil attachments
    glGenRenderbuffers(1, &m_RenderBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER,
                                     1,
                                     GL_DEPTH24_STENCIL8,
                                     width,
                                     height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER,
                              m_RenderBufferID);

    // build the render buffer
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    // unbind the frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // create multisampled output texture
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    // configure texture filters to use and bind texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // create and bind a multisampled texture buffer
    glGenFramebuffers(1, &m_TextureBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_TextureBufferID);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           m_TextureID,
                           0);

    // build the multisampled texture buffer
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    // unbind the texture buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // set the viewport size
    m_Width  = width;
    m_Height = height;

    return true;
}
//---------------------------------------------------------------------------
