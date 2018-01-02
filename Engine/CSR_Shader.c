/****************************************************************************
 * ==> CSR_Shader ----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the shader functions                  *
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

    // initialize the vertex buffer content
    pShader->m_ProgramID    = 0;
    pShader->m_VertexID     = 0;
    pShader->m_FragmentID   = 0;
    pShader->m_VertexSlot   = 0;
    pShader->m_NormalSlot   = 0;
    pShader->m_TexCoordSlot = 0;
    pShader->m_ColorSlot    = 0;
    pShader->m_ModelSlot    = 0;

    return pShader;
}
//---------------------------------------------------------------------------
CSR_Shader* csrShaderFileLoad(const char* pVertex, const char* pFragment)
{
    CSR_Buffer* pVertexProgram;
    CSR_Buffer* pFragmentProgram;
    CSR_Shader* pShader;

    // open the vertex file to load
    pVertexProgram = csrFileOpen(pVertex);

    // succeeded?
    if (pVertexProgram)
        return 0;

    // open the fragment file to load
    pFragmentProgram = csrFileOpen(pFragment);

    // succeeded?
    if (pFragmentProgram)
    {
        csrBufferRelease(pVertexProgram);
        return 0;
    }

    // load the shader from opened vertex and fragment programs
    pShader = csrShaderBufferLoad(pVertexProgram, pFragmentProgram);

    // release the program buffers
    csrBufferRelease(pVertexProgram);
    csrBufferRelease(pFragmentProgram);

    return pShader;
}
//---------------------------------------------------------------------------
CSR_Shader* csrShaderBufferLoad(const CSR_Buffer* pVertex, const CSR_Buffer* pFragment)
{
    CSR_Shader* pShader;
    GLint       linkSuccess;

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

    // link shader programs
    glAttachShader(pShader->m_ProgramID, pShader->m_VertexID);
    glAttachShader(pShader->m_ProgramID, pShader->m_FragmentID);
    glLinkProgram(pShader->m_ProgramID);

    // get linker result
    glGetProgramiv(pShader->m_ProgramID, GL_LINK_STATUS, &linkSuccess);

    // succeeded?
    if (linkSuccess == GL_FALSE)
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
    if (!pSource)
        return 0;

    // no shader to add result to?
    if (!pShader)
        return 0;

    // create and compile shader program
    shaderID = glCreateShader(shaderType);
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
