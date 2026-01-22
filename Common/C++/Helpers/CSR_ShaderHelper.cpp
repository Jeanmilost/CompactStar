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
        #ifdef _MSC_VER
            case CSR_ShaderHelper::IEShaderType::IE_ST_Color:
        #else
            case IE_ST_Color:
        #endif
            #ifndef __APPLE__
                return "#version 120\n"
                       "precision mediump float;"
            #else
                return "precision mediump float;"
            #endif
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

        #ifdef _MSC_VER
            case CSR_ShaderHelper::IEShaderType::IE_ST_Texture:
        #else
            case IE_ST_Texture:
        #endif
            #ifndef __APPLE__
                return "#version 120\n"
                       "precision mediump float;"
            #else
                return "precision mediump float;"
            #endif
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

        #ifdef _MSC_VER
            case CSR_ShaderHelper::IEShaderType::IE_ST_Skybox:
        #else
            case IE_ST_Skybox:
        #endif
            #ifndef __APPLE__
                return "#version 120\n"
                       "precision mediump float;"
            #else
                return "precision mediump float;"
            #endif
                       "attribute vec3 csr_aVertices;"
                       "uniform   mat4 csr_uProjection;"
                       "uniform   mat4 csr_uView;"
                       "varying   vec3 csr_vTexCoord;"
                       "void main()"
                       "{"
                       "    csr_vTexCoord = csr_aVertices;"
                       "    gl_Position   = csr_uProjection * csr_uView * vec4(csr_aVertices, 1.0);"
                       "}";

        #ifdef _MSC_VER
            case CSR_ShaderHelper::IEShaderType::IE_ST_Line:
        #else
            case IE_ST_Line:
        #endif
            #ifndef __APPLE__
                return "#version 120\n"
                       "precision mediump float;"
            #else
                return "precision mediump float;"
            #endif
                       "attribute    vec3 csr_aVertices;"
                       "attribute    vec4 csr_aColor;"
                       "uniform      mat4 csr_uProjection;"
                       "uniform      mat4 csr_uView;"
                       "uniform      mat4 csr_uModel;"
                       "varying lowp vec4 csr_vColor;"
                       "void main(void)"
                       "{"
                       "    csr_vColor  = csr_aColor;"
                       "    gl_Position = csr_uProjection * csr_uView * csr_uModel * vec4(csr_aVertices, 1.0);"
                       "}";

        #ifdef _MSC_VER
            case CSR_ShaderHelper::IEShaderType::IE_ST_Water:
        #else
            case IE_ST_Water:
        #endif
            #ifndef __APPLE__
                return "#version 120\n"
                       "precision mediump float;"
            #else
                return "precision mediump float;"
            #endif
                       ""
                       "attribute vec3 csr_aVertices;\n"
                       "attribute vec2 csr_aTexCoord;\n"
                       ""
                       "varying vec2   csr_vTexCoord;\n"
                       "varying vec3   csr_vFragPos;\n"
                       "varying vec3   csr_vNormal;\n"
                       "varying vec4   csr_vClipSpace;\n"
                       "varying float  csr_vWaveHeight;\n"
                       "varying vec3   csr_vWorldPos;\n"
                       ""
                       "uniform mat4   csr_uModel;\n"
                       "uniform mat4   csr_uView;\n"
                       "uniform mat4   csr_uProjection;\n"
                       "uniform float  csr_uTime;\n"
                       "uniform float  csr_uWaveStrength;\n"
                       ""
                       "void main()\n"
                       "{\n"
                       "    csr_vTexCoord = csr_aTexCoord;\n"
                       "    vec3 pos = csr_aVertices;\n"
                       ""
                       "    // large ocean swells (slow, rolling waves)\n"
                       "    float swell1 = sin(pos.x * 0.15                + csr_uTime * 0.3)  * cos(pos.z * 0.12 + csr_uTime * 0.25);\n"
                       "    float swell2 = sin(pos.x * 0.18 - pos.z * 0.15 + csr_uTime * 0.35) * 0.8;\n"
                       ""
                       "    // medium waves (wind waves)\n"
                       "    float wind1 = sin(pos.x * 0.5 + pos.z * 0.4 + csr_uTime * 0.9) * 0.5;\n"
                       "    float wind2 = sin(pos.x * 0.7 - pos.z * 0.6 + csr_uTime * 1.1) * 0.4;\n"
                       ""
                       "    // small surface ripples\n"
                       "    float ripple1 = sin(pos.x * 2.0 + pos.z * 1.8 + csr_uTime * 1.8) * 0.2;\n"
                       "    float ripple2 = sin(pos.x * 2.8 - pos.z * 2.3 - csr_uTime * 2.1) * 0.15;\n"
                       ""
                       "    // choppy detail\n"
                       "    float chop1 = sin(pos.x * 4.5 + csr_uTime * 2.8) * 0.1;\n"
                       "    float chop2 = sin(pos.z * 5.2 - csr_uTime * 3.2) * 0.08;\n"
                       ""
                       "    // combine all wave layers with proper weighting\n"
                       "    float finalWave = (swell1 * 1.2 + swell2 + wind1 + wind2 +\n"
                       "            ripple1 + ripple2 + chop1 + chop2) * csr_uWaveStrength;\n"
                       "    pos.y          += finalWave;\n"
                       "    csr_vWaveHeight = finalWave;\n"
                       "    csr_vWorldPos   = pos;\n"
                       ""
                       "    // more accurate normal calculation with finer sampling\n"
                       "    float offset = 0.08;\n"
                       ""
                       "    // sample heights around current position\n"
                       "    vec2 offX = vec2(offset, 0.0);\n"
                       "    vec2 offZ = vec2(0.0,    offset);\n"
                       ""
                       "    float hL = (sin((pos.x - offset) * 0.15 + csr_uTime * 0.3) * cos(pos.z * 0.12 + csr_uTime * 0.25) * 1.2 +\n"
                       "                sin((pos.x - offset) * 0.18 - pos.z * 0.15 + csr_uTime * 0.35) * 0.8 +\n"
                       "                sin((pos.x - offset) * 0.5  + pos.z * 0.4  + csr_uTime * 0.9)  * 0.5 +\n"
                       "                sin((pos.x - offset) * 0.7  - pos.z * 0.6  + csr_uTime * 1.1)  * 0.4 +\n"
                       "                sin((pos.x - offset) * 2.0  + pos.z * 1.8  + csr_uTime * 1.8)  * 0.2 +\n"
                       "                sin((pos.x - offset) * 4.5                 + csr_uTime * 2.8)  * 0.1) * csr_uWaveStrength;\n"
                       ""
                       "    float hR = (sin((pos.x + offset) * 0.15 + csr_uTime * 0.3) * cos(pos.z * 0.12 + csr_uTime * 0.25) * 1.2 +\n"
                       "                sin((pos.x + offset) * 0.18 - pos.z * 0.15 + csr_uTime * 0.35) * 0.8 +\n"
                       "                sin((pos.x + offset) * 0.5  + pos.z * 0.4  + csr_uTime * 0.9)  * 0.5 +\n"
                       "                sin((pos.x + offset) * 0.7  - pos.z * 0.6  + csr_uTime * 1.1)  * 0.4 +\n"
                       "                sin((pos.x + offset) * 2.0  + pos.z * 1.8  + csr_uTime * 1.8)  * 0.2 +\n"
                       "                sin((pos.x + offset) * 4.5                 + csr_uTime * 2.8)  * 0.1) * csr_uWaveStrength;\n"
                       ""
                       "    float hD = (sin(pos.x * 0.15 + csr_uTime * 0.3) * cos((pos.z - offset) * 0.12 + csr_uTime * 0.25) * 1.2 +\n"
                       "                sin(pos.x * 0.18 - (pos.z - offset) * 0.15 + csr_uTime * 0.35) * 0.8 +\n"
                       "                sin(pos.x * 0.5  + (pos.z - offset) * 0.4  + csr_uTime * 0.9)  * 0.5 +\n"
                       "                sin(pos.x * 0.7  - (pos.z - offset) * 0.6  + csr_uTime * 1.1)  * 0.4 +\n"
                       "                sin(pos.x * 2.0  + (pos.z - offset) * 1.8  + csr_uTime * 1.8)  * 0.2 +\n"
                       "                sin((pos.z - offset) * 5.2                 - csr_uTime * 3.2)  * 0.08) * csr_uWaveStrength;\n"
                       ""
                       "    float hU = (sin(pos.x * 0.15 + csr_uTime * 0.3) * cos((pos.z + offset) * 0.12 + csr_uTime * 0.25) * 1.2 +\n"
                       "                sin(pos.x * 0.18 - (pos.z + offset) * 0.15 + csr_uTime * 0.35) * 0.8 +\n"
                       "                sin(pos.x * 0.5  + (pos.z + offset) * 0.4  + csr_uTime * 0.9)  * 0.5 +\n"
                       "                sin(pos.x * 0.7  - (pos.z + offset) * 0.6  + csr_uTime * 1.1)  * 0.4 +\n"
                       "                sin(pos.x * 2.0  + (pos.z + offset) * 1.8  + csr_uTime * 1.8)  * 0.2 +\n"
                       "                sin((pos.z + offset) * 5.2                 - csr_uTime * 3.2)  * 0.08) * csr_uWaveStrength;\n"
                       ""
                       "    vec3 tangentX = normalize(vec3(1.0, (hR - hL) / (2.0 * offset), 0.0));\n"
                       "    vec3 tangentZ = normalize(vec3(0.0, (hU - hD) / (2.0 * offset), 1.0));\n"
                       "    csr_vNormal   = normalize(cross(tangentZ, tangentX));\n"
                       ""
                       "    csr_vFragPos   = vec3(csr_uModel * vec4(pos, 1.0));\n"
                       "    csr_vClipSpace = csr_uProjection * csr_uView * vec4(csr_vFragPos, 1.0);\n"
                       "    gl_Position    = csr_vClipSpace;\n"
                       "}";
    }

    return "";
}
//---------------------------------------------------------------------------
std::string CSR_ShaderHelper::GetFragmentShader(IEShaderType type)
{
    switch (type)
    {
        #ifdef _MSC_VER
            case CSR_ShaderHelper::IEShaderType::IE_ST_Color:
        #else
            case IE_ST_Color:
        #endif
            #ifndef __APPLE__
                return "#version 120\n"
                       "precision mediump float;"
            #else
                return "precision mediump float;"
            #endif
                       "varying lowp vec4 csr_vColor;"
                       "void main(void)"
                       "{"
                       "    gl_FragColor = csr_vColor;"
                       "}";

        #ifdef _MSC_VER
            case CSR_ShaderHelper::IEShaderType::IE_ST_Texture:
        #else
            case IE_ST_Texture:
        #endif
            #ifndef __APPLE__
                return "#version 120\n"
                       "precision mediump float;"
            #else
                return "precision mediump float;"
            #endif
                       "uniform      sampler2D csr_sTexture;"
                       "varying lowp vec4      csr_vColor;"
                       "varying      vec2      csr_vTexCoord;"
                       "void main(void)"
                       "{"
                       "    gl_FragColor = csr_vColor * texture2D(csr_sTexture, csr_vTexCoord);"
                       "}";

        #ifdef _MSC_VER
            case CSR_ShaderHelper::IEShaderType::IE_ST_Skybox:
        #else
            case IE_ST_Skybox:
        #endif
            #ifndef __APPLE__
                return "#version 330\n"
                       "precision mediump float;"
                       "uniform samplerCube csr_sCubemap;"
                       "varying vec3        csr_vTexCoord;"
                       "void main()"
                       "{"
                       "    gl_FragColor = texture(csr_sCubemap, csr_vTexCoord);"
                       "}";
            #else
                return "precision mediump float;"
                       "uniform samplerCube csr_sCubemap;"
                       "varying vec3        csr_vTexCoord;"
                       "void main()"
                       "{"
                       "    gl_FragColor = textureCube(csr_sCubemap, csr_vTexCoord);"
                       "}";
            #endif

        #ifdef _MSC_VER
            case CSR_ShaderHelper::IEShaderType::IE_ST_Line:
        #else
            case IE_ST_Line:
        #endif
            #ifndef __APPLE__
                return "#version 120\n"
                       "precision mediump float;"
            #else
                return "precision mediump float;"
            #endif
                       "varying lowp vec4 csr_vColor;"
                       "void main(void)"
                       "{"
                       "    gl_FragColor = csr_vColor;"
                       "}";

        #ifdef _MSC_VER
            case CSR_ShaderHelper::IEShaderType::IE_ST_Water:
        #else
            case IE_ST_Water:
        #endif
            #ifndef __APPLE__
                return "#version 120\n"
                       "precision mediump float;"
            #else
                return "precision mediump float;"
            #endif
                       ""
                       "varying vec2  csr_vTexCoord;\n"
                       "varying vec3  csr_vFragPos;\n"
                       "varying vec3  csr_vNormal;\n"
                       "varying float csr_vWaveHeight;\n"
                       "varying vec3  csr_vWorldPos;\n"
                       ""
                       "uniform float csr_uTime;\n"
                       "uniform vec3  csr_CameraPos;\n"
                       "uniform vec3  csr_LightDir;\n"
                       "uniform vec3  csr_WaterColor;\n"
                       "uniform vec3  csr_DeepWaterColor;\n"
                       "uniform float csr_WaterClearness;\n"
                       ""
                       "float hash(vec2 p)\n"
                       "{\n"
                       "    p = fract(p * vec2(443.897, 441.423));\n"
                       "    p += dot(p, p.yx + 19.19);\n"
                       "    return fract(p.x * p.y);\n"
                       "}\n"
                       ""
                       "float noise(vec2 p)\n"
                       "{\n"
                       "    vec2 i = floor(p);\n"
                       "    vec2 f = fract(p);\n"
                       ""
                       "    // quintic interpolation for smoother results\n"
                       "    f = f * f * f * (f * (f * 6.0 - 15.0) + 10.0);\n"
                       ""
                       "    float a = hash(i);\n"
                       "    float b = hash(i + vec2(1.0, 0.0));\n"
                       "    float c = hash(i + vec2(0.0, 1.0));\n"
                       "    float d = hash(i + vec2(1.0, 1.0));\n"
                       ""
                       "    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);\n"
                       "}\n"
                       ""
                       "// high quality FBM with 5 octaves\n"
                       "float fbm(vec2 p)\n"
                       "{\n"
                       "    float value     = 0.0;\n"
                       "    float amplitude = 0.5;\n"
                       "    float frequency = 1.0;\n"
                       ""
                       "    for (int i = 0; i < 5; ++i)\n"
                       "    {\n"
                       "        value     += amplitude * noise(p * frequency);\n"
                       "        frequency *= 2.1;\n"
                       "        amplitude *= 0.48;\n"
                       "    }\n"
                       "    return value;\n"
                       "}\n"
                       ""
                       "float caustics(vec2 uv, float csr_uTime)\n"
                       "{\n"
                       "    vec2  p  = uv * 4.0;\n"
                       ""
                       "    float c1 = fbm(p       + csr_uTime * 0.08);\n"
                       "    float c2 = fbm(p * 1.3 - csr_uTime * 0.1 + vec2(100.0, 100.0));\n"
                       "    float c3 = fbm(p * 1.7 + csr_uTime * 0.06);\n"
                       ""
                       "    float caustic = c1 * c2 + c3 * 0.5;\n"
                       "    caustic       = pow(max(caustic, 0.0), 3.0);\n"
                       ""
                       "    return caustic * 2.5;\n"
                       "}\n"
                       ""
                       "void main()\n"
                       "{\n"
                       "    vec3 norm    = normalize(csr_vNormal);\n"
                       "    vec3 viewDir = normalize(csr_CameraPos - csr_vFragPos);\n"
                       ""
                       "    // physically accurate Fresnel (Schlick's approximation)\n"
                       "    float F0      = 0.02; // water's reflectance at normal incidence\n"
                       "    float NdotV   = max(dot(norm, viewDir), 0.0);\n"
                       "    float fresnel = F0 + (1.0 - F0) * pow(1.0 - NdotV, 5.0);\n"
                       ""
                       "    // enhanced surface distortion for realism\n"
                       "    vec2 distortion1 = vec2(\n"
                       "        fbm(csr_vTexCoord * 2.5 + csr_uTime * 0.03),\n"
                       "        fbm(csr_vTexCoord * 2.5 + csr_uTime * 0.03 + 100.0)\n"
                       "    ) * 0.04;\n"
                       ""
                       "    vec2 distortion2 = vec2(\n"
                       "        noise(csr_vTexCoord * 5.0 - csr_uTime * 0.05 + 50.0),\n"
                       "        noise(csr_vTexCoord * 5.0 - csr_uTime * 0.05 + 150.0)\n"
                       "    ) * 0.02;\n"
                       ""
                       "    vec2 distortedCoord = csr_vTexCoord + distortion1 + distortion2;\n"
                       ""
                       "    // improved caustics\n"
                       "    float causticsEffect = caustics(distortedCoord, csr_uTime);\n"
                       ""
                       "    // realistic foam on wave crests\n"
                       "    float foamThreshold = 0.35;\n"
                       "    float foamAmount    = smoothstep(foamThreshold, foamThreshold + 0.3, csr_vWaveHeight);\n"
                       "    float foamPattern   = fbm(csr_vWorldPos.xz * 3.0 + csr_uTime * 0.5) * 0.5 + 0.5;\n"
                       "    float foam          = foamAmount * foamPattern * 0.6;\n"
                       ""
                       "    // sun lighting\n"
                       "    vec3 lightDirection = normalize(-csr_LightDir);\n"
                       "    float diffuse       = max(dot(norm, lightDirection), 0.0);\n"
                       ""
                       "    // enhanced specular with sun color\n"
                       "    vec3 halfDir    = normalize(lightDirection + viewDir);\n"
                       "    float specAngle = max(dot(norm, halfDir), 0.0);\n"
                       "    float specular  = pow(specAngle, 256.0); // sharper highlights\n"
                       ""
                       "    // stronger specular on peaks\n"
                       "    specular *= (1.0 + csr_vWaveHeight * 2.0);\n"
                       ""
                       "    // subsurface scattering (light through wave peaks)\n"
                       "    vec3  H           = normalize(lightDirection + norm * 0.4);\n"
                       "    float subsurface  = pow(clamp(dot(viewDir, -H), 0.0, 1.0), 4.0) * 0.6;\n"
                       "    subsurface       *= max(0.0, csr_vWaveHeight); // only on elevated waves\n"
                       ""
                       "    // depth-based color with more realistic variation\n"
                       "    float depthFactor = mix(0.2, 1.0, fresnel);\n"
                       ""
                       "    // darker, more realistic water colors\n"
                       "    vec3 shallowColor = csr_WaterColor     * vec3(0.8, 1.0, 1.1);   // slight cyan tint\n"
                       "    vec3 deepColor    = csr_DeepWaterColor * vec3(0.7, 0.85, 0.95); // deep blue-green\n"
                       ""
                       "    // mix based on depth and viewing angle\n"
                       "    vec3 baseColor = mix(shallowColor, deepColor, depthFactor);\n"
                       ""
                       "    // darken water in troughs, brighten on peaks\n"
                       "    baseColor *= mix(0.7, 1.3, smoothstep(-0.5, 0.5, csr_vWaveHeight));\n"
                       ""
                       "    // sun color (warm yellow-white)\n"
                       "    vec3 sunColor = vec3(1.0, 0.95, 0.85);\n"
                       ""
                       "    // ambient light (darker for more realistic look)\n"
                       "    vec3 ambient = baseColor * 0.3;\n"
                       ""
                       "    // diffuse lighting\n"
                       "    vec3 diffuseLight = baseColor * sunColor * diffuse * 0.6;\n"
                       ""
                       "    // specular highlights (sun reflections)\n"
                       "    vec3 specularLight = sunColor * specular * 2.5 * fresnel;\n"
                       ""
                       "    // subsurface scattering (light through waves)\n"
                       "    vec3 subsurfaceLight = vec3(0.1, 0.4, 0.5) * subsurface;\n"
                       ""
                       "    // caustics (underwater light patterns) - only visible at certain angles\n"
                       "    vec3 causticsLight = vec3(0.5, 0.7, 0.9) * causticsEffect * 0.15 * (1.0 - fresnel);\n"
                       ""
                       "    // foam (white caps)\n"
                       "    vec3 foamColor = vec3(1.0, 1.0, 1.0) * foam;\n"
                       ""
                       "    // combine all lighting\n"
                       "    vec3 color = ambient + diffuseLight + specularLight + subsurfaceLight + causticsLight + foamColor;\n"
                       ""
                       "    // add slight color variation based on viewing angle (atmospheric perspective)\n"
                       "    vec3 skyTint = vec3(0.7, 0.85, 1.0);\n"
                       "    color        = mix(color, color * skyTint, fresnel * 0.2);\n"
                       ""
                       "    // enhanced edge darkening (water appears darker at grazing angles in real life)\n"
                       "    color *= mix(0.8, 1.0, NdotV * 0.5 + 0.5);\n"
                       ""
                       "    // dynamic alpha for realistic transparency\n"
                       "    float alpha = mix(csr_WaterClearness * 0.7, 0.98, fresnel);\n"
                       "    alpha       = mix(alpha, 1.0, foam); // Foam is opaque\n"
                       ""
                       "    gl_FragColor = vec4(color, alpha);\n"
                       "}";
    }

    return "";
}
//---------------------------------------------------------------------------
