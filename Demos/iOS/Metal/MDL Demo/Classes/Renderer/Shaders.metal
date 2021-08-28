/****************************************************************************
 * ==> Metal shader --------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the Metal shader programs             *
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

#include <metal_stdlib>
#include <simd/simd.h>

// metal
#import "../../../../../../SDK/CSR_Renderer_Metal_Types.h"

using namespace metal;

/**
* Vertex input
*/
typedef struct
{
    vector_float4 m_Position;
    vector_float4 m_TexCoord;
    vector_float4 m_Color;
} CSR_Vertex_In;

/**
* Vertex output
*/
typedef struct
{
    float4 m_Position[[position]];
    float2 m_TexCoord;
    half4  m_Color;
} CSR_Vertex_Out;

//---------------------------------------------------------------------------
// Vertex shader
//---------------------------------------------------------------------------
vertex CSR_Vertex_Out csrVertexShader(device   CSR_Vertex_In* pVertices[[buffer(0)]],
                                      constant CSR_Uniforms&  uniforms[[buffer(1)]],
                                      uint                    vid[[vertex_id]])
{
    CSR_Vertex_Out out;

    // calculate the vertex position
    out.m_Position = uniforms.m_ProjectionMatrix *
                     uniforms.m_ViewMatrix       *
                     uniforms.m_ModelMatrix      *
                     pVertices[vid].m_Position;

    // calculate the vertex texture coordinates
    out.m_TexCoord = pVertices[vid].m_TexCoord.xy;

    // calculate the vertex color
    out.m_Color = half4(pVertices[vid].m_Color);
    
    return out;
}
//---------------------------------------------------------------------------
// Fragment shader
//---------------------------------------------------------------------------
fragment half4 csrFragmentShader(CSR_Vertex_Out  pVertex[[stage_in]],
                                 texture2d<half> pColorMap[[texture(0)]])
{
    constexpr sampler colorSampler(mip_filter::linear,
                                   mag_filter::linear,
                                   min_filter::linear);
    
    half4 colorSample = pColorMap.sample(colorSampler, pVertex.m_TexCoord.xy);
    
    return (colorSample * pVertex.m_Color);
}
//---------------------------------------------------------------------------
