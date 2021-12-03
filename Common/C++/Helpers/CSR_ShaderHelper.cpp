/****************************************************************************
 * ==> CSR_ShaderHelper ----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides an helper class for the shader        *
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

#include "CSR_ShaderHelper.h"

// compactStar engine
#include "CSR_Model.h"

//---------------------------------------------------------------------------
// CSR_ShaderHelper
//---------------------------------------------------------------------------
std::string CSR_ShaderHelper::GetVertexShader(IEShaderType type)
{
    switch (type)
    {
        case IE_ST_Color:
            return "precision mediump float;"
                   "attribute    vec3 csr_aVertices;"
                   "attribute    vec4 csr_aColor;"
                   "uniform      mat4 csr_uProjection;"
                   "uniform      mat4 csr_uView;"
                   "uniform      mat4 csr_uModel;"
                   "varying lowp vec4 csr_vColor;"
                   "void main(void)"
                   "{"
                   "    csr_vColor   = csr_aColor;"
                   "    gl_Position  = csr_uProjection * csr_uView * csr_uModel * vec4(csr_aVertices, 1.0);"
                   "}";

        case IE_ST_Texture:
            return "precision mediump float;"
                   "attribute    vec3 csr_aVertices;"
                   "attribute    vec4 csr_aColor;"
                   "attribute    vec2 csr_aTexCoord;"
                   "uniform      mat4 csr_uProjection;"
                   "uniform      mat4 csr_uView;"
                   "uniform      mat4 csr_uModel;"
                   "varying lowp vec4 csr_vColor;"
                   "varying      vec2 csr_vTexCoord;"
                   "void main(void)"
                   "{"
                   "    csr_vColor    = csr_aColor;"
                   "    csr_vTexCoord = csr_aTexCoord;"
                   "    gl_Position   = csr_uProjection * csr_uView * csr_uModel * vec4(csr_aVertices, 1.0);"
                   "}";

        case IE_ST_Skybox:
            return "precision mediump float;"
                   "attribute vec3 csr_aVertices;"
                   "uniform   mat4 csr_uProjection;"
                   "uniform   mat4 csr_uView;"
                   "varying   vec3 csr_vTexCoord;"
                   "void main()"
                   "{"
                   "    csr_vTexCoord = csr_aVertices;"
                   "    gl_Position   = csr_uProjection * csr_uView * vec4(csr_aVertices, 1.0);"
                   "}";
    }

    return "";
}
//---------------------------------------------------------------------------
std::string CSR_ShaderHelper::GetFragmentShader(IEShaderType type)
{
    switch (type)
    {
        case IE_ST_Color:
            return "precision mediump float;"
                   "varying lowp vec4 csr_vColor;"
                   "void main(void)"
                   "{"
                   "    gl_FragColor = csr_vColor;"
                   "}";

        case IE_ST_Texture:
            return "precision mediump float;"
                   "uniform      sampler2D csr_sTexture;"
                   "varying lowp vec4      csr_vColor;"
                   "varying      vec2      csr_vTexCoord;"
                   "void main(void)"
                   "{"
                   "    gl_FragColor = csr_vColor * texture2D(csr_sTexture, csr_vTexCoord);"
                   "}";

        case IE_ST_Skybox:
            #ifdef __APPLE__
                return "precision mediump float;"
                       "uniform samplerCube csr_sCubemap;"
                       "varying vec3        csr_vTexCoord;"
                       "void main()"
                       "{"
                       "    gl_FragColor = textureCube(csr_sCubemap, csr_vTexCoord);"
                       "}";
            #else
                return "precision mediump float;"
                       "uniform samplerCube csr_sCubemap;"
                       "varying vec3        csr_vTexCoord;"
                       "void main()"
                       "{"
                       "    gl_FragColor = texture(csr_sCubemap, csr_vTexCoord);"
                       "}";
            #endif
    }

    return "";
}
//---------------------------------------------------------------------------
