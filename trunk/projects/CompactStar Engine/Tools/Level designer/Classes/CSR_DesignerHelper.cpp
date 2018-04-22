/****************************************************************************
 * ==> CSR_DesignerHelper --------------------------------------------------*
 ****************************************************************************
 * Description : This module provides an helper class for the designer      *
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

#include "CSR_DesignerHelper.h"

//---------------------------------------------------------------------------
std::string CSR_DesignerHelper::GetVertexShader(IEShaderType type)
{
    switch (type)
    {
        case IE_ST_Color:
            return "precision mediump float;"
                   "attribute    vec4 csr_aVertices;"
                   "attribute    vec4 csr_aColor;"
                   "uniform      mat4 csr_uProjection;"
                   "uniform      mat4 csr_uView;"
                   "uniform      mat4 csr_uModel;"
                   "varying lowp vec4 csr_vColor;"
                   "void main(void)"
                   "{"
                   "    csr_vColor   = csr_aColor;"
                   "    gl_Position  = csr_uProjection * csr_uView * csr_uModel * csr_aVertices;"
                   "}";

        case IE_ST_Texture:
            return "precision mediump float;"
                   "attribute    vec4 csr_aVertices;"
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
                   "    gl_Position   = csr_uProjection * csr_uView * csr_uModel * csr_aVertices;"
                   "}";
    }

    return "";
}
//---------------------------------------------------------------------------
std::string CSR_DesignerHelper::GetFragmentShader(IEShaderType type)
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
    }

    return "";
}
//---------------------------------------------------------------------------
std::wstring CSR_DesignerHelper::GetApplicationDir()
{
    return ::ExtractFilePath(Application->ExeName).c_str();
}
//---------------------------------------------------------------------------
std::wstring CSR_DesignerHelper::GetSceneDir()
{
    return GetApplicationDir() + L"Scene\\";
}
//---------------------------------------------------------------------------
