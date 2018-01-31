/****************************************************************************
 * ==> CSR_Shader ----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the shader functions and types        *
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

#include "CSR_Shader.h"

//---------------------------------------------------------------------------
// Shader functions
//---------------------------------------------------------------------------
CSR_Shader* csrShaderCreate(void)
{
    // create a new shader
    CSR_Shader* pShader = (CSR_Shader*)malloc(sizeof(CSR_Shader));

    // succeeded?
    if (!pShader)
        return 0;

    // initialize the shader content
    csrShaderInit(pShader);

    return pShader;
}
//---------------------------------------------------------------------------
void csrShaderRelease(CSR_Shader* pShader)
{
    // no shader to release?
    if (!pShader)
        return;

    // delete the fragment shader
    if (pShader->m_FragmentID)
        glDeleteShader(pShader->m_FragmentID);

    // delete the vertex shader
    if (pShader->m_VertexID)
        glDeleteShader(pShader->m_VertexID);

    // delete the shader program
    if (pShader->m_ProgramID)
        glDeleteProgram(pShader->m_ProgramID);

    // free the shader
    free(pShader);
}
//---------------------------------------------------------------------------
void csrShaderInit(CSR_Shader* pShader)
{
    // no shader to initialize?
    if (!pShader)
        return;

    // initialize the shader content
    pShader->m_ProgramID    =  0;
    pShader->m_VertexID     =  0;
    pShader->m_FragmentID   =  0;
    pShader->m_VertexSlot   = -1;
    pShader->m_NormalSlot   = -1;
    pShader->m_TexCoordSlot = -1;
    pShader->m_TextureSlot  = -1;
    pShader->m_BumpMapSlot  = -1;
    pShader->m_ColorSlot    = -1;
    pShader->m_ModelSlot    = -1;
}
//---------------------------------------------------------------------------
CSR_Shader* csrShaderLoadFromFile(const char* pVertex, const char* pFragment)
{
    CSR_Buffer* pVertexProgram;
    CSR_Buffer* pFragmentProgram;
    CSR_Shader* pShader;

    // open the vertex file to load
    pVertexProgram = csrFileOpen(pVertex);

    // succeeded?
    if (!pVertexProgram)
        return 0;

    // open the fragment file to load
    pFragmentProgram = csrFileOpen(pFragment);

    // succeeded?
    if (!pFragmentProgram)
    {
        csrBufferRelease(pVertexProgram);
        return 0;
    }

    // load the shader from opened vertex and fragment programs
    pShader = csrShaderLoadFromBuffer(pVertexProgram, pFragmentProgram);

    // release the program buffers
    csrBufferRelease(pVertexProgram);
    csrBufferRelease(pFragmentProgram);

    return pShader;
}
//------------------------------------------------------------------------------
CSR_Shader* csrShaderLoadFromStr(const char* pVertex,
                                 size_t      vertexLength,
                                 const char* pFragment,
                                 size_t      fragmentLength)
{
    CSR_Buffer* pVS;
    CSR_Buffer* pFS;
    CSR_Shader* pShader;

    // validate the input
    if (!pVertex || !vertexLength || !pFragment || !fragmentLength)
        return 0;

    // create buffers to contain vertex and fragment programs
    pVS = csrBufferCreate();
    pFS = csrBufferCreate();

    // copy the vertex program to read
    pVS->m_Length = vertexLength;
    pVS->m_pData  = (unsigned char*)malloc(pVS->m_Length + 1);
    memcpy(pVS->m_pData, pVertex, pVS->m_Length);
    pVS->m_pData[pVS->m_Length] = 0x0;

    // copy the fragment program to read
    pFS->m_Length = fragmentLength;
    pFS->m_pData  = (unsigned char*)malloc(pFS->m_Length + 1);
    memcpy(pFS->m_pData, pFragment, pFS->m_Length);
    pFS->m_pData[pFS->m_Length] = 0x0;

    // compile and build the shader
    pShader = csrShaderLoadFromBuffer(pVS, pFS);

    // release the buffers
    csrBufferRelease(pVS);
    csrBufferRelease(pFS);

    return pShader;
}
//---------------------------------------------------------------------------
CSR_Shader* csrShaderLoadFromBuffer(const CSR_Buffer* pVertex, const CSR_Buffer* pFragment)
{
    CSR_Shader* pShader;

    // source vertex or fragment program is missing?
    if (!pVertex || !pFragment)
        return 0;

    // create a new shader
    pShader = csrShaderCreate();

    // succeeded?
    if (!pShader)
        return 0;

    // create a new shader program
    pShader->m_ProgramID = glCreateProgram();

    // succeeded?
    if (!pShader->m_ProgramID)
    {
        csrShaderRelease(pShader);
        return 0;
    }

    // create and compile the vertex shader
    if (!csrShaderCompile(pVertex, GL_VERTEX_SHADER, pShader))
    {
        csrShaderRelease(pShader);
        return 0;
    }

    // create and compile the fragment shader
    if (!csrShaderCompile(pFragment, GL_FRAGMENT_SHADER, pShader))
    {
        csrShaderRelease(pShader);
        return 0;
    }

    // attach compiled programs with shader
    glAttachShader(pShader->m_ProgramID, pShader->m_VertexID);
    glAttachShader(pShader->m_ProgramID, pShader->m_FragmentID);

    // link shader
    if (!csrShaderLink(pShader))
    {
        csrShaderRelease(pShader);
        return 0;
    }

    return pShader;
}
//---------------------------------------------------------------------------
int csrShaderCompile(const CSR_Buffer* pSource, GLenum shaderType, CSR_Shader* pShader)
{
    GLuint shaderID;
    GLint  success;

    // no source data to compile?
    if (!pSource || !pSource->m_Length || !pSource->m_pData)
        return 0;

    // no shader to add result to?
    if (!pShader)
        return 0;

    // create a new shader program
    shaderID = glCreateShader(shaderType);

    // succeeded?
    if (!shaderID)
        return 0;

    // compile the shader program
    glShaderSource(shaderID, 1, (GLchar**)&pSource->m_pData, 0);
    glCompileShader(shaderID);

    // get compiler result
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

    // succeeded?
    if (success == GL_FALSE)
    {
        // delete the shader
        glDeleteShader(shaderID);
        return 0;
    }

    // assign the compiled shader to the correct identifier
    switch (shaderType)
    {
        case GL_VERTEX_SHADER:   pShader->m_VertexID   = shaderID; break;
        case GL_FRAGMENT_SHADER: pShader->m_FragmentID = shaderID; break;
        default:                 glDeleteShader(shaderID);         return 0;
    }

    return 1;
}
//---------------------------------------------------------------------------
int csrShaderLink(CSR_Shader* pShader)
{
    GLint success;

    // no shader to link?
    if (!pShader || !pShader->m_ProgramID)
        return 0;

    // link the shader
    glLinkProgram(pShader->m_ProgramID);

    // get linker result
    glGetProgramiv(pShader->m_ProgramID, GL_LINK_STATUS, &success);

    // succeeded?
    if (success == GL_FALSE)
        return 0;

    return 1;
}
//---------------------------------------------------------------------------
void csrShaderEnable(CSR_Shader* pShader)
{
    // no shader to enable?
    if (!pShader)
    {
        // disable all
        glUseProgram(0);
        return;
    }

    // enable the shader
    glUseProgram(pShader->m_ProgramID);
}
//---------------------------------------------------------------------------
void csrShaderAttributeInit(CSR_ShaderAttribute* pSA)
{
    // no shader attribute to initialize?
    if (!pSA)
        return;

    // initialize the shader attribute content
    pSA->m_pName  = 0;
    pSA->m_Index  = 0;
    pSA->m_Length = 0;
}
//---------------------------------------------------------------------------
void csrShaderAttributesInit(CSR_ShaderAttributes* pSA)
{
    // no shader attributes to initialize?
    if (!pSA)
        return;

    // initialize the shader attributes content
    pSA->m_pAttribute = 0;
    pSA->m_Count      = 0;
}
//---------------------------------------------------------------------------
